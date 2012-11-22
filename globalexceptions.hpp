#ifndef GLOBALEXCEPTIONS_HPP
#define GLOBALEXCEPTIONS_HPP

class GlobalExceptions
{
};

class GeneralFatalError : public GlobalExceptions {
public:
    char* errorMessage;
};

class shExecuteError : public GlobalExceptions
{
    public:
        const char* errorMessage;
        shExecuteError(const char* pErrorMessage);
};
class IOError : public GlobalExceptions {};
class processManagementError : public GlobalExceptions {};
#endif // GLOBALEXCEPTIONS_HPP
