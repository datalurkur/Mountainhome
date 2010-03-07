/*
 *  SocketTCP.h
 *  Base
 *
 *  Created by Brent Wilson on 10/3/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SOCKETTCP_H_
#define _SOCKETTCP_H_
#include "IOTarget.h"
#include <string>

#include <netinet/in.h>
#warning This is not working yet!!!

class SocketTCP : public IOTarget {
public:
    enum SocketErrors {
        NoError         = 0,      /*!< No current error.                             */
        CreationError   = 1 << 0, /*!< Error occurred during socket creation.        */
        ConnectionError = 1 << 1  /*!< Error occurred while establishing connection. */
    };

    /*! Creates a SocketTCP based on the given host name and port number.
     * \param hostName The host to connect to.
     * \param portNumber The port to connect to. */
    SocketTCP(const std::string &hostName, int portNumber);

    /*! Creates a SocketTCP based on the given host name and port number and connects to
     *  the server immediately with the given openFlags.
     * \param hostName The host to connect to.
     * \param portNumber The port to connect to.
     * \param openFlags The flags to use while opening the SocketTCP. */
    SocketTCP(const std::string &hostName, int portNumber, OpenMode openFlags);

    /*! Basic SocketTCP destructor. */
    virtual ~SocketTCP();

    /*! \copydoc IOTarget::open */
    virtual bool open(OpenMode openFlags = ReadWrite);

    /*! \copydoc IOTarget::close */
    virtual void close();

    /*! \copydoc IOTarget::isOpen */
    virtual bool isOpen();

    /*! \copydoc IOTarget::bytesLeft */
    virtual long long bytesLeft();

    /*! \copydoc IOTarget::peek */
    virtual char peek();

    /*! \copydoc IOTarget::getc */
    virtual char getc();

    /*! \copydoc IOTarget::read */
    virtual long long read(void* buffer, long long size);

    /*! \copydoc IOTarget::write */
    virtual long long write(const void* buffer, long long size);

    /*! Sets the connection retry information used when opening a connection to a server.
     * \param milliDelay The number of milliseconds between connection attempts.
     * \param times The numbers of time to retry.
     * \param exponential If set to true, the retry attempt delay will increase with each
     *  new attempt. */
    void setConnectionRetry(int milliDelay, int times, bool exponential = true);

public:
    static void Test();

protected:
    int _socket;
    struct sockaddr_in _serverInfo;
    OpenMode _mode;
    bool _open;
    int _error;

    int _times;
    int _milliDelay;
    bool _exponentialIncrease;

protected:
    /*! Creates a socket with the given id and client info.
     * \param socket The socket ID.
     * \param clientInfo The client info. */
    SocketTCP(int socket, struct sockaddr_in clientInfo);
    friend class ServerTCP;

    /*! Handles the common constructor functionality.
     * \param hostName The hostname to connect to.
     * \param portNumber The port number to connect to. */
    void init(const std::string &hostName, int portNumber);

};

#endif
