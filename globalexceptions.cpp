#include "globalexceptions.hpp"

GeneralFatalError::GeneralFatalError(const char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
GeneralFatalError::~GeneralFatalError(){
    delete errorMessage;
}

shExecuteError::shExecuteError(const char* pErrorMessage):
    errorMessage(pErrorMessage)
{}
shExecuteError::~shExecuteError(){
    delete errorMessage;
}

processManagementError::processManagementError(char *pErrorMessage):
    errorMessage(pErrorMessage)
{}
processManagementError::~processManagementError(){
    delete errorMessage;
}

