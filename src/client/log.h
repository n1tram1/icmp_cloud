#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG_ON 1

#if LOG_ON
#define LOG(...) fprintf(stderr, __VA_ARGS__);
#else
#define LOG(...)
#endif

#endif /* !LOG_H */
