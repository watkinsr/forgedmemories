#include "Log.h"
#include <cstdio>
#include <ctime>
#include <cstdarg>

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
int LOG(int prio, const char *tag, const char *fmt, ...) {
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
    return 0; // Or any appropriate return value
}
