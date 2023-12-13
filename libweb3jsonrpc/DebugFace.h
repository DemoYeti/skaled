/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

#ifndef JSONRPC_CPP_STUB_DEV_RPC_DEBUGFACE_H_
#define JSONRPC_CPP_STUB_DEV_RPC_DEBUGFACE_H_

#include "ModularServer.h"

namespace dev {
namespace rpc {
class DebugFace : public ServerInterface< DebugFace > {
public:
    DebugFace() {
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_accountRangeAt",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param1",
                                    jsonrpc::JSON_STRING, "param2", jsonrpc::JSON_INTEGER, "param3",
                                    jsonrpc::JSON_STRING, "param4", jsonrpc::JSON_INTEGER, NULL ),
            &dev::rpc::DebugFace::debug_accountRangeAtI );
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_traceTransaction",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param1",
                                    jsonrpc::JSON_STRING, "param2", jsonrpc::JSON_OBJECT, NULL ),
            &dev::rpc::DebugFace::debug_traceTransactionI );
        this->bindAndAddMethod(
            jsonrpc::Procedure( "debug_storageRangeAt", jsonrpc::PARAMS_BY_POSITION,
                jsonrpc::JSON_OBJECT, "param1", jsonrpc::JSON_STRING, "param2",
                jsonrpc::JSON_INTEGER, "param3", jsonrpc::JSON_STRING, "param4",
                jsonrpc::JSON_STRING, "param5", jsonrpc::JSON_INTEGER, NULL ),
            &dev::rpc::DebugFace::debug_storageRangeAtI );
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_preimage", jsonrpc::PARAMS_BY_POSITION,
                                    jsonrpc::JSON_OBJECT, "param1", jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_preimageI );
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_traceBlockByNumber",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param1",
                                    jsonrpc::JSON_STRING, "param2", jsonrpc::JSON_OBJECT, NULL ),
            &dev::rpc::DebugFace::debug_traceBlockByNumberI );
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_traceBlockByHash",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_OBJECT, "param1",
                                    jsonrpc::JSON_STRING, "param2", jsonrpc::JSON_OBJECT, NULL ),
            &dev::rpc::DebugFace::debug_traceBlockByHashI );
        this->bindAndAddMethod( jsonrpc::Procedure( "debug_traceCall", jsonrpc::PARAMS_BY_POSITION,
                                    jsonrpc::JSON_OBJECT, "param1", jsonrpc::JSON_OBJECT, "param2",
                                    jsonrpc::JSON_STRING, "param3", jsonrpc::JSON_OBJECT, NULL ),
            &dev::rpc::DebugFace::debug_traceCallI );

        this->bindAndAddMethod(
            jsonrpc::Procedure( "debug_pauseConsensus", jsonrpc::PARAMS_BY_POSITION,
                jsonrpc::JSON_BOOLEAN, "param1", jsonrpc::JSON_BOOLEAN, NULL ),
            &dev::rpc::DebugFace::debug_pauseConsensusI );

        this->bindAndAddMethod(
            jsonrpc::Procedure( "debug_pauseBroadcast", jsonrpc::PARAMS_BY_POSITION,
                jsonrpc::JSON_BOOLEAN, "param1", jsonrpc::JSON_BOOLEAN, NULL ),
            &dev::rpc::DebugFace::debug_pauseBroadcastI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_forceBlock", jsonrpc::PARAMS_BY_POSITION,
                                    jsonrpc::JSON_BOOLEAN, NULL ),
            &dev::rpc::DebugFace::debug_forceBlockI );

        this->bindAndAddMethod(
            jsonrpc::Procedure( "debug_forceBroadcast", jsonrpc::PARAMS_BY_POSITION,
                jsonrpc::JSON_BOOLEAN, "param1", jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_forceBroadcastI );

        this->bindAndAddMethod(
            jsonrpc::Procedure( "debug_interfaceCall", jsonrpc::PARAMS_BY_POSITION,
                jsonrpc::JSON_STRING, "param1", jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_interfaceCallI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getVersion", jsonrpc::PARAMS_BY_POSITION,
                                    jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getVersionI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getArguments",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getArgumentsI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getConfig", jsonrpc::PARAMS_BY_POSITION,
                                    jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getConfigI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getSchainName",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getSchainNameI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getSnapshotCalculationTime",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getSnapshotCalculationTimeI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getSnapshotHashCalculationTime",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getSnapshotHashCalculationTimeI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_doStateDbCompaction",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_doStateDbCompactionI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_doBlocksDbCompaction",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_doBlocksDbCompactionI );

        this->bindAndAddMethod( jsonrpc::Procedure( "debug_getFutureTransactions",
                                    jsonrpc::PARAMS_BY_POSITION, jsonrpc::JSON_STRING, NULL ),
            &dev::rpc::DebugFace::debug_getFutureTransactionsI );
    }
    inline virtual void debug_accountRangeAtI( const Json::Value& request, Json::Value& response ) {
        response = this->debug_accountRangeAt( request[0u].asString(), request[1u].asInt(),
            request[2u].asString(), request[3u].asInt() );
    }
    inline virtual void debug_traceTransactionI(
        const Json::Value& request, Json::Value& response ) {
        response = this->debug_traceTransaction( request[0u].asString(), request[1u] );
    }
    inline virtual void debug_storageRangeAtI( const Json::Value& request, Json::Value& response ) {
        response = this->debug_storageRangeAt( request[0u].asString(), request[1u].asInt(),
            request[2u].asString(), request[3u].asString(), request[4u].asInt() );
    }
    inline virtual void debug_preimageI( const Json::Value& request, Json::Value& response ) {
        response = this->debug_preimage( request[0u].asString() );
    }
    inline virtual void debug_traceBlockByNumberI(
        const Json::Value& request, Json::Value& response ) {
        response = this->debug_traceBlockByNumber( request[0u].asString(), request[1u] );
    }
    inline virtual void debug_traceBlockByHashI(
        const Json::Value& request, Json::Value& response ) {
        response = this->debug_traceBlockByHash( request[0u].asString(), request[1u] );
    }
    inline virtual void debug_traceCallI( const Json::Value& request, Json::Value& response ) {
        response = this->debug_traceCall( request[0u], request[1u].asString(), request[2u] );
    }

    virtual void debug_pauseBroadcastI( const Json::Value& request, Json::Value& response ) {
        this->debug_pauseBroadcast( request[0u].asBool() );
        response = true;  // TODO make void
    }
    virtual void debug_pauseConsensusI( const Json::Value& request, Json::Value& response ) {
        this->debug_pauseConsensus( request[0u].asBool() );
        response = true;  // TODO make void
    }
    virtual void debug_forceBlockI( const Json::Value&, Json::Value& response ) {
        this->debug_forceBlock();
        response = true;  // TODO make void
    }
    virtual void debug_forceBroadcastI( const Json::Value& request, Json::Value& response ) {
        this->debug_forceBroadcast( request[0u].asString() );
        response = true;  // TODO make void
    }
    virtual void debug_interfaceCallI( const Json::Value& request, Json::Value& response ) {
        response = this->debug_interfaceCall( request[0u].asString() );
    }

    virtual void debug_getVersionI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getVersion();
    }

    virtual void debug_getArgumentsI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getArguments();
    }

    virtual void debug_getConfigI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getConfig();
    }

    virtual void debug_getSchainNameI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getSchainName();
    }

    virtual void debug_getSnapshotCalculationTimeI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getSnapshotCalculationTime();
    }

    virtual void debug_getSnapshotHashCalculationTimeI(
        const Json::Value&, Json::Value& response ) {
        response = this->debug_getSnapshotHashCalculationTime();
    }

    virtual void debug_doStateDbCompactionI( const Json::Value&, Json::Value& response ) {
        response = this->debug_doStateDbCompaction();
    }

    virtual void debug_doBlocksDbCompactionI( const Json::Value&, Json::Value& response ) {
        response = this->debug_doBlocksDbCompaction();
    }

    virtual void debug_getFutureTransactionsI( const Json::Value&, Json::Value& response ) {
        response = this->debug_getFutureTransactions();
    }

    virtual Json::Value debug_accountRangeAt(
        const std::string& param1, int param2, const std::string& param3, int param4 ) = 0;
    virtual Json::Value debug_traceTransaction(
        const std::string& param1, const Json::Value& param2 ) = 0;
    virtual Json::Value debug_storageRangeAt( const std::string& param1, int param2,
        const std::string& param3, const std::string& param4, int param5 ) = 0;
    virtual std::string debug_preimage( const std::string& param1 ) = 0;
    virtual Json::Value debug_traceBlockByNumber(
        const std::string& param1, const Json::Value& param2 ) = 0;
    virtual Json::Value debug_traceBlockByHash(
        const std::string& param1, const Json::Value& param2 ) = 0;
    virtual Json::Value debug_traceCall( Json::Value const& _call, std::string const& _blockNumber,
        Json::Value const& _options ) = 0;
    virtual void debug_pauseBroadcast( bool pause ) = 0;
    virtual void debug_pauseConsensus( bool pause ) = 0;
    virtual void debug_forceBlock() = 0;
    virtual void debug_forceBroadcast( const std::string& _transactionHash ) = 0;
    virtual std::string debug_interfaceCall( const std::string& _arg ) = 0;

    virtual std::string debug_getVersion() = 0;
    virtual std::string debug_getArguments() = 0;
    virtual std::string debug_getConfig() = 0;
    virtual std::string debug_getSchainName() = 0;
    virtual uint64_t debug_getSnapshotCalculationTime() = 0;
    virtual uint64_t debug_getSnapshotHashCalculationTime() = 0;

    virtual uint64_t debug_doStateDbCompaction() = 0;
    virtual uint64_t debug_doBlocksDbCompaction() = 0;

    virtual Json::Value debug_getFutureTransactions() = 0;
};

}  // namespace rpc
}  // namespace dev
#endif  // JSONRPC_CPP_STUB_DEV_RPC_DEBUGFACE_H_
