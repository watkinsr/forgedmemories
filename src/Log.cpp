#include "Log.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>  // for strcmp
#include <ctime>

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
int LOG_nodt(int prio, const char *tag, const char *fmt, ...) {
    // Oh boy, WASM doesn't like this !
    #ifdef __EMSCRIPTEN__
    // No-op
    #else
    va_list args;
    va_start(args, fmt);
    std::printf("[%s] ", tag);
    std::vprintf(fmt, args);
    va_end(args);
    #endif
    return 0;
}

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
 int LOG(int prio, const char *tag, const char *fmt, ...) {
     // printf("LOG(%i, %s, fmt=?)\n", prio, tag);

     if (std::strcmp(tag, "PERF") == 0) {
#ifndef PERF
         return 0;
#endif
     }

     if (std::strcmp(tag, "VERBOSE") == 0) {
#ifndef VERBOSE
         return 0;
#endif
    }

#ifdef __EMSCRIPTEN__
    // Oh boy, WASM doesn't like this => No-op
#else

    
    // Get current date and time
    std::time_t now = std::time(0);
    char datetime[20];
    std::strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    // Start the variable argument processing
    va_list args;
    va_start(args, fmt);
    // Print the log with the provided format
    std::printf("[%s] [%s] ", datetime, tag);
    std::vprintf(fmt, args);
    // End variable argument processing
    va_end(args);
    #endif
    return 0; // Or any appropriate return value
}
