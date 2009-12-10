/*
 *  TestSocketTCP.cpp
 *  System
 *
 *  Created by loch on 10/23/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestSocketTCP.h"
#include "SocketTCP.h"
#include "ServerTCP.h"

void TestSocketTCP::RunTests() {
//    TestBasics();
//    TestPeekGetc();
//    TestRead();
}

void TestSocketTCP::TestBasics() {
    int port = 31234;
    SocketTCP *sock = new SocketTCP("127.0.0.1", port);
    ServerTCP::SpawnOneShotServer(port, 1, 0, "");

    TASSERT(!sock->isOpen());
    TASSERT( sock->open(IOTarget::Read));
    TASSERT( sock->isOpen());
    TASSERT_EQ(0, sock->bytesLeft());

    // Not allowed on sequential IOTargets.
    TASSERT(!sock->atEnd());
    TASSERT(!sock->seek(0));
    TASSERT_EQ(-1, sock->length());
    TASSERT_EQ(-1, sock->position());

    // Cleanup
    sock->close();
    TASSERT(!sock->isOpen());
    delete sock;
}

void TestSocketTCP::TestPeekGetc() {
    int port = 31234;
    SocketTCP *sock = new SocketTCP("127.0.0.1", port);
    ServerTCP::SpawnOneShotServer(port, 1, 0, "abcdef");
    TASSERT(sock->open(IOTarget::ReadWrite));
    TASSERT(sock->isOpen());

    TASSERT_EQ(sock->bytesLeft(), 6);
    TASSERT_EQ(sock->peek(), 'a');
    TASSERT_EQ(sock->bytesLeft(), 6);
    TASSERT_EQ(sock->getc(), 'a');

    TASSERT_EQ(sock->bytesLeft(), 5);
    TASSERT_EQ(sock->peek(), 'b');
    TASSERT_EQ(sock->bytesLeft(), 5);
    TASSERT_EQ(sock->getc(), 'b');

    TASSERT_EQ(sock->bytesLeft(), 4);
    TASSERT_EQ(sock->peek(), 'c');
    TASSERT_EQ(sock->bytesLeft(), 4);
    TASSERT_EQ(sock->getc(), 'c');

    TASSERT_EQ(sock->bytesLeft(), 3);
    TASSERT_EQ(sock->peek(), 'd');
    TASSERT_EQ(sock->bytesLeft(), 3);
    TASSERT_EQ(sock->getc(), 'd');

    TASSERT_EQ(sock->bytesLeft(), 2);
    TASSERT_EQ(sock->peek(), 'e');
    TASSERT_EQ(sock->bytesLeft(), 2);
    TASSERT_EQ(sock->getc(), 'e');

    TASSERT_EQ(sock->bytesLeft(), 1);
    TASSERT_EQ(sock->peek(), 'f');
    TASSERT_EQ(sock->bytesLeft(), 1);
    TASSERT_EQ(sock->getc(), 'f');

    TASSERT_EQ(sock->bytesLeft(), 0);
    TASSERT_EQ(sock->peek(), 0);
    TASSERT_EQ(sock->bytesLeft(), 0);
    TASSERT_EQ(sock->getc(), 0);
    TASSERT_EQ(sock->bytesLeft(), 0);

    sock->close();
    TASSERT(!sock->isOpen());
    delete sock;
}

void TestSocketTCP::TestRead() {
    int size = sizeof(int) + sizeof(float) + sizeof(double) + sizeof(char);
    char *data = new char[size];
    char* current = data;
    *((int*)current) = 31;
    current += sizeof(int);
    *((float*)current) = -1.0f;
    current += sizeof(float);
    *((double*)current) = 8.5;
    current += sizeof(double);
    *current = ' ';

    int i    = 0;
    float f  = 0;
    double d = 0;
    char c   = 0;

    int port = 31234;
    SocketTCP *sock = new SocketTCP("127.0.0.1", port);
    ServerTCP::SpawnOneShotServer(port, 1, 0, data);
    TASSERT(sock->open(IOTarget::ReadWrite));
    TASSERT(sock->isOpen());

    sock->read((char*)&i, sizeof(int));
    sock->read((char*)&f, sizeof(float));
    sock->read((char*)&d, sizeof(double));
    sock->read((char*)&c, sizeof(char));

    TASSERT_EQ(i, 31);
    TASSERT_EQ(f, -1.0f);
    TASSERT_EQ(d, 8.5);
    TASSERT_EQ(c, ' ');

    sock->close();
    TASSERT(!sock->isOpen());
    delete sock;
}
