#ifndef SMART_ASSERT_H
#define SMART_ASSERT_H

#define __ASSERT_USE_STDERR
#include <assert.h>
#include <Logging.h>

extern
void __assert (const char *func, const char *file, int line, const char *failedexpr);

#endif