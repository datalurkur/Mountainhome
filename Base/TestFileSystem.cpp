/*
 *  TestFileSystem.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestFileSystem.h"
#include "FileSystem.h"

// TODO Why am I using regular assert instead of my ASSERT macros? I think it had to do with dependencies, but I don't remember.
#include <assert.h>

void TestFileSystem::RunTests() {
    TestExists();
    TestIsDirectory();
    TestExtractDir();
    TestAdjustCurrentDir();
    TestHasLeadingDecorations();
    TestFormatPath();
    TestExtractExtension();
    TestExtractFilename();
    TestLength();
    TestTouchAndDelete();
    TestListing();
}

void TestFileSystem::TestListing() {
    std::list<std::string> *listing;
    std::list<std::string>::iterator itr;

    listing = FileSystem::GetListing("garbage");
    TASSERT(!listing);

    listing = FileSystem::GetListing("test", false, false);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 2);

    itr = listing->begin();
    TASSERT_EQ(*itr, "./test/readTest"); itr++;
    TASSERT_EQ(*itr, "./test/testFile");
    delete listing;

    listing = FileSystem::GetListing("test", true, false);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 3);

    itr = listing->begin();
    TASSERT_EQ(*itr, "./test/deeper/deepest"); itr++;
    TASSERT_EQ(*itr, "./test/readTest"); itr++;
    TASSERT_EQ(*itr, "./test/testFile");
    delete listing;

    listing = FileSystem::GetListing("test", false, true);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 4);

    itr = listing->begin();
    TASSERT_EQ(*itr, "./test/"); itr++;
    TASSERT_EQ(*itr, "./test/deeper/"); itr++;
    TASSERT_EQ(*itr, "./test/readTest"); itr++;
    TASSERT_EQ(*itr, "./test/testFile");
    delete listing;

    listing = FileSystem::GetListing("test", true, true);
    TASSERT(listing);
    TASSERT_EQ(listing->size(), 5);

    itr = listing->begin();
    TASSERT_EQ(*itr, "./test/"); itr++;
    TASSERT_EQ(*itr, "./test/deeper/"); itr++;
    TASSERT_EQ(*itr, "./test/deeper/deepest"); itr++;
    TASSERT_EQ(*itr, "./test/readTest"); itr++;
    TASSERT_EQ(*itr, "./test/testFile");
    delete listing;
}

void TestFileSystem::TestIsDirectory() {
    TASSERT(FileSystem::IsDirectory("test"));
    TASSERT(FileSystem::IsDirectory("test/"));
    TASSERT(FileSystem::IsDirectory("./test"));
    TASSERT(FileSystem::IsDirectory("./test/"));

    TASSERT(!FileSystem::IsDirectory("testFile"));
    TASSERT(!FileSystem::IsDirectory("testFile/"));
    TASSERT(!FileSystem::IsDirectory("./testFile"));
    TASSERT(!FileSystem::IsDirectory("./testFile/"));

    TASSERT(FileSystem::IsDirectory("./"));
    TASSERT(FileSystem::IsDirectory("."));
    TASSERT(FileSystem::IsDirectory("/"));
    TASSERT(FileSystem::IsDirectory(""));
}

void TestFileSystem::TestExtractDir() {
    string result;
    TASSERTS_EQ(FileSystem::ExtractDirectory("/", result), "/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/u", result), "/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("./", result), "./");
    TASSERTS_EQ(FileSystem::ExtractDirectory("users", result), "./");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/abcde", result), "/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/Users", result), "/Users/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/Users/", result), "/Users/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/Users/s.txt", result), "/Users/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("Users/s.txt", result), "./Users/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/blarhc/another.dd/some", result), "/blarhc/another.dd/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("/blarhc/anothe.d/some.txt", result), "/blarhc/anothe.d/");
    TASSERTS_EQ(FileSystem::ExtractDirectory("testFile", result), "./");
    TASSERTS_EQ(FileSystem::ExtractDirectory("test", result), "./test/");
}

void TestFileSystem::TestExtractExtension() {
    string result;
    TASSERTS_EQ(FileSystem::ExtractExtension("/users", result), "");
    TASSERTS_EQ(FileSystem::ExtractExtension("/users/", result), "");
    TASSERTS_EQ(FileSystem::ExtractExtension("/users.txt", result), "txt");
    TASSERTS_EQ(FileSystem::ExtractExtension("/users.a.b.c", result), "c");
    TASSERTS_EQ(FileSystem::ExtractExtension("/users.a.b.c.", result), "");
}

void TestFileSystem::TestExtractFilename() {
    string result;
    
    TASSERTS_EQ(FileSystem::ExtractFilename("testFile", result), "testFile");
    TASSERTS_EQ(FileSystem::ExtractFilename("/test", result), "test");
    TASSERTS_EQ(FileSystem::ExtractFilename("/test/", result), "");
    TASSERTS_EQ(FileSystem::ExtractFilename("/test/s.txt", result), "s.txt");
    TASSERTS_EQ(FileSystem::ExtractFilename("test", result), "");
    TASSERTS_EQ(FileSystem::ExtractFilename("./test", result), "");
    TASSERTS_EQ(FileSystem::ExtractFilename("C:/blarhc/another.dd/some", result), "some");
    TASSERTS_EQ(FileSystem::ExtractFilename("C:/blarhc/another.dd/some.txt", result), "some.txt");

    // If it sees something it knows is a directory, ignore it.
    TASSERTS_EQ(FileSystem::ExtractFilename("test", result), "");
    TASSERTS_EQ(FileSystem::ExtractFilename("test/testFile", result), "testFile");
}

void TestFileSystem::TestAdjustCurrentDir() {
    // Check default
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./");

    // Check that it appends to the current value if no leading ./ or /
    FileSystem::ChangeDirectory("test");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    // Test that nothing happens if a file is specified.
    FileSystem::ChangeDirectory("testFile");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    // Test that nothing happens if a non-existent something is specified.
    FileSystem::ChangeDirectory("evenmoreso");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    // Test that nothing happens if a non-existent something is specified.
    FileSystem::ChangeDirectory("./evenmoreso");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    FileSystem::ChangeDirectory("./deeper");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    FileSystem::ChangeDirectory("deeper");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/deeper/");

    FileSystem::ChangeDirectory("..");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    FileSystem::ChangeDirectory("deeper/..");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");

    FileSystem::ChangeDirectory("deeper/../..");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./");

    // TODO Should I change this?
    FileSystem::ChangeDirectory("..");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./");

    FileSystem::ClearCurrentDirectory();
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./");
}

void TestFileSystem::TestHasLeadingDecorations() {
    TASSERT( FileSystem::HasLeadingDecorations("./test"));
    TASSERT( FileSystem::HasLeadingDecorations("/test"));
    TASSERT(!FileSystem::HasLeadingDecorations("test"));
}

void TestFileSystem::TestFormatPath() {
    string str = "/Users";
    TASSERTS_EQ(FileSystem::FormatPath(str, false), "/Users/");
    str = "/Users";
    TASSERTS_EQ(FileSystem::FormatPath(str, true) , "/Users/");

    str = "Users";
    TASSERTS_EQ(FileSystem::FormatPath(str, false), "./Users");

    str = "Users";
    TASSERTS_EQ(FileSystem::FormatPath(str, true) , "./Users");

    str = "Users";
    FileSystem::ChangeDirectory("test");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./test/");
    TASSERTS_EQ(FileSystem::FormatPath(str, true) , "./test/Users");

    str = "Users";
    TASSERTS_EQ(FileSystem::FormatPath(str, false), "./Users");

    str = "test";
    TASSERTS_EQ(FileSystem::FormatPath(str, false), "./test/");
    str = "test";
    TASSERTS_EQ(FileSystem::FormatPath(str, true) , "./test/test");

    str = "test";
    FileSystem::ClearCurrentDirectory();
    TASSERTS_EQ(FileSystem::FormatPath(str, true) , "./test/");

    str = "test";
    TASSERTS_EQ(FileSystem::FormatPath(str, false), "./test/");
    TASSERTS_EQ(FileSystem::CurrentDirectory(), "./");
}

void TestFileSystem::TestLength() {
    assert(FileSystem::Exists(  "testFile"));
    TASSERT_EQ(FileSystem::Length(  "testFile"), 12);
    TASSERT_EQ(FileSystem::Length("./testFile"), 12);
    TASSERT_EQ(FileSystem::Length("vnklsvmfkl"), 0 );
}

void TestFileSystem::TestExists() {
    assert(!FileSystem::Exists("readTest"));
    assert(!FileSystem::Exists("/readTest"));
    assert(!FileSystem::Exists("./readTest"));
    assert( FileSystem::Exists("test/readTest"));
    assert(!FileSystem::Exists("/test/readTest"));
    assert( FileSystem::Exists("./test/readTest"));
}

void TestFileSystem::TestTouchAndDelete() {
    string name = "./abcdefg";
    FileSystem::Delete(name);

    // Make sure touch a non-existant file creates one.
    TASSERT(!FileSystem::Exists(name));
    TASSERT(FileSystem::Touch(name));
    TASSERT(FileSystem::Exists(name));

    // Make sure that touching a file doesn't destroy the contents. A simple length
    // check should suffice.
    std::fstream stream(name.c_str(), std::fstream::out);
    stream.write("    ", 4);
    stream.close();

    TASSERT(FileSystem::Exists(name));
    TASSERT_EQ(FileSystem::Length(name), 4);
    TASSERT(!FileSystem::Touch(name));
    TASSERT(FileSystem::Exists(name));
    TASSERT_EQ(FileSystem::Length(name), 4);

    // Test the delete operation to cleanup.
    TASSERT(FileSystem::Delete(name));
    TASSERT(!FileSystem::Exists(name));
    TASSERT(FileSystem::Delete(name)); /// \todo Or should it fail since the file does not exists?
    TASSERT(!FileSystem::Exists(name));
}

