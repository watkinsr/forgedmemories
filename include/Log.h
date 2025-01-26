#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdio>
#include <cstdarg>

using namespace std;
#define CURRENT_DATETIME() \
    ([]() { \
        auto now = chrono::system_clock::now(); \
        auto time = chrono::system_clock::to_time_t(now); \
        tm tm; \
        localtime_r(&time, &tm); \
        stringstream ss; \
        ss << put_time(&tm, "%Y-%m-%d %H:%M:%S"); \
        return ss.str(); \
    }())
#ifdef __GNUC__
#define LOG_INFO_FORMAT __attribute__ ((format(printf, 2, 3)))
#define LOG_FORMAT __attribute__ ((format(printf, 3, 4)))
#else
#define LOG_INFO3_FORMAT
#endif
int LOG(int prio, const char *tag, const char *fmt, ...) LOG_FORMAT;
int LOG_nodt(int prio, const char *tag, const char *fmt, ...) LOG_FORMAT;
#define LOG_INFO(fmt, ...) LOG(1, "INFO", fmt, ##__VA_ARGS__)
#endif
