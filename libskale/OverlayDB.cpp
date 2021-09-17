/*
    Copyright (C) 2018-present, SKALE Labs

    This file is part of skaled.

    skaled is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    skaled is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with skaled.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file OverlayDB.cpp
 * @author Dmytro Stebaiev
 * @date 2018
 */

#include "OverlayDB.h"

#include <thread>

using std::string;
using std::unordered_map;
using std::vector;

#include <libdevcore/Common.h>
#include <libdevcore/db.h>

//#include "SHA3.h"

using dev::bytes;
using dev::bytesConstRef;
using dev::h160;
using dev::h256;
using dev::u256;
using dev::db::Slice;

#ifndef DEV_GUARDED_DB
#define DEV_GUARDED_DB 0
#endif

namespace skale {

namespace slicing {

dev::db::Slice toSlice( dev::h256 const& _h ) {
    return dev::db::Slice( reinterpret_cast< char const* >( _h.data() ), _h.size );
}

dev::db::Slice toSlice( dev::bytes const& _b ) {
    return dev::db::Slice( reinterpret_cast< char const* >( &_b[0] ), _b.size() );
}

dev::db::Slice toSlice( dev::h160 const& _h ) {
    return dev::db::Slice( reinterpret_cast< char const* >( _h.data() ), _h.size );
}

dev::db::Slice toSlice( std::string const& _s ) {
    return dev::db::Slice( reinterpret_cast< char const* >( &_s[0] ), _s.size() );
}

};  // namespace slicing

OverlayDB::OverlayDB( std::unique_ptr< dev::db::DatabaseFace > _db )
    : m_db( _db.release(), []( dev::db::DatabaseFace* db ) {
          // clog(dev::VerbosityDebug, "overlaydb") << "Closing state DB";
          //        std::cerr << "!!! Closing state DB !!!" << std::endl;
          //        std::cerr.flush();
          delete db;
      } ) {}

const OverlayDB::fn_pre_commit_t OverlayDB::g_fn_pre_commit_empty =
    []( std::shared_ptr< dev::db::DatabaseFace > /*db*/,
        std::unique_ptr< dev::db::WriteBatchFace >& /*writeBatch*/ ) {};

dev::h256 OverlayDB::getLastExecutedTransactionHash() const {
    if ( lastExecutedTransactionHash.has_value() )
        return lastExecutedTransactionHash.value();

    dev::h256 shaLastTx;
    if ( m_db ) {
        const std::string l =
            m_db->lookup( skale::slicing::toSlice( "safeLastExecutedTransactionHash" ) );
        if ( !l.empty() )
            shaLastTx = dev::h256( l, dev::h256::FromBinary );
    }

    lastExecutedTransactionHash = shaLastTx;
    return shaLastTx;
}

dev::bytes OverlayDB::getPartialTransactionReceipts() const {
    if ( lastExecutedTransactionReceipts.has_value() )
        return lastExecutedTransactionReceipts.value();

    dev::bytes partialTransactionReceipts;
    if ( m_db ) {
        const std::string l =
            m_db->lookup( skale::slicing::toSlice( "safeLastTransactionReceipts" ) );
        if ( !l.empty() )
            partialTransactionReceipts.insert(
                partialTransactionReceipts.end(), l.begin(), l.end() );
    }

    lastExecutedTransactionReceipts = partialTransactionReceipts;
    return partialTransactionReceipts;
}

void OverlayDB::setLastExecutedTransactionHash( const dev::h256& _newHash ) {
    this->lastExecutedTransactionHash = _newHash;
}
void OverlayDB::setPartialTransactionReceipts( const dev::bytes& _newReceipts ) {
    this->lastExecutedTransactionReceipts = _newReceipts;
}

void OverlayDB::commit() {
    commit( g_fn_pre_commit_empty );
}
void OverlayDB::commit( OverlayDB::fn_pre_commit_t fn_pre_commit ) {
    if ( m_db ) {
        for ( unsigned commitTry = 0; commitTry < 10; ++commitTry ) {
            auto writeBatch = m_db->createWriteBatch();
//      cnote << "Committing nodes to disk DB:";
#if DEV_GUARDED_DB
            DEV_READ_GUARDED( x_this )
#endif
            {
                for ( auto const& addressValuePair : m_cache ) {
                    h160 const& address = addressValuePair.first;
                    bytes const& value = addressValuePair.second;
                    writeBatch->insert(
                        skale::slicing::toSlice( address ), skale::slicing::toSlice( value ) );
                }
                for ( auto const& addressSpacePair : m_auxiliaryCache ) {
                    h160 const& address = addressSpacePair.first;
                    unordered_map< _byte_, bytes > const& spaces = addressSpacePair.second;
                    for ( auto const& spaceValuePair : spaces ) {
                        _byte_ space = spaceValuePair.first;
                        bytes const& value = spaceValuePair.second;

                        writeBatch->insert(
                            skale::slicing::toSlice( getAuxiliaryKey( address, space ) ),
                            skale::slicing::toSlice( value ) );
                    }
                }
                for ( auto const& addressStoragePair : m_storageCache ) {
                    h160 const& address = addressStoragePair.first;
                    unordered_map< h256, h256 > const& storage = addressStoragePair.second;
                    for ( auto const& stateAddressValuePair : storage ) {
                        h256 const& storageAddress = stateAddressValuePair.first;
                        h256 const& value = stateAddressValuePair.second;

                        writeBatch->insert(
                            skale::slicing::toSlice( getStorageKey( address, storageAddress ) ),
                            skale::slicing::toSlice( value ) );
                    }
                }
                writeBatch->insert( skale::slicing::toSlice( "storageUsed" ),
                    skale::slicing::toSlice( storageUsed_.str() ) );

                writeBatch->insert( skale::slicing::toSlice( "safeLastExecutedTransactionHash" ),
                    skale::slicing::toSlice( getLastExecutedTransactionHash() ) );

                writeBatch->insert( skale::slicing::toSlice( "safeLastTransactionReceipts" ),
                    skale::slicing::toSlice( getPartialTransactionReceipts() ) );
            }
            bool bIsPreCommitCallbackPassed = false;
            try {
                if ( fn_pre_commit )
                    fn_pre_commit( m_db, writeBatch );
                bIsPreCommitCallbackPassed = true;
                m_db->commit( std::move( writeBatch ) );
                break;
            } catch ( boost::exception const& ex ) {
                if ( commitTry == 9 ) {
                    cwarn << "Fail(1) writing to state database. Bombing out. "
                          << ( bIsPreCommitCallbackPassed ? "(during DB commit)" :
                                                            "(during pre-commit callback)" );
                    exit( -1 );
                }
                std::cerr << "Error(2) writing to state database (during "
                          << ( bIsPreCommitCallbackPassed ? "DB commit" : "pre-commit callback" )
                          << "): " << boost::diagnostic_information( ex ) << std::endl;
                cwarn << "Error writing to state database: " << boost::diagnostic_information( ex );
                cwarn << "Sleeping for" << ( commitTry + 1 ) << "seconds, then retrying.";
                std::this_thread::sleep_for( std::chrono::seconds( commitTry + 1 ) );
            } catch ( std::exception const& ex ) {
                if ( commitTry == 9 ) {
                    cwarn << "Fail(2) writing to state database. Bombing out. "
                          << ( bIsPreCommitCallbackPassed ? "(during DB commit)" :
                                                            "(during pre-commit callback)" );
                    exit( -1 );
                }
                std::cerr << "Error(2) writing to state database (during "
                          << ( bIsPreCommitCallbackPassed ? "DB commit" : "pre-commit callback" )
                          << "): " << ex.what() << std::endl;
                cwarn << "Error(2) writing to state database: " << ex.what();
                cwarn << "Sleeping for" << ( commitTry + 1 ) << "seconds, then retrying.";
                std::this_thread::sleep_for( std::chrono::seconds( commitTry + 1 ) );
            }
        }
#if DEV_GUARDED_DB
        DEV_WRITE_GUARDED( x_this )
#endif
        {
            m_cache.clear();
            m_auxiliaryCache.clear();
            m_storageCache.clear();
        }
    }
}

string OverlayDB::lookupAuxiliary( h160 const& _address, _byte_ _space ) const {
    string value;
    auto addressSpacePairPtr = m_auxiliaryCache.find( _address );
    if ( addressSpacePairPtr != m_auxiliaryCache.end() ) {
        auto spaceValuePtr = addressSpacePairPtr->second.find( _space );
        if ( spaceValuePtr != addressSpacePairPtr->second.end() ) {
            value = string( spaceValuePtr->second.begin(), spaceValuePtr->second.end() );
        }
    }
    if ( !value.empty() || !m_db )
        return value;

    std::string const loadedValue =
        m_db->lookup( skale::slicing::toSlice( getAuxiliaryKey( _address, _space ) ) );
    if ( loadedValue.empty() )
        cwarn << "Aux not found: " << _address;

    return loadedValue;
}

void OverlayDB::killAuxiliary( const dev::h160& _address, _byte_ _space ) {
    bool cache_hit = false;
    auto spaces_ptr = m_auxiliaryCache.find( _address );
    if ( spaces_ptr != m_auxiliaryCache.end() ) {
        auto value_ptr = spaces_ptr->second.find( _space );
        if ( value_ptr != spaces_ptr->second.end() ) {
            cache_hit = true;
            spaces_ptr->second.erase( value_ptr );
            if ( spaces_ptr->second.empty() ) {
                m_auxiliaryCache.erase( spaces_ptr );
            }
        }
    }
    if ( !cache_hit ) {
        if ( m_db ) {
            bytes key = getAuxiliaryKey( _address, _space );
            if ( m_db->exists( skale::slicing::toSlice( key ) ) ) {
                m_db->kill( skale::slicing::toSlice( key ) );
            } else {
                cnote << "Try to delete non existing key " << _address << "(" << _space << ")";
            }
        }
    }
}

void OverlayDB::insertAuxiliary(
    const dev::h160& _address, dev::bytesConstRef _value, _byte_ _space ) {
    auto address_ptr = m_auxiliaryCache.find( _address );
    if ( address_ptr != m_auxiliaryCache.end() ) {
        auto space_ptr = address_ptr->second.find( _space );
        if ( space_ptr != address_ptr->second.end() ) {
            space_ptr->second = _value.toBytes();
        } else {
            address_ptr->second[_space] = _value.toBytes();
        }
    } else {
        m_auxiliaryCache[_address][_space] = _value.toBytes();
    }
}

std::unordered_map< h160, string > OverlayDB::accounts() const {
    unordered_map< h160, string > accounts;
    if ( m_db ) {
        m_db->forEach( [&accounts]( Slice key, Slice value ) {
            if ( key.size() == h160::size ) {
                // key is account address
                string keyString( key.begin(), key.end() );
                h160 address = h160( keyString, h160::ConstructFromStringType::FromBinary );
                accounts[address] = string( value.begin(), value.end() );
            }
            return true;
        } );
    } else {
        cerror << "Try to load account but connection to database is not established";
    }
    return accounts;
}

std::unordered_map< u256, u256 > OverlayDB::storage( const dev::h160& _address ) const {
    unordered_map< u256, u256 > storage;
    if ( m_db ) {
        m_db->forEach( [&storage, &_address]( Slice key, Slice value ) {
            if ( key.size() == h160::size + h256::size ) {
                // key is storage address
                string keyString( key.begin(), key.end() );
                h160 address = h160(
                    keyString.substr( 0, h160::size ), h160::ConstructFromStringType::FromBinary );
                if ( address == _address ) {
                    h256 memoryAddress = h256(
                        keyString.substr( h160::size ), h256::ConstructFromStringType::FromBinary );
                    u256 memoryValue = h256( string( value.begin(), value.end() ),
                        h256::ConstructFromStringType::FromBinary );
                    storage[memoryAddress] = memoryValue;
                }
            }
            return true;
        } );
    } else {
        cerror << "Try to load account's storage but connection to database is not established";
    }
    return storage;
}

void OverlayDB::rollback() {
#if DEV_GUARDED_DB
    WriteGuard l( x_this );
#endif
    m_cache.clear();
    m_auxiliaryCache.clear();
    m_storageCache.clear();
}

void OverlayDB::clearDB() {
    if ( m_db ) {
        vector< Slice > keys;
        m_db->forEach( [&keys]( Slice key, Slice ) {
            keys.push_back( key );
            return true;
        } );
        for ( const auto& key : keys ) {
            m_db->kill( key );
        }
    }
}

bool OverlayDB::connected() const {
    return m_db != nullptr;
}

bool OverlayDB::empty() const {
    if ( m_db ) {
        bool empty = true;
        m_db->forEach( [&empty]( Slice, Slice ) {
            empty = false;
            return false;
        } );
        return empty;
    } else {
        return true;
    }
}

dev::bytes OverlayDB::getAuxiliaryKey( dev::h160 const& _address, _byte_ space ) const {
    bytes key = _address.asBytes();
    key.push_back( space );  // for aux
    return key;
}

dev::bytes OverlayDB::getStorageKey(
    dev::h160 const& _address, dev::h256 const& _storageAddress ) const {
    bytes key = _address.asBytes();
    bytes storageAddress = _storageAddress.asBytes();
    key.insert( key.end(), storageAddress.begin(), storageAddress.end() );
    return key;
}

string OverlayDB::lookup( h160 const& _h ) const {
    string ret;
    auto p = m_cache.find( _h );
    if ( p != m_cache.end() ) {
        ret = string( p->second.begin(), p->second.end() );
    }
    if ( !ret.empty() || !m_db )
        return ret;

    return m_db->lookup( skale::slicing::toSlice( _h ) );
}

bool OverlayDB::exists( h160 const& _h ) const {
    if ( m_cache.find( _h ) != m_cache.end() )
        return true;
    return m_db && m_db->exists( skale::slicing::toSlice( _h ) );
}

void OverlayDB::kill( h160 const& _h ) {
    auto p = m_cache.find( _h );
    if ( p != m_cache.end() ) {
        m_cache.erase( p );
    } else {
        if ( m_db ) {
            if ( m_db->exists( skale::slicing::toSlice( _h ) ) ) {
                m_db->kill( skale::slicing::toSlice( _h ) );
            } else {
                cnote << "Try to delete non existing key " << _h;
            }
        }
    }
}

void OverlayDB::insert( const dev::h160& _address, dev::bytesConstRef _value ) {
    auto it = m_cache.find( _address );
    if ( it != m_cache.end() ) {
        it->second = _value.toBytes();
    } else
        m_cache[_address] = _value.toBytes();
}

h256 OverlayDB::lookup( const dev::h160& _address, const dev::h256& _storageAddress ) const {
    auto address_ptr = m_storageCache.find( _address );
    if ( address_ptr != m_storageCache.end() ) {
        auto storage_ptr = address_ptr->second.find( _storageAddress );
        if ( storage_ptr != address_ptr->second.end() ) {
            return storage_ptr->second;
        }
    }

    if ( m_db ) {
        string value =
            m_db->lookup( skale::slicing::toSlice( getStorageKey( _address, _storageAddress ) ) );
        return h256( value, h256::ConstructFromStringType::FromBinary );
    } else {
        return h256( 0 );
    }
}

void OverlayDB::insert(
    const dev::h160& _address, const dev::h256& _storageAddress, dev::h256 const& _value ) {
    auto address_ptr = m_storageCache.find( _address );
    if ( address_ptr != m_storageCache.end() ) {
        auto storage_ptr = address_ptr->second.find( _storageAddress );
        if ( storage_ptr != address_ptr->second.end() ) {
            storage_ptr->second = _value;
        } else {
            address_ptr->second[_storageAddress] = _value;
        }
    } else {
        m_storageCache[_address][_storageAddress] = _value;
    }
}

dev::s256 OverlayDB::storageUsed() const {
    if ( m_db ) {
        return dev::s256( m_db->lookup( skale::slicing::toSlice( "storageUsed" ) ) );
    }
    return 0;
}

void OverlayDB::updateStorageUsage( dev::s256 const& _storageUsed ) {
    storageUsed_ = _storageUsed;
}

}  // namespace skale
