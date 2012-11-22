#ifndef GLOBALEXCEPTIONS_HPP
#define GLOBALEXCEPTIONS_HPP

class GlobalExceptions
{
};

class FatalError : public GlobalExceptions {
    public:
        const char* errorMessage;

        FatalError(const char* pErrorMessage);
        ~FatalError();
};

class shExecuteError : public GlobalExceptions {
    public:
        const char* errorMessage;

        shExecuteError(const char* pErrorMessage);
        ~shExecuteError();
};

class IOError : public GlobalExceptions {
    public:
        char* errorMessage;

        IOError(char* pErrorMessage);
        ~IOError();
};

class processManagementError : public GlobalExceptions {
    public:
        char* errorMessage;

        processManagementError(char* pErrorMessage);
        ~processManagementError();
};

#endif // GLOBALEXCEPTIONS_HPP
