#include "globalexceptions.hpp"

FatalError::FatalError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
FatalError::~FatalError(){
    delete errorMessage;
}

ShellExecuteError::ShellExecuteError(const char* pErrorMessage):
    errorMessage(pErrorMessage)
{}
ShellExecuteError::~ShellExecuteError(){
    delete errorMessage;
}

IOError::IOError(char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
IOError::~IOError(){
    delete errorMessage;
}

processManagementError::processManagementError(char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
processManagementError::~processManagementError(){
    delete errorMessage;
}

