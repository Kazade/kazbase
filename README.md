# KazBase

KazBase is my C++ base library where I put all my common code. The library inherits a lot from the Python standard library, but with more consistent naming.

KazBase includes code for the following:
    
    - Logging system (to file, or stdout)
    - Unicode support (see unicode.h, based on Python's unicode type)
    - File utilities (e.g. reading an entire file into a buffer)
    - An Exception class heirarchy (base on Python + Django exceptions (e.g. DoesNotExist))
    - Filesystem operations (based on Python's 'os' module)
    - Threading utilities
    - XDG Base Directory spec utility functions (see fdo namespace)
    - JSON parsing/saving
    - Globbing functions
    - Loads of random stuff that might be useful

All of my code in this repository is BSD licensed. Knock yourself out! But, bear in mind that the whole API is in permenant, constant flux!


