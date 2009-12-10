/*
 *  TestFile.cpp
 *  System
 *
 *  Created by loch on 10/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestFile.h"

#include "FileSystem.h"
#include "File.h"

void TestFile::RunTests() {
    TestConstructor();
    TestBasics();
    TestStreamPosition();
    TestPeekGetc();
    TestRead();
    TestWrite();
}

void TestFile::TestConstructor() {
    File *a = FileSystem::GetFile("./testFile", IOTarget::None);
    TASSERTS_EQ(a->fullName(), "./testFile");
    TASSERTS_EQ(a->fileName(), "testFile");
    TASSERTS_EQ(a->dirName(), "./");
    TASSERT_EQ(a->length(), 12);
    TASSERT(!a->isOpen());
    TASSERT(a->exists());
    delete a;

    File *b = FileSystem::GetFile("./testFile", IOTarget::None);
    TASSERT(b->open(IOTarget::Read));
    TASSERTS_EQ(b->fullName(), "./testFile");
    TASSERTS_EQ(b->fileName(), "testFile");
    TASSERTS_EQ(b->dirName(), "./");
    TASSERT_EQ(b->length(), 12);
    TASSERT(b->isOpen());
    TASSERT(b->exists());
    delete b;
}

void TestFile::TestBasics() {
    File *a = FileSystem::GetFile("./asdflkjh", IOTarget::None);
    TASSERT(!a->isOpen());
    TASSERT(!a->exists());
    TASSERT(!a->atEnd());
    a->deleteFile(); // Delete here to avoid having to manually delete it for some reason.

//    a._internal.open(a._fullName.c_str(), std::fstream::binary | std::fstream::out);
//    TASSERT(a._internal.is_open());

    TASSERT(a->open(IOTarget::ReadWrite));
    TASSERT(a->isOpen());
    TASSERT(a->exists());
    TASSERT(!a->deleteFile());
    TASSERT(a->exists());
    TASSERT(!a->atEnd());

    a->getc();
    TASSERT(a->atEnd());

    a->close();
    TASSERT(!a->isOpen());
    TASSERT(a->exists());
    TASSERT(a->deleteFile());
    TASSERT(!a->exists());
    delete a;
}

void TestFile::TestPeekGetc() {
    File *a = FileSystem::GetFile("./asdflkjh", IOTarget::ReadWrite);
    TASSERT(a->isOpen());
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->bytesLeft(), 0);
    TASSERT_EQ(a->length(), 0);
    a->write("abcdef", 6);
    TASSERT_EQ(a->position(), 6);
    TASSERT_EQ(a->bytesLeft(), 0);
    TASSERT_EQ(a->length(), 6);

    TASSERT(a->seek(0));
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->bytesLeft(), 6);
    TASSERT_EQ(a->length(), 6);

    TASSERT_EQ(a->peek(), 'a');
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->getc(), 'a');
    TASSERT_EQ(a->position(), 1);

    TASSERT_EQ(a->peek(), 'b');
    TASSERT_EQ(a->position(), 1);
    TASSERT_EQ(a->getc(), 'b');
    TASSERT_EQ(a->position(), 2);

    TASSERT_EQ(a->peek(), 'c');
    TASSERT_EQ(a->position(), 2);
    TASSERT_EQ(a->getc(), 'c');
    TASSERT_EQ(a->position(), 3);

    TASSERT_EQ(a->peek(), 'd');
    TASSERT_EQ(a->position(), 3);
    TASSERT_EQ(a->getc(), 'd');
    TASSERT_EQ(a->position(), 4);

    TASSERT_EQ(a->peek(), 'e');
    TASSERT_EQ(a->position(), 4);
    TASSERT_EQ(a->getc(), 'e');
    TASSERT_EQ(a->position(), 5);

    TASSERT_EQ(a->peek(), 'f');
    TASSERT_EQ(a->position(), 5);
    TASSERT_EQ(a->getc(), 'f');
    TASSERT_EQ(a->position(), 6);

    TASSERT_EQ(a->peek(), -1);
    TASSERT_EQ(a->position(), 6);
    TASSERT_EQ(a->getc(), -1);
    TASSERT_EQ(a->position(), -1);

    // Clean up the temp file.
    a->close();
    TASSERT(a->deleteFile());
    delete a;
}

void TestFile::TestRead() {
    int i    = 31;
    float f  = -1.0f;
    double d = 8.5;
    char c   = ' ';

    // Write out several values to a temp file.
    std::ofstream fout("./asdflkjh", std::fstream::out | std::fstream::binary);
    fout.write((char*)&i, sizeof(int));
    fout.write((char*)&f, sizeof(float));
    fout.write((char*)&d, sizeof(double));
    fout.write((char*)&c, sizeof(char));
    fout.close();

    // Create a file to read in with.
    File *a = FileSystem::GetFile("./asdflkjh", IOTarget::Read);
    int size = 0;
    TASSERT(a->isOpen());
    TASSERT_EQ(a->position(), size);

    // Read in all of the variables we previously wrote out, check the stream pos too.
    // Note that the ISO standard doesn't dictate sizes for int, float, double, etc... so
    // I've avoided making assumptions.
    TASSERT(a->read((char*)&i, sizeof(int)));
    size += sizeof(int);
    TASSERT_EQ(a->position(), size);

    TASSERT(a->read((char*)&f, sizeof(float)));
    size += sizeof(float);
    TASSERT_EQ(a->position(), size);

    TASSERT(a->read((char*)&d, sizeof(double)));
    size += sizeof(double);
    TASSERT_EQ(a->position(), size);

    TASSERT(a->read(&c, sizeof(char)));
    size += sizeof(char);
    TASSERT_EQ(a->position(), size);

    // Assert the readback values fit.
    TASSERT_EQ(i, 31);
    TASSERT_EQ(f, -1.0f);
    TASSERT_EQ(d, 8.5);
    TASSERT_EQ(c, ' ');

    // Kill the temp file.
    a->close();
    TASSERT(a->deleteFile());
    delete a;
}

void TestFile::TestWrite() {
    int i    = 31;
    float f  = -1.0f;
    double d = 8.5;
    char c   = ' ';

    // Create the file and make sure write calls fail until we open it.
    File *a = FileSystem::GetFile("./asdflkjh", IOTarget::None);
    TASSERT(!a->write("vfdvdfv", 7));
    TASSERT(!a->write("asdf", 4));
    TASSERT(a->open(IOTarget::Write));

    // Write some test values to the file and check the stream position progression.
    // Note that the ISO standard doesn't dictate absolute sizes for int, float, double,
    // etc... so I've avoided making assumptions.
    int size = 0;
    TASSERT_EQ(a->position(), size);
    TASSERT(a->write((char*)&i, sizeof(int)));
    size += sizeof(int);

    TASSERT_EQ(a->position(), size);
    TASSERT(a->write((char*)&f, sizeof(float)));
    size += sizeof(float);

    TASSERT_EQ(a->position(), size);
    TASSERT(a->write((char*)&d, sizeof(double)));
    size += sizeof(double);

    TASSERT_EQ(a->position(), size);
    TASSERT(a->write(&c, sizeof(char)));
    size += sizeof(char);

    // Make sure the right ammount of data was written to the file.
    TASSERT_EQ(a->position(), size);
    TASSERT_EQ(a->length(), size);

    i = f = d = c = 0;

    // Open an ifstream and check what we read in against what we wrote out.
    std::ifstream fin("./asdflkjh", std::fstream::in | std::fstream::binary);
    fin.read((char*)&i, sizeof(int));
    fin.read((char*)&f, sizeof(float));
    fin.read((char*)&d, sizeof(double));
    fin.read((char*)&c, sizeof(char));

    TASSERT_EQ(i, 31);
    TASSERT_EQ(f, -1.0f);
    TASSERT_EQ(d, 8.5);
    TASSERT_EQ(c, ' ');
    fin.close();

    // Kill the temp file.
    a->close();
    TASSERT(a->deleteFile());
    delete a;
}

void TestFile::TestStreamPosition() {
    // Create the file and make sure we can't set positions until we open it.
    File *a = FileSystem::GetFile("./asdflkjh", IOTarget::None);
    TASSERT(!a->seek(0));
    TASSERT(a->open(IOTarget::Write));
    a->write("abcdef", 6);

    // Test basic functionality.
    TASSERT(a->seek(0, IOTarget::Beginning));
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->bytesLeft(), 6);

    TASSERT(a->seek(0, IOTarget::End));
    TASSERT_EQ(a->position(), 6);
    TASSERT_EQ(a->bytesLeft(), 0);

    TASSERT(a->seek(-3, IOTarget::End));
    TASSERT_EQ(a->position(), 3);
    TASSERT_EQ(a->bytesLeft(), 3);

    TASSERT(a->seek(1, IOTarget::Current));
    TASSERT_EQ(a->position(), 4);
    TASSERT_EQ(a->bytesLeft(), 2);

    TASSERT(a->seek(-1, IOTarget::Current));
    TASSERT_EQ(a->position(), 3);
    TASSERT_EQ(a->bytesLeft(), 3);

    // Check that negative numbers work.
    TASSERT(a->seek(-1, IOTarget::Current));
    TASSERT_EQ(a->position(), 2);
    TASSERT_EQ(a->bytesLeft(), 4);

    TASSERT(a->seek(1, IOTarget::Current));
    TASSERT_EQ(a->position(), 3);
    TASSERT_EQ(a->bytesLeft(), 3);

    // Test the default value for setting the position.
    TASSERT(a->seek(0));
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->bytesLeft(), 6);

    TASSERT(a->seek(2));
    TASSERT_EQ(a->position(), 2);
    TASSERT_EQ(a->bytesLeft(), 4);

    // Test setting the stream position out of range.
    TASSERT(a->seek(-1));
    TASSERT_EQ(a->position(), 0);
    TASSERT_EQ(a->bytesLeft(), 6);

    TASSERT(a->seek( 7));
    TASSERT_EQ(a->position(), 6);
    TASSERT_EQ(a->bytesLeft(), 0);
    
    // Clean up the temp file.
    a->close();
    TASSERT(a->deleteFile());
    delete a;
}
