#include "globalexceptions.hpp"
#include <cstring>
#include <iostream>

FatalError::FatalError(const char *pErrorMessage){
    /* INFO:
     * http://stackoverflow.com/questions/13552529/sigabrt-in-destructor-of-an-exception-class
     */
    char* buf = new char[strlen(pErrorMessage)+1];
    strcpy(buf, pErrorMessage);
    errorMessage = buf;
}

FatalError::FatalError(const FatalError& rhs){
    char* buf = new char[strlen(rhs.errorMessage)+1];
    strcpy(buf, rhs.errorMessage);
    errorMessage = buf;
}

FatalError& FatalError::operator =(const FatalError& rhs){
    if (this == &rhs)
        return *this;
    delete[] errorMessage;
    char* buf = new char[strlen(rhs.errorMessage)+1];
    strcpy(buf,rhs.errorMessage);
    errorMessage = buf;
    return *this;
}

FatalError::~FatalError(){
    delete[] errorMessage;
}

ShellExecuteError::ShellExecuteError(const char* pErrorMessage):
    errorMessage(pErrorMessage)
{}

IOError::IOError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}

ProcessManagementError::ProcessManagementError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
