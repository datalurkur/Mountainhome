/*
 *  Assertion.h
 *  3DMath
 *
 *  Created by Brent Wilson on 9/27/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ASSERTION_H_
#define _ASSERTION_H_

#include "Singleton.h"
#include "Base.h"
#include "Logger.h"

// See the class documentation below for more information.

// Basic asserts.
#define ASSERT_EQ(a, b)  ASSERT_CMP(eq, a, b)
#define ASSERT_NE(a, b)  ASSERT_CMP(ne, a, b)
#define ASSERT_GT(a, b)  ASSERT_CMP(gt, a, b)
#define ASSERT_GE(a, b)  ASSERT_CMP(ge, a, b)
#define ASSERT_LT(a, b)  ASSERT_CMP(lt, a, b)
#define ASSERT_LE(a, b)  ASSERT_CMP(le, a, b)
#define ASSERTS_EQ(a, b) ASSERT_CMP(eq, std::string(a), std::string(b))

// Test asserts. This can only be used in void returning functions.
#define TASSERT_EQ(a, b)  TASSERT_CMP(eq, a, b)
#define TASSERT_NE(a, b)  TASSERT_CMP(ne, a, b)
#define TASSERT_GT(a, b)  TASSERT_CMP(gt, a, b)
#define TASSERT_GE(a, b)  TASSERT_CMP(ge, a, b)
#define TASSERT_LT(a, b)  TASSERT_CMP(lt, a, b)
#define TASSERT_LE(a, b)  TASSERT_CMP(le, a, b)
#define TASSERTS_EQ(a, b) TASSERT_CMP(eq, std::string(a), std::string(b))

#define TASSERT(a) do { switch(AssertionHandler::Get()->checkAssertion((a), "", #a, __FILE__, __LINE__)) { case 0: break; case 1: return; case 2: abort(); } } while(0)
#define TASSERT_CMP(func, a, b) do { switch(AssertionHandler::Get()->func((a), (b), "", #a, #b, __FILE__, __LINE__)) { case 0: break; case 1: return; case 2: abort(); } } while(0)

// If DISABLE_ASSERT is defined, all non test asserts will be turned off.
#if !defined(DISABLE_ASSERT)
#   define ASSERT(a) do { switch(AssertionHandler::Get()->checkAssertion((a), "", #a, __FILE__, __LINE__)) { case 0: break; case 1: SoftAssert(); case 2: abort(); } } while(0)
#   define ASSERT_CMP(func, a, b) do { switch(AssertionHandler::Get()->func((a), (b), "", #a, #b, __FILE__, __LINE__)) { case 0: break; case 1: break; case 2: abort(); } } while(0)
#else
#   define ASSERT(a) do {} while(0)
#   define ASSERT_CMP(func, a, b) do {} while(0)
#endif

// Helper macro for creating the comparison functions.
#define MAKE_ASSERT_COMPARISON(name, float, standard) \
template <typename T1, typename T2> inline \
int name( \
    const T1 &valueOne, \
    const T2 &valueTwo, \
    const std::string &message, \
    const std::string &condOne, \
    const std::string &condTwo, \
    const std::string &file, \
    int line \
) { \
    std::string str; \
    if (is_numeric<T1>::value && is_numeric<T2>::value && (is_float<T1>::value || is_float<T2>::value)) { \
        str = float(garbage_cast(valueOne), garbage_cast(valueTwo)) ? "" : #name; \
    } else { \
        str = valueOne standard valueTwo ? "" : #standard; \
    } \
    return checkAssertion(str, valueOne, valueTwo, message, condOne, condTwo, file, line); \
}

/*! This class is a helper class for making assertion calls. The main way of interacting
 *  with this class will be through use of macro functions. These macro functions are
 *  ASSERT_EQ(a, b)  // a == b
 *  ASSERT_NE(a, b)  // a != b
 *  ASSERT_GT(a, b)  // a >  b
 *  ASSERT_GE(a, b)  // a >= b
 *  ASSERT_LT(a, b)  // a <  b
 *  ASSERT_LE(a, b)  // a <= b
 *
 *  An extra set of assertions are available for testing purposes as well. They are
 *  to the previously listed assertions except that they have 'T' prepended to the call.
 *  TASSERT_EQ
 *  TASSERT_NE
 *  ...
 *
 *  There are two main differences between these two sets of functions. First, the regular
 *  assertions may be compiled out by defining DISABLE_ASSERT. By doing this all regular
 *  assertions will be removed, leaving only the test assertions. The other difference is
 *  that the test assertions, assuming the soft assert flag is set, will return from the
 *  function they are called in if they fail, rather than aborting or continuing as
 *  normal.
 *
 *
 *  Assertion passes
 *          | Print | Continue | Return | Abort |
 *  ASSERT  |       |    X     |        |       |
 *  TASSERT |       |    X     |        |       |
 *
 *
 *  Assertion fails with the soft assert flag set
 *          | Print | Continue | Return | Abort |
 *  ASSERT  |   X   |    X     |        |       |
 *  TASSERT |   X   |          |   X    |       |
 *
 *
 *  Assertion fails without the soft assert flag set
 *          | Print | Continue | Return | Abort |
 *  ASSERT  |   X   |          |        |   X   |
 *  TASSERT |   X   |          |        |   X   |
 *
 *  This allows for testing similar to standard testing suites as every test function will
 *  run until an assertion fails and then continue to the next function, making note of
 *  failure.
 *
 *  At any given point in time, the user can check the assertion failure statistics. This
 *  will typically be only useful when the soft assert flag is set and generally most
 *  used for testing purposes (though it can certainly be used during real program
 *  execution).
 *
 *  It is important to note that this is simply a useful tool for testing and does not
 *  attempt to take control of the entire process. Generally the user will be in charge of
 *  calling all of the test methods themselves.
 * \brief A helper class for performing runtime assertions.
 * \author Brent Wilson
 * \date Sep 28, 2007 */
