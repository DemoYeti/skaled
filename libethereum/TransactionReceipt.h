/*
    Modifications Copyright (C) 2018 SKALE Labs

    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file TransactionReceipt.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <libdevcore/RLP.h>
#include <libethcore/Common.h>
#include <libethcore/Counter.h>
#include <libethcore/LogEntry.h>

#include <boost/variant/variant.hpp>
#include <array>
#include <string>


namespace dev {
namespace eth {

/// Transaction receipt, constructed either from RLP representation or from individual values.
/// Either a state root or a status code is contained.  m_hasStatusCode is true when it contains a
/// status code. Empty state root is not included into RLP-encoding.
class TransactionReceipt {
public:
    TransactionReceipt( bytesConstRef _rlp );
    TransactionReceipt( h256 const& _root, u256 const& _gasUsed, LogEntries const& _log );
    TransactionReceipt( uint8_t _status, u256 const& _gasUsed, LogEntries const& _log );
    TransactionReceipt( const TransactionReceipt& other ) = default;
    TransactionReceipt& operator=( const TransactionReceipt& other ) = default;

    /// @returns true if the receipt has a status code.  Otherwise the receipt has a state root
    /// (pre-EIP658).
    bool hasStatusCode() const;
    /// @returns the state root.
    /// @throw TransactionReceiptVersionError when the receipt has a status code instead of a state
    /// root.
    h256 const& stateRoot() const;
    /// @returns the status code.
    /// @throw TransactionReceiptVersionError when the receipt has a state root instead of a status
    /// code.
    uint8_t statusCode() const;
    u256 const& cumulativeGasUsed() const { return m_gasUsed; }
    LogBloom const& bloom() const { return m_bloom; }
    LogEntries const& log() const { return m_log; }

    static volatile bool g_bEnableRevertReasonPersistence;
    std::string const& getRevertReason() const { return m_strRevertReason; }
    void setRevertReason( std::string const& strRevertReason ) {
        m_strRevertReason = strRevertReason;
    }

    void streamRLP( RLPStream& _s ) const;

    bytes rlp() const {
        RLPStream s;
        streamRLP( s );
        return s.out();
    }

private:
    boost::variant< uint8_t, h256 > m_statusCodeOrStateRoot;
    u256 m_gasUsed;
    LogBloom m_bloom;
    LogEntries m_log;
    std::string m_strRevertReason;

    Counter< TransactionReceipt > c;

public:
    static uint64_t howMany() { return Counter< TransactionReceipt >::howMany(); }
};

using TransactionReceipts = std::vector< TransactionReceipt >;

std::ostream& operator<<( std::ostream& _out, eth::TransactionReceipt const& _r );

class LocalisedTransactionReceipt : public TransactionReceipt {
public:
    LocalisedTransactionReceipt( TransactionReceipt const& _t, h256 const& _hash,
        h256 const& _blockHash, BlockNumber _blockNumber, unsigned _transactionIndex, Address _from,
        Address _to, u256 const& _gasUsed, Address const& _contractAddress = Address(),
        int _txType = 0, u256 _effectiveGasPrice = 0 )
        : TransactionReceipt( _t ),
          m_hash( _hash ),
          m_blockHash( _blockHash ),
          m_blockNumber( _blockNumber ),
          m_transactionIndex( _transactionIndex ),
          m_from( _from ),
          m_to( _to ),
          m_gasUsed( _gasUsed ),
          m_contractAddress( _contractAddress ),
          m_txType( _txType ),
          m_effectiveGasPrice( _effectiveGasPrice ) {
        LogEntries entries = log();
        for ( unsigned i = 0; i < entries.size(); i++ )
            m_localisedLogs.push_back( LocalisedLogEntry(
                entries[i], m_blockHash, m_blockNumber, m_hash, m_transactionIndex, i ) );
    }
    LocalisedTransactionReceipt( const LocalisedTransactionReceipt& other ) = default;
    LocalisedTransactionReceipt& operator=( const LocalisedTransactionReceipt& other ) = default;

    h256 const& hash() const { return m_hash; }
    h256 const& blockHash() const { return m_blockHash; }
    BlockNumber blockNumber() const { return m_blockNumber; }
    unsigned transactionIndex() const { return m_transactionIndex; }
    Address from() const { return m_from; }
    Address to() const { return m_to; }
    u256 const& gasUsed() const { return m_gasUsed; }
    Address const& contractAddress() const { return m_contractAddress; }
    LocalisedLogEntries const& localisedLogs() const { return m_localisedLogs; };
    int txType() const { return m_txType; }
    u256 effectiveGasPrice() const { return m_effectiveGasPrice; }

private:
    h256 m_hash;
    h256 m_blockHash;
    BlockNumber m_blockNumber;
    unsigned m_transactionIndex = 0;
    Address m_from, m_to;
    u256 m_gasUsed;
    Address m_contractAddress;
    LocalisedLogEntries m_localisedLogs;
    int m_txType;
    u256 m_effectiveGasPrice = 0;

    Counter< TransactionReceipt > c;

public:
    static uint64_t howMany() { return Counter< TransactionReceipt >::howMany(); }
};

}  // namespace eth
}  // namespace dev
