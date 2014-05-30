#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1

/* -- Macro Definitions */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef DEBUG
#define debug_print(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                               __LINE__, __func__, __VA_ARGS__); } while (0)
#else
#define debug_print(fmt, ...) do {} while (0)
#endif

#endif
