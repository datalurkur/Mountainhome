/*
 *  Test.h
 *  Base
 *
 *  Created by Brent Wilson on 10/15/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _TEST_H_
#define _TEST_H_
#include <list>
#include "Assertion.h"

/*! This class stores all tests that have been loaded into the system and allows the user
 *  to run them will a simple static function call. After the tests have been run the
 *  test stats will be printed out for the user to see. Tests may be added to the system
 *  manually, by calling AddTest. Tests may also be added automatically by subclassing the
 *  Test class.
 * \brief Gives the user a simple interface for running tests.
 * \author Brent Wilson
 * \date Oct 21, 2007 */
class TestRunner {
private:
    /*! Inits the pointer to the test list if it is still null. */
    static void init();

    /*! Cleans up the pointer to the test list at program shutdown. */
    static void deinit();

public:
    typedef void (*FunctionPtr)();           /*!< typedef to a function pointer. */
    typedef std::list<FunctionPtr> TestList; /*!< typedef to a list of func pointers. */
    static TestList *_tests;                 /*!< The list of known tests. */

public:
    /*! Runs all of the tests that have been added to the test list thus far. */
    static void RunAllTests();

    /*! Adds a single test to the list. This happens automatically by subclassing Test. */
    static void AddTest(FunctionPtr);

    /*! Removes a single test to the list. */
    static void RemoveTest(FunctionPtr);

};

/*! This class allows the user to have their tests added automatically to the TestRunner.
 *  Once a class is made a subclass of Test, the subclass's RunTests method will be added
 *  to the TestRunner class automatically. For this to work, the user must pass the sub-
 *  class as the template parameter and have the constructor call the Test<T> constructor
 *  as well. The user must also declare a static void RunTests method. For example:
 *
 *  class Example : public Test<Example> {
 *  public:
 *    Example(): Test<Example>() {}
 *    static void RunTests() {
 *      std::cout << "Running Tests." << std::endl;
 *      RunTestOne();
 *      RunTestTwo();
 *      std::cout << "Finished Running Tests." << std::endl;
 *    }
 *
 *  private:
 *    static void RunTestOne() { std::cout << "  Test One" << std::endl; }
 *    static void RunTestTwo() { std::cout << "  Test Two" << std::endl; }
 *
 *  };
 *
 *  int main(int argc, char *argv[]) {
 *    TestRunner::RunAllTests();
 *  }
 *
 *  Output:
 *  Running Tests.
 *    Test One
 *    Test Two
 *  Finished Running Tests.
 *
 *  As you can see, the RunTests method was never explicitly called by the user, but the
 *  TestRunner knew about the test automatically.
 * \brief A helper classes for automated testing.
 * \author Brent Wilson
 * \date Oct 21, 2007 */
template <typename T>
class Test {
public:
    /*! This is what allows the automatic calling of the constructor. */
    static T _load;

protected:
    /*! Adds the subclasses RunTests method to the TestRunner. */
    Test();

    /*! Destroys the Test. */
    virtual ~Test();

};

template <typename T>
Test<T>::Test() {
    TestRunner::AddTest(T::RunTests);
    _load; // To ensure _load isn't compiled out.
}

template <typename T>
Test<T>::~Test() {
    TestRunner::RemoveTest(T::RunTests);
}

template <typename T>
T Test<T>::_load;

#endif
