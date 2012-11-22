#ifndef GLOBALEXCEPTIONS_HPP
#define GLOBALEXCEPTIONS_HPP

class GlobalExceptions
{
};

class GeneralFatalError : public GlobalExceptions {
    public:
        const char* errorMessage;

        GeneralFatalError(const char* pErrorMessage);
        ~GeneralFatalError();
};

class shExecuteError : public GlobalExceptions {
    public:
        const char* errorMessage;

        shExecuteError(const char* pErrorMessage);
        ~shExecuteError();
};

class processManagementError : public GlobalExceptions {
    public:
        char* errorMessage;

        processManagementError(char* pErrorMessage);
        ~processManagementError();
};

#endif // GLOBALEXCEPTIONS_HPP
