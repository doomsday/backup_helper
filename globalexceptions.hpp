#ifndef GLOBALEXCEPTIONS_HPP
#define GLOBALEXCEPTIONS_HPP

/* INFO:
 * Objects that are thrown must have a publicly accessible copy-constructor.
 * The compiler is allowed to generate code that copies the thrown object any
 * number of times, including zero. However even if the compiler never actually
 * copies the thrown object, it must make sure the exception class's copy
 * constructor exists and is accessible.
 */

class FatalError {
    public:
        const char* errorMessage;
        /* WARNING:
         * "Rule of three". You see it below.
         */
        FatalError(const char* pErrorMessage);
        FatalError(const FatalError& rhs);
        FatalError& operator=(const FatalError& rhs);
        ~FatalError();
};

class ShellExecuteError {
    public:
        const char* errorMessage;
        ShellExecuteError(const char* pErrorMessage);
};

class IOError {
    public:
        const char* errorMessage;
        IOError(const char *pErrorMessage);
};

class ProcessManagementError {
    public:
        const char* errorMessage;
        ProcessManagementError(const char *pErrorMessage);
};

#endif // GLOBALEXCEPTIONS_HPP
