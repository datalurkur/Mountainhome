/*
 *  SocketTCP.cpp
 *  System
 *
 *  Created by loch on 10/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SocketTCP.h"
#include "Log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

SocketTCP::SocketTCP(int socket, struct sockaddr_in clientInfo): _socket(socket),
_mode(None), _open(true), _times(5), _milliDelay(500), _exponentialIncrease(true) {
    memcpy(&_serverInfo, &clientInfo, sizeof(_serverInfo));
}

SocketTCP::SocketTCP(const std::string &hostName, int portNumber): _socket(-1),
_mode(None), _open(false), _times(5), _milliDelay(500), _exponentialIncrease(true) {
    init(hostName, portNumber);
}

SocketTCP::SocketTCP(const std::string &hostName, int portNumber, OpenMode openFlags)
:_socket(-1), _mode(None), _open(false) {
    init(hostName, portNumber);
    open(openFlags);
}

SocketTCP::~SocketTCP() {
    if (isOpen()) {
        close();
    }
}

void SocketTCP::setConnectionRetry(int milliDelay, int times, bool exponential) {
    _times = times;
    _milliDelay = milliDelay;
    _exponentialIncrease = exponential;
}

void SocketTCP::init(const std::string &hostName, int portNumber) {
    _socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        _error |= CreationError;
        Error("Could not create socket to" << hostName << ":" << portNumber);
        return;
    }

    memset(&_serverInfo, 0, sizeof(_serverInfo));
    _serverInfo.sin_family = AF_INET;
    _serverInfo.sin_port = htons(portNumber);
    int result = inet_pton(AF_INET, hostName.c_str(), (void*) &_serverInfo.sin_addr); 

    if (result < 0) {
        Error("Invalid address family given to SocketTCP.");
        _error |= CreationError;
        return;
    } else if (result == 0) {
        _error |= CreationError;
        Error("Invalid address given to SocketTCP:" << hostName);
    }
}

bool SocketTCP::open(OpenMode openFlags) {
    _mode = openFlags;
    if (isOpen()) {
        Warn("Socket is already open. Updating mode to" << _mode);
        return true;
    }

    struct timespec sleepInfo;
    sleepInfo.tv_sec  = 0;
    sleepInfo.tv_nsec = _milliDelay * 1000000;
    while (sleepInfo.tv_nsec > 1000000000ll) {
        sleepInfo.tv_nsec -= 1000000000ll;
        sleepInfo.tv_sec  += 1;
    }

    int connected = -1;
    for (int i = 0; i < _times && connected < 0; i++) {
        connected = connect(_socket, (struct sockaddr*)&_serverInfo, sizeof(_serverInfo));
        if (connected < 0) {
            Info("Connection to" << inet_ntoa(_serverInfo.sin_addr) << "failed. Retrying after" <<
                ((sleepInfo.tv_nsec / 1000000) + (sleepInfo.tv_sec * 1000)) << "milliseconds.");
            nanosleep(&sleepInfo, NULL);
            if (_exponentialIncrease) {
                sleepInfo.tv_sec *= 2;
                sleepInfo.tv_nsec *= 2;
                while (sleepInfo.tv_nsec > 1000000000ll) {
                    sleepInfo.tv_nsec -= 1000000000ll;
                    sleepInfo.tv_sec  += 1;
                }
            }
        }
    }

    if (connected < 0) {
        Error("Could not connect to" << inet_ntoa(_serverInfo.sin_addr));
        _error |= ConnectionError;
    }

    _open = connected >= 0;
    return _open;
}

void SocketTCP::close() {
    if (!isOpen()) {
        Warn("Attempting to close a socket that is not opened.");
        return;
    }

    shutdown(_socket, 2);
    ::close(_socket);

    _open = false;
}

bool SocketTCP::isOpen() {
    return _open;
}

long long SocketTCP::bytesLeft() {
    return 0;
}

char SocketTCP::peek() {
    return 0;
}

char SocketTCP::getc() {
    return 0;
}

long long SocketTCP::read(void* buffer, long long size) {
    return 0;
}

long long SocketTCP::write(const void* buffer, long long size) {
    return 0;
}

void Test() {
    
}
