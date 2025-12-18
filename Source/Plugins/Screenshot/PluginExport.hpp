#pragma once

#ifdef _WIN32
    #ifdef SCREENSHOT_EXPORTS
        #define SCREENSHOT_API __declspec(dllexport)
    #else
        #define SCREENSHOT_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define SCREENSHOT_API __attribute__((visibility("default")))
    #else
        #define SCREENSHOT_API
    #endif
#endif