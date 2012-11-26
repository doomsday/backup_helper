REMEMBER:

1. Do not create an object to throw it like this:

        IOError e("Can't open requested configuration file\n");
        throw e;

2. Throwing the caught exception object using throw e; will cause
a copy of e to be made. This is not the same as rethrowing the exception

3. Technically, even when you catch an exception by reference, the compiler
still uses pass by value. This is due to the fact that a catch never
returns control to the caller, and is thus responsible for clean-up.


WHAT'S INTERESTING:

1. Configuration parsers based on boost/spirit
2. Using of system() call which allows to run shell command in Linux
3. Using of basic Exceptions concept

I USED:

1. http://habrahabr.ru/post/50973/
2. http://habrahabr.ru/post/50976/
3. http://stackoverflow.com/a/6898456/947501
4. man

Thanks all authors and thousands of respects to you!
