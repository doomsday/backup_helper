#ifndef GLOBALEXCEPTIONS_HPP
#define GLOBALEXCEPTIONS_HPP

class FatalError {
    public:
        const char* errorMessage;

        FatalError(const char* pErrorMessage);
        ~FatalError();
};

class ShellExecuteError {
    public:
        const char* errorMessage;

        ShellExecuteError(const char* pErrorMessage);
        ~ShellExecuteError();
};

class IOError {
    public:
        char* errorMessage;

        IOError(char* pErrorMessage);
        ~IOError();
};

class ProcessManagementError {
    public:
        char* errorMessage;

        ProcessManagementError(char* pErrorMessage);
        ~ProcessManagementError();
};

#endif // GLOBALEXCEPTIONS_HPP
