/*
 *  BinaryStreamFileTests.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/2/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "BinaryStreamFileTests.h"
#include "BinaryStream.h"
#include "FileSystem.h"
#include "File.h"


void BinaryStreamFileTests::RunTests() {
    TestBinaryReadArray();
    TestBinaryReadOperator();
    TestBinaryWriteString();
    TestBinaryWriteArray();
    TestBinaryWriteOperator();
}

void BinaryStreamFileTests::TestBinaryReadArray() {
    int out[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int  in[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char  c[] = "aaaa\0";

    // Write out a few values to the file.
    std::ofstream fout("./asdflkjh", std::fstream::binary | std::fstream::out);
    fout.write("John", 4);
    fout.write((char*)out, 10 * sizeof(int));
    fout.close();

    // Create the binary file to readback the previously written values with.
    // Make sure the read doesn't work until we open the file.
    File *f = FileSystem::GetFile("./asdflkjh");
    BinaryStream a(f, IOTarget::Read);
    TASSERT_EQ(a.position(), 0);

    // Read in the small string and check its value.
    TASSERT(a.read(c, 4));
    TASSERTS_EQ(c, "John");
    TASSERT_EQ(a.position(), 4);

    // Read in the array of ints and check their values.
    TASSERT(a.read((char*)in, 10 * sizeof(int)));
    TASSERT_EQ(a.position(), 4 + (10 * sizeof(int)));
    for (int i = 0; i < 10; i++) {
        TASSERT_EQ(in[i], out[i]);
    }

    // Kill the temp file.
    f->close();
    TASSERT(f->deleteFile());
}

void BinaryStreamFileTests::TestBinaryReadOperator() {
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

    i = f = d = c = 0;

    // Create a file to read in with.
    File *file = FileSystem::GetFile("./asdflkjh");
    BinaryStream a(file, IOTarget::Read);

    int size = 0;
    TASSERT_EQ(a.position(), size);

    // Read in the various values, checking stream progression along the way.
    // Note that the ISO standard doesn't dictate absolute sizes for int, float, double,
    // etc... so I've avoided making assumptions.
    a >> i;
    size += sizeof(int);
    TASSERT_EQ(a.position(), size);

    a >> f;
    size += sizeof(float);
    TASSERT_EQ(a.position(), size);

    a >> d;
    size += sizeof(double);
    TASSERT_EQ(a.position(), size);

    a >> c;
    size += sizeof(char);
    TASSERT_EQ(a.position(), size);

    // Make sure the values we read back are valid.
    TASSERT_EQ(i, 31);
    TASSERT_EQ(f, -1.0f);
    TASSERT_EQ(d, 8.5);
    TASSERT_EQ(c, ' ');

    // Kill the temp file.
    file->close();
    TASSERT(file->deleteFile());
}

void BinaryStreamFileTests::TestBinaryWriteString() {
    // Create a temp file and make sure we can't write until it's open.
    File *f = FileSystem::GetFile("./asdflkjh");
    BinaryStream a(f, IOTarget::None);
    TASSERT(!a.write("vfdvfd"));
    TASSERT(f->open(IOTarget::Write));

    // Write out some strings to the file.
    TASSERT_EQ(a.position(), 0);
    TASSERT(a.write("John"));
    TASSERT_EQ(a.position(), 4);
    TASSERT(a.write(" Jacob"));
    TASSERT_EQ(a.position(), 10);
    TASSERT(a.write(" Jingleheimer"));
    TASSERT_EQ(a.position(), 23);
    TASSERT(a.write(" Smith\n"));
    TASSERT_EQ(a.position(), 30);
    TASSERT(a.write("asdfghjkl;'\n"));
    TASSERT_EQ(a.position(), 42);

    // Readback and compare the written strings to the read strings.
    string str;
    std::ifstream fin("./asdflkjh", std::fstream::in);
    getline(fin, str);
    TASSERTS_EQ(str, "John Jacob Jingleheimer Smith");
    getline(fin, str);
    TASSERTS_EQ(str, "asdfghjkl;'");
    getline(fin, str);
    TASSERTS_EQ(str, "");
    getline(fin, str);
    TASSERT(fin.eof());
    fin.close();

    // Kill the temp file.
    f->close();
    TASSERT(f->deleteFile());
}

void BinaryStreamFileTests::TestBinaryWriteArray() {
    int out[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int  in[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char  c[] = "aaaa";

    // Create a temp file and make sure we can't write until it's open.
    File *f = FileSystem::GetFile("./asdflkjh");
    BinaryStream a(f, IOTarget::None);
    TASSERT(!a.write("vfdvfd"));
    TASSERT(f->open(IOTarget::Write));

    // Write out some values to the file.
    TASSERT_EQ(a.position(), 0);
    TASSERT(a.write("John", 4));
    TASSERT_EQ(a.position(), 4);
    TASSERT(a.write((char*)out, 10 * sizeof(int)));
    TASSERT_EQ(a.position(), 4 + (10 * sizeof(int)));

    // Readback and compare the written values to the read values.
    string str;
    std::ifstream fin("./asdflkjh", std::fstream::in | std::fstream::binary);
    fin.read(c, 4);
    TASSERTS_EQ(string(c), "John");
    fin.read((char*)in, 10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        TASSERT_EQ(in[i], out[i]);
    }

    // Kill the temp file.
    f->close();
    TASSERT(f->deleteFile());
}

void BinaryStreamFileTests::TestBinaryWriteOperator() {
    int i    = 31;
    float f  = -1.0f;
    double d = 8.5;
    char c   = ' ';

    // Create the file and make sure write calls fail until we open it.
    File *file = FileSystem::GetFile("./asdflkjh");
    BinaryStream a(file, IOTarget::None);
    TASSERT(!a.write("vfdvdfv"));
    TASSERT(!a.write("asdf", 4));
    TASSERT(file->open(IOTarget::Write));

    // Write some test values to the file.
    int size = 0;
    TASSERT_EQ(a.position(), size);
    TASSERT_EQ(a.length(), size);

    a << i;
    size = sizeof(int);
    TASSERT_EQ(a.position(), size);
    TASSERT_EQ(a.length(), size);

    a << f;
    size += sizeof(float);
    TASSERT_EQ(a.position(), size);
    TASSERT_EQ(a.length(), size);

    a << d;
    size += sizeof(double);
    TASSERT_EQ(a.position(), size);
    TASSERT_EQ(a.length(), size);

    a << c;
    size += sizeof(char);
    TASSERT_EQ(a.position(), size);
    TASSERT_EQ(a.length(), size);

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
    file->close();
    TASSERT(file->deleteFile());
}
