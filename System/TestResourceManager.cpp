/*
 *  TestResourceManager.cpp
 *  Base
 *
 *  Created by Brent Wilson on 10/23/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "TestResourceManager.h"
#include "ResourceManager.h"

void TestResourceManager::RunTests() {
    TestResourceAddition();
    TestResourceRetrieval();
}

void TestResourceManager::TestResourceAddition() {
    ResourceManager<Resource> manager;
    TASSERT(manager._namedResources.size() == 0);
    TASSERT(manager._unnamedResources.size() == 0);

    manager.registerResource("name", new Resource);
    TASSERT(manager._namedResources.size() == 1);
    TASSERT(manager._unnamedResources.size() == 0);

    manager.registerResource("", new Resource);
    TASSERT(manager._namedResources.size() == 1);
    TASSERT(manager._unnamedResources.size() == 1);

    manager.clearResources();
    TASSERT(manager._namedResources.size() == 0);
    TASSERT(manager._unnamedResources.size() == 0);
}

void TestResourceManager::TestResourceRetrieval() {
    ResourceManager<Resource> manager;

    Resource *a = new Resource;
    manager.registerResource("name", a);
    TASSERT(manager.getCachedResource("name") == a);
    TASSERT(manager.getCachedResource("unname") == NULL);

    manager.clearResources();
    TASSERT(manager.getCachedResource("name") == NULL);
    TASSERT(manager.getCachedResource("unname") == NULL);
}
