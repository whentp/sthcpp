//============================================================================
// Name        : exception_backtrace.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice copy from http://kb.cnblogs.com/page/79171/
// Description : exception plus backtrace
// compile cmd: g++ -rdynamic -g -O2 -Wall -I . exception_backtrace.cpp -o exception_backtrace
// check address cmd:addr2line 0x8049c79 -e ./exception_backtrace -f
//============================================================================

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

#define BIBLE_THROW(ExClass, args...)                          \
    do {                                                       \
        ExClass e(args);                                       \
        e.Init(__FILE__, __PRETTY_FUNCTION__, __LINE__);       \
        throw e;                                               \
    }                                                          \
    while (false)

#define BIBLE_DEFINE_EXCEPTION(ExClass, Base)                  \
    ExClass(const std::string& msg = "") throw()               \
        : Base(msg)                                            \
    {}                                                     	   \
    ~ExClass() throw() {}                                      \
    std::string GetClassName() const {return #ExClass;}

namespace bible {

class ExceptionBase: public std::exception {
public:
    ExceptionBase(const std::string &msg = "") throw();

    virtual ~ExceptionBase() throw();

    void Init(const char *file, const char *func, int line);

    virtual std::string GetClassName() const;

    virtual std::string GetMessage() const;

    const char *what() const throw();

    const std::string &ToString() const;

protected:
    std::string mMsg;
    const char *mFile;
    const char *mFunc;
    int mLine;

private:
    //enum { MAX_STACK_TRACE_SIZE = 50 };
    //void* mStackTrace[MAX_STACK_TRACE_SIZE];
    //size_t mStackTraceSize;
    mutable std::string mWhat;
};

class ExceptionBible: public ExceptionBase {
public:
    BIBLE_DEFINE_EXCEPTION(ExceptionBible, ExceptionBase);
};

} // end namespace bible.

#endif
