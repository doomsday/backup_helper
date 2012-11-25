#include "globalexceptions.hpp"
#include <cstring>
#include <iostream>

FatalError::FatalError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}

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
