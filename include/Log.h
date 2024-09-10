#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdio>

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

#define LOG_INFO(fmt, ...) \
    do { \
        std::printf("[%s] [INFO] " fmt "\n", CURRENT_DATETIME().c_str(), ##__VA_ARGS__); \
    } while (0)

#endif
