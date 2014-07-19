//============================================================================
// Copyright   : Your copyright notice copy from http://kb.cnblogs.com/page/79171/
// Thanks to http://program-exercise-project.googlecode.com/svn-history/r14/trunk/Exercise/C++/exception_backtrace.cpp
// Description : exception. Modified from the above url. Remove stack stuff.
//============================================================================

//#include <execinfo.h>
#include <stdlib.h>
//#include <cxxabi.h>

#include <iostream>
#include <sstream>

#include "exceptions.h"

namespace bible {
using namespace std;

ExceptionBase::ExceptionBase(const std::string &msg) throw()
    : mMsg(msg),
      mFile("<unknown file>"),
      mFunc("<unknown func>"),
      mLine(-1) {}

ExceptionBase::~ExceptionBase() throw() {}

void ExceptionBase::Init(const char *file, const char *func, int line) {
    mFile = file;
    mFunc = func;
    mLine = line;
    //mStackTraceSize = backtrace(mStackTrace, MAX_STACK_TRACE_SIZE);
}

std::string ExceptionBase::GetClassName() const {
    return "ExceptionBase";
}

const char *ExceptionBase::what() const throw() {
    return ToString().c_str();
}

const std::string &ExceptionBase::ToString() const {
    if (mWhat.empty()) {
        stringstream sstr("");
        if (mLine > 0) {
            sstr << mFile << "(" << mLine << ")";
        }
        sstr <<  ": " << GetClassName();
        if (!GetMessage().empty()) {
            sstr << ": " << GetMessage();
        }
        //sstr << "\nStack Trace:\n";
        //sstr << GetStackTrace();
        mWhat = sstr.str();
    }
    return mWhat;
}

std::string ExceptionBase::GetMessage() const {
    return mMsg;
}

/* example:

   void testThrow(int &a, int &b) {
   try{
   if(a > b) {
   BIBLE_THROW(ExceptionBible, "a is big than b ,so throw");        //抛异常
   }
   else {
   cout<<"a not big than b"<<endl;
   }
   }
   catch (ExceptionBible& e) {
   cout << e.what() << endl;
   }
   }*/

} // end namespace bible.