class AssertionHandler : public Singleton<AssertionHandler> {
public:
    /*! This sets or unsets the soft abort flag. The soft abort flag determines if abort()
     *  is called when an assertion fails. An assertion failure will ALWAYS be logged,
     *  even if the soft assert flag is set to true. For regular asserts, a failure will 
     *  result in a note made to the logs and nother else. With test asserts, a failure 
     *  result in a note made to the logs and a return from the current function. If the
     *  soft assert flag is not set, ANY assertion failure will result in a program halt.
     *  Typically soft assert should be set to true when testing or running in a release
     *  environment.
     * \param softAbort The value to set the internal flag to.
     * \return The previous value of the flag. */
    bool setSoftAbort(bool softAbort);

    /*! Returns the assertion statistics for the current execution of the program.
     * \param made The number of assertions made.
     * \param failed The number of assertions failed.
     * \percentage Percentage of assertions passed. (passed / made * 100) */
    void getTestStats(int &made, int &failed, float &percentage);

    /*! Prints out the assertions statistics to the terminal, complete with a green or
     *  red bar. */
    void printTestStats();

    /*! Clears the stored information on passed/failed assertions. */
    void clearTestStats();

public:
    /*! This function is in charge of printing out the various information about an 
     *  assertion call that checks a single boolean value.
     * \note This function is meant to be used only by the assertions macros.
     * \param value The boolean value to check.
     * \param message A user message associated with the assertion.
     * \param condOne The string description of the value.
     * \param file The file the assertion was called from.
     * \param line The line the assertion was called from.
     * \return Returns Returns 0 if no error occurs, 1 if there is a soft failure, and 2
     *         if there is a hard failure. */
    int checkAssertion(
        bool value,
        const std::string &message,
        const std::string &cond,
        const std::string &file,
        int line
    );

    MAKE_ASSERT_COMPARISON(eq, feq, ==);
    MAKE_ASSERT_COMPARISON(ne, fne, !=);
    MAKE_ASSERT_COMPARISON(gt, fgt, > );
    MAKE_ASSERT_COMPARISON(ge, fge, >=);
    MAKE_ASSERT_COMPARISON(lt, flt, < );
    MAKE_ASSERT_COMPARISON(le, fle, <=);

private:
    bool _softAssert;       /*!< The soft assert flag. */
    int _assertionsMade;    /*!< The number of assertions made. */
    int _assertionsFailed;  /*!< The number of assertions failed. */

private:
    /*! Performs the pointer level cast on T when numeric is false.
     * \note A struct is used to allow for partial specialization. */
    template <typename T, bool numeric>
    struct gargabe_caster {
        static Real cast(const T &val) {
            return *((Real*)&val);
        }
    };

