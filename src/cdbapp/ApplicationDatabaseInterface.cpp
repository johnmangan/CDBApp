/*
 * ApplicationDatabaseInterface.cpp
 *
 *  Created on: Jan 18, 2012
 *     Authors: John Mangan <jmangan@eng.ucsd.edu>,
 *              David Srour <dsrour@eng.ucsd.edu>
 */

#include <sstream>
#include <algorithm>
#include <iostream>

#include <zmq.hpp>

#include <json/json.h>
#include <cdbapp/ApplicationDatabaseInterface.h>
#include <cdbapp/UuidGenerator.h>

ApplicationDatabaseInterface::ApplicationDatabaseInterface(void)
{
    mLoadRequestPending = false;
    mConnectedFlag = false;
    mClientUuid = UuidGenerator::GenerateUuid();
    mZmqContext = NULL;
    mZmqDealer = NULL;
}

ApplicationDatabaseInterface::ApplicationDatabaseInterface(std::string const& connectionAddress)
{
    mLoadRequestPending = false;
    mConnectedFlag = false;
    mClientUuid = UuidGenerator::GenerateUuid();
    mZmqContext = NULL;
    mZmqDealer = NULL;

    Connect(connectionAddress);
}

/*virtual*/
ApplicationDatabaseInterface::~ApplicationDatabaseInterface(void)
{
    Disconnect();
}

void
ApplicationDatabaseInterface::Connect(std::string const& connectionAddress)
{
    if (mConnectedFlag)
    {
        std::cerr << "Connect(...) was already called.  Call Disconnect() 1st." << std::endl;
        return;
    }

    mConnectedFlag = true;

    mZmqContext = new zmq::context_t(1);
    mZmqDealer = new zmq::socket_t(*mZmqContext, ZMQ_DEALER);

    mZmqDealer->setsockopt( ZMQ_IDENTITY, mClientUuid.c_str(), mClientUuid.size()+1 );

    try
    {
        mZmqDealer->connect( connectionAddress.c_str() );
    }
    catch (...)
    {
        std::cerr << "Could not connect to address " << connectionAddress << "." << std::endl;
        mConnectedFlag = false;
    }
}

void
ApplicationDatabaseInterface::Disconnect(void)
{
    if (mZmqDealer)
    {
        int linger = 0;
        mZmqDealer->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
        mZmqDealer->close();
        delete mZmqDealer;
        mZmqDealer = NULL;
    }

    if (mZmqContext)
    {
        delete mZmqContext;
        mZmqContext = NULL;
    }

    mConnectedFlag = false;
    mLoadRequestPending = false;
}

void
ApplicationDatabaseInterface::SendSaveMsg( std::list< std::string >& statesToDb )
{
    if (!mConnectedFlag)
    {
        std::cerr << "ApplicationDatabaseInterface::SendSaveMsg(), Not connected to server!  Cannot communicate." << std::endl;
        return;
    }

    std::list< std::string >::iterator it;
    std::string tcp;

    tcp = "{\"msg\":\"SAVE\",\"states\":[";

    for (it = statesToDb.begin(); it != statesToDb.end(); it++)
    {
        if (it != statesToDb.begin())
            tcp += ",";

        tcp += *it;

    }
    tcp += "]}";

    //std::cout << "Sending via TCP/IP:" << std::endl << tcp << std::endl << std::endl;

    if (mZmqDealer)
    {
        zmq::message_t msg(tcp.size());
        memcpy((void*)msg.data(), tcp.c_str(), tcp.size());
        mZmqDealer->send(msg);
    }
}

void
ApplicationDatabaseInterface::SendLoadMsg(  uint64_t const& timeStampOut,
                                            uint64_t& timeStampIn,
                                            std::list< std::string >& statesBack )
{
    if (!mConnectedFlag)
    {
        std::cerr << "ApplicationDatabaseInterface::SendLoadMsg(), Not connected to server!  Cannot communicate." << std::endl;
        return;
    }

    // Poll for incoming stuff
    zmq::pollitem_t item = { *mZmqDealer, 0, ZMQ_POLLIN, 0 };

    // Process messages from socket
    zmq::message_t message;
    zmq::poll(&item, 1, 0);

    timeStampIn = 0; // Will change if a msg is queued back from DB.

    if (item.revents & ZMQ_POLLIN)
    {
        // Got something back... parse and write to parameters for caller.
        //std::cout << "Got something back: \n";

        zmq::message_t message_bck;
        if ( mZmqDealer->recv(&message_bck, ZMQ_NOBLOCK) )
        {
            ParseIncomingStates(message_bck, timeStampIn, statesBack);
            mLoadRequestPending = false;
        }
    }
    else if (!mLoadRequestPending)
    {
        // Send the LOAD msg request.
        std::stringstream stream;
        std::string time_stmp;
        stream << timeStampOut;
        stream >> time_stmp;

        std::string tcp = "{\"msg\":\"LOAD\",\"timestamp\":" + time_stmp + "}";

        zmq::message_t msg(tcp.size());
        memcpy((void*)msg.data(), tcp.c_str(), tcp.size());
        if (mZmqDealer->send(msg, ZMQ_NOBLOCK))
        {
            //std::cout << "Sent via TCP/IP:" << std::endl << tcp << std::endl << std::endl;
            mLoadRequestPending = true;
        }
    }
}

void
ApplicationDatabaseInterface::ParseIncomingStates(  zmq::message_t& statesMsg,
                                                    uint64_t& timeStampIn,
                                                    std::list< std::string >& statesBack )
{
    std::string json_str((char*)statesMsg.data(), statesMsg.size());

    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(json_str, root))
    {
        std::cerr << "Warning: ApplicationDatabaseInterface::ParseIncomingStates passed an invalid Json string. (Ignoring)" << std::endl;
        return;
    }
    // Get timestamp
    timeStampIn = root["timestamp"].asUInt64();

    // States
    Json::FastWriter writer;
    Json::Value states = root["states"];
    for (int i = 0; i < states.size(); ++i)
    {
       statesBack.push_back(writer.write(states[i]));
    }
}
