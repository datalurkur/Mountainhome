/*
 *  ServerTCP.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/3/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "SocketTCP.h"
#include "ServerTCP.h"
#include "Logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <unistd.h>

class OneShotServer {
public:
    static void* Launch(void *arg) {
        OneShotServer *server = (OneShotServer*)arg;
        server->sendData();
        delete server;
        return NULL;
    }

public:
    OneShotServer(int port, int cap, int length, const char *output)
    : _server(NULL), _dataLength(length), _data(NULL) {
        _server = new ServerTCP(port, cap);
        _data = new char[_dataLength];

        memcpy(_data, output, sizeof(char) * _dataLength);
    }

    ~OneShotServer() {
        delete _server;
        delete[] _data;
    }

    void sendData() {
        SocketTCP *sock = _server->accept();
        sock->write(_data, _dataLength);
        sock->close();
        delete sock;
    }

private:
    ServerTCP* _server;
    int _dataLength;
    char *_data;

};

void ServerTCP::SpawnOneShotServer(int port, int cap, int length, const char *output) {
    pthread_attr_t attributes;
    pthread_t thread;

    OneShotServer *server = new OneShotServer(port, cap, length, output);

    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attributes, OneShotServer::Launch, server);
}

ServerTCP::ServerTCP(int portNumber, int queueLength): _socket(-1) {
    THROW(NotImplementedError, "ServerTCP has not been finished.");

    memset(&_serverInfo, 0, sizeof(struct sockaddr_in));
    _serverInfo.sin_family = AF_INET;
    _serverInfo.sin_port = htons(portNumber);
    _serverInfo.sin_addr.s_addr = htonl(INADDR_ANY);

    _socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        Error("Could not create socket on port " << portNumber);
        return;
    }

    int yes = 1;
    if (bind(_socket, (struct sockaddr*)&_serverInfo, sizeof(_serverInfo)) < 0) {
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
            Error("Could not bind socket to port " << portNumber);
            return;
        }
    }

    if (listen(_socket, queueLength) < 0) {
        Error("Listening on port " << portNumber << " failed.");
        return;
    }
}

ServerTCP::~ServerTCP() {
    close(_socket);
}

SocketTCP* ServerTCP::accept() {
    struct sockaddr_in clientInfo;
    memset(&clientInfo, 0, sizeof(clientInfo));

    socklen_t length = sizeof(clientInfo);
    int connected = ::accept(_socket, (struct sockaddr*)&clientInfo, &length); 
    if (connected < 0) {
        Error("Could not complete connection to client.");
        return NULL;
    }

    return new SocketTCP(connected, clientInfo);
}
