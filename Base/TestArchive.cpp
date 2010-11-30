/*
 *  ArchiveTest.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/30/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestArchive.h"
#include <BinaryStream.h>
#include <FileSystem.h>


void TestArchive::RunTests() {
    TestFromDisk();
    TestFromData();
}

void TestStuff(IOTarget *target) {
    std::list<std::string>* listing;
    Archive a("test.zip", target);
    listing = a.listing("", true, false);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 98);
    delete listing;

    listing = a.listing("", false, false);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 0);
    delete listing;

    listing = a.listing("", false, true);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 1);
    TASSERTS_EQ(listing->front(), "materials/");
    delete listing;

    listing = a.listing("", true, true);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 106);
    delete listing;

    DataTarget *file = a.open("materials/maps/crossfire2/cubemapdefault.vtf");
    TASSERT_EQ(file->getc(), 'V');
    TASSERT_EQ(file->getc(), 'T');
    TASSERT_EQ(file->getc(), 'F');
    TASSERT_EQ(file->getc(), '\0');
    delete file;
}

void TestArchive::TestFromData() {
    BinaryStream *str = new BinaryStream(FileSystem::GetFile("test.zip"), IOTarget::Read, true);

    int length = str->length();
    unsigned char *data = new unsigned char[length];
    str->read(data, length);
    delete str;

    TestStuff(new DataTarget(data, length));
}

void TestArchive::TestFromDisk() {
    TestStuff(FileSystem::GetFile("test.zip"));
}
