#pragma once

#ifdef _WIN32
    #ifdef SAMPLE_PLUGIN_EXPORTS
        #define SAMPLE_PLUGIN_API __declspec(dllexport)
    #else
        #define SAMPLE_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define SAMPLE_PLUGIN_API __attribute__((visibility("default")))
    #else
        #define SAMPLE_PLUGIN_API
    #endif
#endif