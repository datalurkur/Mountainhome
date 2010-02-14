/*
 *  ServerTCP.h
 *  Base
 *
 *  Created by Brent Wilson on 10/3/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SERVERTCP_H_
#define _SERVERTCP_H_

#include <netinet/in.h>
#warning This is not working yet!!!

class SocketTCP;
class ServerTCP {
public:
    /*! Creates a ServerTCP that starts listening for connections on the given port.
     * \param portNumber The port number to listen on.
     * \param queueLength The number of connections to hold onto. */
    ServerTCP(int portNumber, int queueLength = 50);

    /*! Destroys the ServerTCP */
    virtual ~ServerTCP();

    /*! Returns the next connection in the connection queue.
     * \return A socket representing the next connection. */
    SocketTCP* accept();

private:
    int _socket;
    struct sockaddr_in _serverInfo;

public:
    static void SpawnOneShotServer(int port, int cap, int length, const char *output);

private:
    static void *OneShotServerThread(void *arg);

};

#endif
