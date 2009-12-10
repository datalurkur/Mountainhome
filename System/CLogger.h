#ifndef _CLOGGER_H_
#define _CLOGGER_H_
#error This doesn't actually exist as of now.

/*! This is a class used to aid in the output of information. It acts as sort of a merge
 *  between c output and c++ output. It's mostly based off of the old printf style,
 *  like with ostreams, you don't need to specify data type. It also supports a small set
 *  of string replacement tags for easy inserting of time, class names, line numbers,
 *  etc... Other features include built in indentation, leveled output (trace, debug, 
 *  etc...), and outputing to a file or the console. The debug level may be set at compile
 *  time allowing certain messages to be completely removed from the code resulting in
 *  absolutely zero performance loss (the downside to this is the necessity of macros).
 *
 *  Like the old printf output, this logger will make use of format strings and %
 *  deliniated wild cards. The difference is that instead of just marking variable
 *  insertion, tags will also mark special replacements. Here is a list of supported
 *  values:
 *
 *    %f - File
 *    %t - Time
 *    %c - Class
 *    %l - Line
 *
 *  Variables will be supported using numerics to index into the passed variables. Here is
 *  an example out how to output with this library, as well as equivilent code using the
 *  conventional C and C++ methods:
 *
 *  int a = 1, b = 5;
 *  Debug("[%t] %2 * %1 = %2", a, b);
 *  Debug("[%t]" << b << "*" << a << "=" << b);
 *  printf("[%s]%i * %i = %i\n", getTimeString(), b, a, b);
 *  cout << "[" << getTimeString() << "] " << b << " * " << a << " = " << b << endl;
 *
 *  All three will output the following (assuming it is 6:30PM): [18:30] 5 * 1 = 5
 *
 * \brief A logging class (what more need be said?).
 * \author Brent Wilson
 * \date 8/14/07
 * \todo Enforce a line width?
 * \todo Add array lookups (so you can just pass an array, rather than each element)?
 * \todo Add separators? (ie ---------Init---------) */
class Logger : public Singleton<Logger> {
public :
    /*! LogType enumerates the different log levels available to the system. */
    enum LogType {
        TraceMessage,   /*!< Used for tracing the codepath.                          */
        DebugMessage,   /*!< Used to help find errors in the code.                   */
        InfoMessage,    /*!< Used for generic information that would be helpful.     */
        WarningMessage, /*!< Used when questionable behavior arises.                 */
        ErrorMessage,   /*!< Used to log errors that could have major repercussions. */
        NoOutput        /*!< Used simply to shut off all output.                     */
    };

public:
    /*! Sets the pretext string that is printed as the object is being created. Often this
     *  will make use of the various built in replacements like %t and %c.
     * \param text The text to print out during object construction. */
    void setPretext(const string &text);

    /*! Sets an indent level for output. By setting the indent level, leading whitespace
     *  is added to all output to aid in clarity.
     * \param level The number of indentations to place before output.
     * \sa setIndentSize */
    void setIndentLevel(int level);

    /*! Sets the number of spaces per indentation.
     * \param size The number of spaces in a single indentation. */
    void setIndentSize(int size);

    /*! Sets the a parameter that determines if LogType 'Error' messages will cause the
     *  program to abort once the message has been printed out. This may make debugging
     *  certain errors easier.
     *  \param abort The new value to determine if errors result in an abort. */
    void setAbortOnError(bool abort);
    
    /*! This function is used to determine where program output is directed. Output can be
     *  sent to a log file or standard out.
     *  \param filename Sets the file to log to. If this value is NULL, logging will go to
     *         standard out. */
    void setLogTarget(const string &filename);

    /*! The log level may be set at both compile and run time. When set at compile time,
     *  certain log messages are not even compiled into the code, thus the compile level
     *  acts as a hard cap for the ammount of output whereas this acts as a soft cap.
     *  \param level The minimum level of output to actually log. */
    void setLogLevel(LogType level);

    /*! This will delete the old log file. */
    void clearLogFile();

    /*! Sets a flag the decides if the old printf wildcard notation should be used. This
     *  means all of the special tags (time, file, etc...) will be unusable, as will the
     *  special variable notation (%1, %2, etc...). All log functions will act just like
     *  printf (want more info? man printf).
     * \param useOld Determines if printf notation should be used. */
    void usePrintfFormatting(bool useOld);

    /*! Outputs a message with a log level of TraceMessage
     * \param format The output format to use.
     * \param ... The arguments used by the format */
    void trace(const string &format, ...);

    /*! Outputs a message with a log level of DebugMessage
     * \param format The output format to use.
     * \param ... The arguments used by the format */
    void debug(const string &format, ...);

    /*! Outputs a message with a log level of InfoMessage
     * \param format The output format to use.
     * \param ... The arguments used by the format */
    void info(const string &format, ...);

    /*! Outputs a message with a log level of WarningMessage
     * \param format The output format to use.
     * \param ... The arguments used by the format */
    void warning(const string &format, ...);

    /*! Outputs a message with a log level of ErrorMessage
     * \param format The output format to use.
     * \param ... The arguments used by the format */
    void error(const string &format, ...);

protected:
    /* This is the method that does all of the work, all of the public methods just
     * delegate to this one.
     * \todo Look into adding support for file, line, and class. I believe all of these
     *       will require additional shit being passed to this function (though I could
     *       also do it through macro magic and additional function calls... hmm...).
     *       I could just pass in the additional information using the varargs...
     * \param logLevel The level at which to output the sent info. This will be culled
     *        based on the software cap.
     * \param format The format of the string to print.
     * \param ... The variables to print. */
    void print(int logLevel, const string &format, ...);

protected:
    Logger();
    virtual ~Logger();

    template <class T> friend class Singleton;
};

#endif
