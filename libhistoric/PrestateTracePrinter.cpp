/*
Copyright (C) 2023-present, SKALE Labs

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


#ifdef HISTORIC_STATE
#include "PrestateTracePrinter.h"
#include "AlethStandardTrace.h"
#include "FunctionCallRecord.h"
#include "TraceStructuresAndDefs.h"

namespace dev::eth {

    void PrestateTracePrinter::print( Json::Value& _jsonTrace, const ExecutionResult& _er,
        const HistoricState& _statePre, const HistoricState& _statePost ) {
        STATE_CHECK( _jsonTrace.isObject() );
        if ( m_trace.getOptions().prestateDiffMode ) {
            printDiff( _jsonTrace, _er, _statePre, _statePost );
        } else {
            printPre( _jsonTrace, _statePre, _statePost );
        }
    }
    void PrestateTracePrinter::printPre(
        Json::Value& _jsonTrace, const HistoricState& _statePre, const HistoricState& _statePost ) {
        for ( auto&& item : m_trace.getAccessedAccounts() ) {
            printAllAccessedAccountPreValues( _jsonTrace, _statePre, _statePost, item );
        };

        // geth always prints the balance of block miner balance

        Address minerAddress = m_trace.getBlockAuthor();
        u256 minerBalance = getMinerBalancePre( _statePre );
        _jsonTrace[toHexPrefixed( minerAddress )]["balance"] =
            AlethStandardTrace::toGethCompatibleCompactHexPrefixed( minerBalance );
    }


    void PrestateTracePrinter::printDiff( Json::Value& _jsonTrace, const ExecutionResult&,
        const HistoricState& _statePre, const HistoricState& _statePost ) {
        STATE_CHECK( _jsonTrace.isObject() )

        Json::Value preDiff( Json::objectValue );
        Json::Value postDiff( Json::objectValue );

        for ( auto&& item : m_trace.getAccessedAccounts() ) {
            printAccountPreDiff( preDiff, _statePre, _statePost, item );
            printAccountPostDiff( postDiff, _statePre, _statePost, item );
        };


        // now deal with miner balance change as a result of transaction
        // geth always prints miner balance change when NOT in call
        if ( !m_trace.isCall() ) {
            printMinerBalanceChange( _statePre, preDiff, postDiff );
        }

        // we are done, complete the trace JSON

        _jsonTrace["pre"] = preDiff;
        _jsonTrace["post"] = postDiff;
    }
    void PrestateTracePrinter::printMinerBalanceChange(
        const HistoricState& _statePre, Json::Value& preDiff, Json::Value& postDiff ) const {
        Address minerAddress = m_trace.getBlockAuthor();
        u256 minerBalancePre = getMinerBalancePre( _statePre );
        u256 minerBalancePost = getMinerBalancePost( _statePre );

        preDiff[toHexPrefixed( minerAddress )]["balance"] =
            AlethStandardTrace::toGethCompatibleCompactHexPrefixed( minerBalancePre );
        postDiff[toHexPrefixed( minerAddress )]["balance"] =
            AlethStandardTrace::toGethCompatibleCompactHexPrefixed( minerBalancePost );
    }


    // this function returns original values (pre) to result
    void PrestateTracePrinter::printAllAccessedAccountPreValues( Json::Value& _jsonTrace,
        const HistoricState& _statePre, const HistoricState& _statePost, const Address& _address ) {
        STATE_CHECK( _jsonTrace.isObject() )

        Json::Value accountPreValues;
        // if this _address did not exist, we do not include it in the diff
        // unless it is contract deploy
        if ( !_statePre.addressInUse( _address ) && !m_trace.isContractCreation() )
            return;

        auto balance = _statePre.balance( _address );

        // take into account that for calls balance is modified in the state before execution
        if ( m_trace.isCall() && _address == m_trace.getFrom() ) {
            balance = m_trace.getOriginalFromBalance();
        }
        printNonce( _statePre, _statePost, _address, accountPreValues );


        accountPreValues["balance"] = AlethStandardTrace::toGethCompatibleCompactHexPrefixed( balance );

        bytes const& code = _statePre.code( _address );
        if ( code != NullBytes ) {
            accountPreValues["code"] = toHexPrefixed( code );
        }

        Json::Value storagePairs;

        auto& accessedStoragedValues = m_trace.getAccessedStorageValues();

        // now print all storage values that were accessed (written or read) during the transaction
        if ( accessedStoragedValues.count( _address ) ) {
            for ( auto&& storageAddressValuePair : accessedStoragedValues.at( _address ) ) {
                auto& storageAddress = storageAddressValuePair.first;
                auto originalValue = _statePre.originalStorageValue( _address, storageAddress );
                storagePairs[toHexPrefixed( storageAddress )] = toHexPrefixed( originalValue );
                // return limited number of values to prevent DOS attacks
                m_storageValuesReturnedAll++;
                if ( m_storageValuesReturnedAll >= MAX_STORAGE_VALUES_RETURNED )
                    break;
            }
        }

        if ( storagePairs ) {
            accountPreValues["storage"] = storagePairs;
        }

        // if nothing changed we do not add it to the diff
        if ( accountPreValues )
            _jsonTrace[toHexPrefixed( _address )] = accountPreValues;
    }
    void PrestateTracePrinter::printNonce( const HistoricState& _statePre,
        const HistoricState& _statePost, const Address& _address,
        Json::Value& accountPreValues ) const {
        // geth does not print nonce for from address in debug_traceCall;


        if ( m_trace.isCall() && _address == m_trace.getFrom() ) {
            return;
        }

        // handle special case of contract creation transaction
        // in this case geth prints nonce = 1 for the contract
        // that has been created
        if (!_statePre.addressHasCode(_address) && _statePost.addressHasCode(_address)) {
            accountPreValues["nonce"] = 1;
            return;
        }

        // now handle the generic case

        auto preNonce = ( uint64_t ) _statePre.getNonce( _address );
        auto postNonce = ( uint64_t ) _statePost.getNonce( _address );
        // in calls nonce is always printed by geth
        // find out if the address is a contract. Geth always prints nonce for contracts
        auto isContract = _statePre.addressHasCode( _address );
        if ( postNonce != preNonce || m_trace.isCall() || isContract ) {
            accountPreValues["nonce"] = preNonce;
        }
    }

    void PrestateTracePrinter::printAccountPreDiff( Json::Value& _preDiffTrace,
        const HistoricState& _statePre, const HistoricState& _statePost, const Address& _address ) {
        Json::Value value( Json::objectValue );

        // balance diff
        if ( !_statePre.addressInUse( _address ) )
            return;

        auto balance = _statePre.balance( _address );
        if ( m_trace.isCall() && _address == m_trace.getFrom() ) {
            // take into account that for calls balance is modified in the state before execution
            balance = m_trace.getOriginalFromBalance();
            value["balance"] = AlethStandardTrace::toGethCompatibleCompactHexPrefixed( balance );
        } else if ( !_statePost.addressInUse( _address ) ||
                    _statePost.balance( _address ) != balance ) {
            value["balance"] = AlethStandardTrace::toGethCompatibleCompactHexPrefixed( balance );
        }

        auto& code = _statePre.code( _address );
        auto nonce = _statePre.getNonce( _address );

        // geth does not print from nonce in calls
        if ( m_trace.isCall() && _address == m_trace.getFrom() ) {
            // take into account that for calls balance is modified in the state before execution
        } else if ( !_statePost.addressInUse( _address ) || _statePost.getNonce( _address ) != nonce ) {
            value["nonce"] = ( uint64_t ) nonce;
        }
        if ( !_statePost.addressInUse( _address ) || _statePost.code( _address ) != code ) {
            if ( code != NullBytes ) {
                value["code"] = toHexPrefixed( code );
            }
        }

        if ( m_trace.getAccessedStorageValues().find( _address ) !=
             m_trace.getAccessedStorageValues().end() ) {
            Json::Value storagePairs;

            for ( auto&& it : m_trace.getAccessedStorageValues().at( _address ) ) {
                auto& storageAddress = it.first;
                auto& storageValuePost = it.second;
                bool includePair;
                if ( !_statePost.addressInUse( _address ) ) {
                    // contract has been deleted. Include in diff
                    includePair = true;
                } else if ( it.second == 0 ) {
                    // storage has been deleted. Do not include
                    includePair = false;
                } else {
                    includePair =
                        _statePre.originalStorageValue( _address, storageAddress ) != storageValuePost;
                }

                if ( includePair ) {
                    storagePairs[toHex( it.first )] =
                        toHex( _statePre.originalStorageValue( _address, it.first ) );
                    // return limited number of storage pairs to prevent DOS attacks
                    m_storageValuesReturnedPre++;
                    if ( m_storageValuesReturnedPre >= MAX_STORAGE_VALUES_RETURNED )
                        break;
                }
            }

            if ( !storagePairs.empty() )
                value["storage"] = storagePairs;
        }

        if ( !value.empty() )
            _preDiffTrace[toHexPrefixed( _address )] = value;
    }


    void PrestateTracePrinter::printAccountPostDiff( Json::Value& _postDiffTrace,
        const HistoricState& _statePre, const HistoricState& _statePost, const Address& _address ) {
        Json::Value value( Json::objectValue );


        // if this address does not exist post-transaction we dot include it in the trace
        if ( !_statePost.addressInUse( _address ) )
            return;

        auto balancePost = _statePost.balance( _address );
        auto noncePost = _statePost.getNonce( _address );
        auto& codePost = _statePost.code( _address );


        // if the new address, ot if the value changed, include in post trace
        if ( m_trace.isCall() && _address == m_trace.getFrom() ) {
            // geth does not postbalance of from address in calls
        } else if ( !_statePre.addressInUse( _address ) ||
                    _statePre.balance( _address ) != balancePost ) {
            value["balance"] = AlethStandardTrace::toGethCompatibleCompactHexPrefixed( balancePost );
        }
        if ( !_statePre.addressInUse( _address ) || _statePre.getNonce( _address ) != noncePost ) {
            value["nonce"] = ( uint64_t ) noncePost;
        }
        if ( !_statePre.addressInUse( _address ) || _statePre.code( _address ) != codePost ) {
            if ( codePost != NullBytes ) {
                value["code"] = toHexPrefixed( codePost );
            }
        }

        // post diffs for storage values
        if ( m_trace.getAccessedStorageValues().find( _address ) !=
             m_trace.getAccessedStorageValues().end() ) {
            Json::Value storagePairs( Json::objectValue );

            // iterate over all accessed storage values
            for ( auto&& it : m_trace.getAccessedStorageValues().at( _address ) ) {
                auto& storageAddress = it.first;
                auto& storageValue = it.second;

                bool includePair;
                if ( !_statePre.addressInUse( _address ) ) {
                    // a new storage pair created. Include it in post diff
                    includePair = true;
                } else if ( storageValue == 0 ) {
                    // the value has been deleted. We do not include it in post diff
                    includePair = false;
                } else {
                    // see if the storage value has been changed
                    includePair =
                        _statePre.originalStorageValue( _address, storageAddress ) != storageValue;
                }

                if ( includePair ) {
                    storagePairs[toHex( storageAddress )] = toHex( storageValue );
                    // return limited number of storage pairs to prevent DOS attacks
                    m_storageValuesReturnedPost++;
                    if ( m_storageValuesReturnedPost >= MAX_STORAGE_VALUES_RETURNED )
                        break;
                }
            }

            if ( !storagePairs.empty() )
                value["storage"] = storagePairs;
        }

        if ( !value.empty() )
            _postDiffTrace[toHexPrefixed( _address )] = value;
    }


    PrestateTracePrinter::PrestateTracePrinter( AlethStandardTrace& standardTrace )
        : TracePrinter( standardTrace, "prestateTrace" ) {}


    u256 PrestateTracePrinter::getMinerBalancePre( const HistoricState& _statePre ) const {
        auto minerAddress = m_trace.getBlockAuthor();
        auto minerBalance = _statePre.balance( minerAddress );

        if ( m_trace.isCall() && minerAddress == m_trace.getFrom() ) {
            // take into account that for calls balance is modified in the state before execution
            minerBalance = m_trace.getOriginalFromBalance();
        }

        return minerBalance;
    }

    u256 PrestateTracePrinter::getMinerBalancePost( const HistoricState& _statePre ) const {
        auto minerBalance =
            getMinerBalancePre( _statePre ) + m_trace.getTotalGasUsed() * m_trace.getGasPrice();
        return minerBalance;
    }

}  // namespace dev::eth

#endif