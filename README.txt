REMEMBER:

1. Do not create an object to throw it like this:

        IOError e("Can't open requested configuration file\n");
        throw e;

When we throw it:

> Created "e" with pointer to character array "errorMessage"
> Created an unnamed object as the copy of "e" of IOError type, using copy-constructor
> Called destructor of "e"
> DESTRUCTED "e" is thrown (I suppose)
