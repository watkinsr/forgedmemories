#include "Log.h"
#include <cstdio>
#include <ctime>
#include <cstdarg>

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
    // Oh boy, WASM doesn't like this !
    #ifdef __EMSCRIPTEN__
    // No-op
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
