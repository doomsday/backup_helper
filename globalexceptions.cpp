#include "globalexceptions.hpp"

FatalError::FatalError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
FatalError::~FatalError(){
    delete errorMessage;
}

shExecuteError::shExecuteError(const char* pErrorMessage):
    errorMessage(pErrorMessage)
{}
shExecuteError::~shExecuteError(){
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

