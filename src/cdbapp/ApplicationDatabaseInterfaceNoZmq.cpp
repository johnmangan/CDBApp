/*
 * ApplicationDatabaseInterfaceNoZmq.cpp
 *
 * Dummy source file so that users may develop without ZMQ dependency if they
 * do not actually wish to use the project for collaboration (yet).
 *
 *  Created on: Jul 16, 2013
 *      Author: John Mangan <jmangan@eng.ucsd.edu>
 */

#include <iostream>

#include <cdbapp/ApplicationDatabaseInterface.h>
#include <cdbapp/UuidGenerator.h>

namespace zmq {
    class message_t {};
    class context_t {};
    class socket_t {};
}

ApplicationDatabaseInterface::ApplicationDatabaseInterface(void)
{
    mLoadRequestPending = false;
    mConnectedFlag = false;
    mClientUuid = UuidGenerator::GenerateUuid();
    mZmqContext = NULL;
    mZmqDealer = NULL;

    std::cerr << "CDBApp project built without ZMQ. Defaulting to dummy service." << std::endl;
}

ApplicationDatabaseInterface::ApplicationDatabaseInterface(std::string const& connectionAddress)
{
    mLoadRequestPending = false;
    mConnectedFlag = false;
    mClientUuid = UuidGenerator::GenerateUuid();
    mZmqContext = NULL;
    mZmqDealer = NULL;

    Connect(connectionAddress);

    std::cerr << "CDBApp project built without ZMQ. Defaulting to dummy service." << std::endl;
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
}

void
ApplicationDatabaseInterface::Disconnect(void)
{
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
// Commenting out code instead of deleting incase developers wish to test/visualize output.
/*
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
*/
    //std::cout << "Sending via TCP/IP:" << std::endl << tcp << std::endl << std::endl;
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

    timeStampIn = 0; // Will change if a msg is queued back from DB.
}

void
ApplicationDatabaseInterface::ParseIncomingStates(  zmq::message_t& statesMsg,
                                                    uint64_t& timeStampIn,
                                                    std::list< std::string >& statesBack )
{
}

