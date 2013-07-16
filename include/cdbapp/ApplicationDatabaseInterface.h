/*
 * ApplicationDatabaseInterface.h
 *
 *  Created on: Jan 18, 2012
 *     Authors: John Mangan <jmangan@eng.ucsd.edu>,
 *              David Srour <dsrour@eng.ucsd.edu>
 */

#ifndef APPLICATIONDATABASEINTERFACE_H_
#define APPLICATIONDATABASEINTERFACE_H_

#include <string>
#include <map>
#include <list>
#include <stdint.h>

namespace zmq {
    class message_t;
    class context_t;
    class socket_t;
}

class ApplicationDatabaseInterface
{
public:
    ApplicationDatabaseInterface(void);

    ApplicationDatabaseInterface(std::string const& connectionAddress);

    virtual
    ~ApplicationDatabaseInterface(void);

    void
    Connect(std::string const& connectionAddress);

    void
    Disconnect(void);

    void
    SendSaveMsg( std::list< std::string >& statesToDb );

    /* Requests for changed states since time stamp from epoch.
     *
     * @param timeStampOut  Load request time stamp.
     * @param timeStampIn   Timestamp returned from DB.
     * @param statesBack    Json class of uuid -> object of variables (empty if no msg came back).
     */
    void
    SendLoadMsg(uint64_t const& timeStampOut,
                uint64_t& timeStampIn,
                std::list< std::string >& statesBack);

private:
    void
    ParseIncomingStates(zmq::message_t& statesMsg,
                        uint64_t& timeStampIn,
                        std::list< std::string >& statesBack);

    std::string         mClientUuid;
    bool                mConnectedFlag;
    bool                mLoadRequestPending;

    zmq::context_t*     mZmqContext;
    zmq::socket_t*      mZmqDealer;
};


#endif /* APPLICATIONDATABASEINTERFACE_H_ */
