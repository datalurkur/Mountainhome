/*
 *  TextStreamFileTest.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/3/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TextStreamFileTests.h"
#include "TextStream.h"
#include "FileSystem.h"
#include "File.h"
#include <math.h>

void TextStreamFileTests::RunTests() {
    TestTextReadAll();
    TestTextReadLine();
    TestTextReadChunk();
    TestTextReadOperator();
    TestTextWriteOperator();
}

void TextStreamFileTests::TestTextReadAll() {
    string out = "Brent\nWas\nHere.";
    string in;

    std::ofstream fout("./asdflkjh", std::fstream::out);
    fout << out;
    fout.close();

    File *f = FileSystem::GetFile("./asdflkjh");
    TextStream a(f, IOTarget::None);
    TASSERT_EQ(a.position(), -1);
    TASSERT(!a.readAll(in));
    TASSERT_EQ(a.position(), -1);
    TASSERT(f->open(IOTarget::Read));

    TASSERT_EQ(a.position(), 0);
    TASSERT(a.readAll(in));
    TASSERT_EQ(a.position(), 15);
    TASSERTS_EQ(in, out);

    f->close();
    TASSERT(f->deleteFile());
}

void TextStreamFileTests::TestTextReadLine() {
    string out = "Brent\nWas\nHere.";
    string in;

    std::ofstream fout("./asdflkjh", std::fstream::out);
    fout << out;
    fout.close();

    File *f = FileSystem::GetFile("./asdflkjh");
    TextStream a(f, IOTarget::None);
    TASSERT_EQ(a.position(), -1);
    TASSERT(!a.readLine(in));
    TASSERT_EQ(a.position(), -1);
    TASSERT(f->open(IOTarget::Read));
    TASSERT_EQ(a.position(), 0);

    TASSERT(a.readLine(in));
    TASSERTS_EQ(in, "Brent");
    TASSERT_EQ(a.position(), 5);
    TASSERT(!a.atEnd());

    TASSERT(a.readLine(in));
    TASSERTS_EQ(in, "Was");
    TASSERT_EQ(a.position(), 9);
    TASSERT(!a.atEnd());

    TASSERT(a.readLine(in));
    TASSERTS_EQ(in, "Here.");
    TASSERT_EQ(a.position(), 15);
    TASSERT(a.atEnd());

    f->close();
    TASSERT(f->deleteFile());
}

void TextStreamFileTests::TestTextReadChunk() {
    string out = "Int 150\nFloat:1.50\nNegative=-100\nInvalid 1-1\nAlso 1.1.1\n nan inf";
    string in;

    std::ofstream fout("./asdflkjh", std::fstream::out);
    fout << out;
    fout.close();

    File *f = FileSystem::GetFile("./asdflkjh");
    TextStream a(f, IOTarget::Read);
    TASSERT_EQ(a.position(), 0);

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 3);
    TASSERTS_EQ(in, "Int");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 7);
    TASSERTS_EQ(in, "150");

    a.setDelimeter(":\n");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 13);
    TASSERTS_EQ(in, "Float");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 18);
    TASSERTS_EQ(in, "1.50");

    a.setDelimeter("=\n");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 27);
    TASSERTS_EQ(in, "Negative");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 32);
    TASSERTS_EQ(in, "-100");

    a.setDelimeter(" \n");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 40);
    TASSERTS_EQ(in, "Invalid");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 44);
    TASSERTS_EQ(in, "1-1");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 49);
    TASSERTS_EQ(in, "Also");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 55);
    TASSERTS_EQ(in, "1.1.1");

    TASSERT(a.readChunk(in));
    TASSERT_EQ(a.position(), 60);
    TASSERTS_EQ(in, "nan");

    TASSERT(a.readChunk(in));
    TASSERTS_EQ(in, "inf");
    TASSERT_EQ(a.position(), 64);

    TASSERT(!a.readChunk(in));
    TASSERT_EQ(a.position(), -1);
    TASSERT(a.atEnd());

    f->close();
    TASSERT(f->deleteFile());
}

void TextStreamFileTests::TestTextReadOperator() {
    string out = "Int 150\nFloat:1.50\nNegative=-100\nInvalid 1-1\nAlso 1.1.1\n nan inf";
    string in;
    float f;
    int i;

    std::ofstream fout("./asdflkjh", std::fstream::out);
    fout << out;
    fout.close();

    File *file = FileSystem::GetFile("./asdflkjh");
    TextStream a(file, IOTarget::Read);
    TASSERT_EQ(a.position(), 0);

    a >> in;
    TASSERT_EQ(a.position(), 3);
    TASSERTS_EQ(in, "Int");

    a >> i;
    TASSERT_EQ(a.position(), 7);
    TASSERT_EQ(i, 150);

    a.setDelimeter(":\n");

    a >> in;
    TASSERT_EQ(a.position(), 13);
    TASSERTS_EQ(in, "Float");

    a >> f;
    TASSERT_EQ(a.position(), 18);
    TASSERT_EQ(f, 1.5);

    a.setDelimeter("=\n");

    a >> in;
    TASSERT_EQ(a.position(), 27);
    TASSERTS_EQ(in, "Negative");

    a >> i;
    TASSERT_EQ(a.position(), 32);
    TASSERT_EQ(i, -100);

    a.setDelimeter(" \n");

    a >> in;
    TASSERT_EQ(a.position(), 40);
    TASSERTS_EQ(in, "Invalid");

    a >> i;
    TASSERT_EQ(a.position(), 44);
    TASSERT_EQ(i, 1);

    a >> in;
    TASSERT_EQ(a.position(), 49);
    TASSERTS_EQ(in, "Also");

    a >> f;
    TASSERT_EQ(a.position(), 55);
    TASSERT_EQ(f, 1.1f);

    a >> f;
    TASSERT_EQ(a.position(), 60);
    TASSERT(isnan(f));

    a >> f;
    TASSERT_EQ(a.position(), 64);
    TASSERT(isinf(f));

    a >> f;
    TASSERT_EQ(a.position(), -1);
    TASSERT(a.atEnd());

    file->close();
    TASSERT(file->deleteFile());
}

void TextStreamFileTests::TestTextWriteOperator() {
    int i = 150;
    float f = -1.625;
    string str;

    File *file = FileSystem::GetFile("./asdflkjh");
    TextStream a(file, IOTarget::Write);

    // Write out some variables.
    TASSERT_EQ(a.position(), 0);
    a << "Int: ";
    TASSERT_EQ(a.position(), 5);
    a << i;
    TASSERT_EQ(a.position(), 8);
    a << " Float: ";
    TASSERT_EQ(a.position(), 16);
    a << f;
    TASSERT_EQ(a.position(), 22);
    a << "\n";
    TASSERT_EQ(a.position(), 23);

    // Open an ifstream and check what we read in against what we wrote out.
    std::ifstream fin("./asdflkjh", std::fstream::in);
    getline(fin, str);
    TASSERTS_EQ(str, "Int: 150 Float: -1.625");
    getline(fin, str);

    file->close();
    TASSERT(file->deleteFile());
}
