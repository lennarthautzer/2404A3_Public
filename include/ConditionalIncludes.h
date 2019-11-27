/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

#ifndef ConditionalIncludeHeader
  #define ConditionalIncludeHeader

  #if defined WIN32 || defined _WIN32 || defined __WIN32 && !defined __CYGWIN__

    #ifndef WINDOWS

      #define WINDOWS

    #endif

  #endif

  #ifdef WINDOWS

    #include <direct.h>
    #define getCurrentDir _getcwd

  #else

    #include <unistd.h>
    #define getCurrentDir getcwd

  #endif

#endif