    /*! Performs the static cast on T when numeric is true.
     * \note A struct is used to allow for partial specialization. */
    template <typename T>
    struct gargabe_caster<T, true> {
        static Real cast(const T &val) {
            return static_cast<Real>(val);
        }
    };

private:
    /*! This function is in charge of printing out the various information about an 
     *  assertion call that compares two arguments.
     * \note This function is meant to be used only by the assertions macros.
     * \param cmpString The comparison performed. If nothing is given, the assertion is
     *        ignored.
     * \param valueOne The first value in the comparison.
     * \param valueTwo The second value in the comparison.
     * \param message A user message associated with the assertion.
     * \param condOne The string description of the first value.
     * \param condTwo The string description of the second value.
     * \param file The file the assertion was called from.
     * \param line The line the assertion was called from.
     * \return Returns Returns 0 if no error occurs, 1 if there is a soft failure, and 2
     *         if there is a hard failure. */
    template <typename T1, typename T2>
    int checkAssertion(
        const std::string &cmpString,
        const T1 &valueOne,
        const T2 &valueTwo,
        const std::string &message,
        const std::string &condOne,
        const std::string &condTwo,
        const std::string &file,
        int line
    );

    /*! The comparison functions need to be able to cast to a float in case a float
     *  comparison is needed. However, because of the way the comparison functions are
     *  laid out, every type being passed to a comparison must be castable to a float.
     *  This function performs a static cast on all numeric inputs and a pointer level
     *  cast on everything else. This will alsways result in a floating point value, but
     *  in the case of a pointer level cast the result will be COMPLETELY BOGUS. This
     *  should not be a real issue because the cast will only ever be performed on actual
     *  numeric values meaning the result should always be legitimate. The only real point
     *  behind the pointer level cast is to allow the program to compile.
     * \param val The value to cast.
     * \return The floating point value. */
    template <typename T> inline
    Real garbage_cast(const T &val) {
        return gargabe_caster<T, is_numeric<T>::value>::cast(val);
    }

    /*! Performs a soft == comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool feq(const float &a, const float &b);

    /*! Performs a soft != comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool fne(const float &a, const float &b);

    /*! Performs a soft <= comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool fle(const float &a, const float &b);

    /*! Performs a soft >= comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool fge(const float &a, const float &b);

    /*! Performs a soft < comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool flt(const float &a, const float &b);

    /*! Performs a soft > comparison to compensate for floating point error.
     * \param a The first value to compare.
     * \param b The second value to compare.
     * \return The result of the comparison. */
    bool fgt(const float &a, const float &b);

protected:
    // These are protected to match the singleton pattern.
    AssertionHandler();
    virtual ~AssertionHandler();

    // The singleton must be able to call the protected contructors (only children can
    // make calls to protected functions, parents cannot).
    template <class T> friend class Singleton;
};

template <typename T1, typename T2> inline
int AssertionHandler::checkAssertion(
    const std::string &cmpString,
    const T1 &valueOne,
    const T2 &valueTwo,
    const std::string &message,
    const std::string &condOne,
    const std::string &condTwo,
    const std::string &file,
    int line) {

    _assertionsMade++;
    if (!cmpString.length()) { return 0; }
    _assertionsFailed++;

    bool abortOnError = LogStream::SetBreakOnError(false);
    Error("");
    Error("");
    Error("Assertion failed - " << file << ":" << line);
    if (message.length()) {
        Error(message);
    }

    Error("Failed:    " << condOne << " " << cmpString << " " << condTwo);
    Error("Value one: " << valueOne);
    Error("Value two: " << valueTwo);

    LogStream::SetBreakOnError(abortOnError);
    
    return _softAssert ? 1 : 2;
}

extern void SoftAssert();

#undef MAKE_ASSERT_COMPARISON
#endif
