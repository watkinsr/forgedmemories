#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>

#define ASSERT_NOT_REACHED()                                             \
  do {                                                                   \
    /* Print location */                                                 \
    fprintf(stderr,                                                      \
            "ERROR: Unreachable code hit at %s:%d in %s()\n",            \
            __FILE__, __LINE__, __func__);                              \
    /* Capture up to 64 frames */                                        \
    void *bt[64];                                                        \
    int bt_size = backtrace(bt, 64);                                     \
    /* Print symbols to stderr */                                        \
    backtrace_symbols_fd(bt, bt_size, STDERR_FILENO);                    \
    abort();                                                             \
  } while (0)
