#define __ASSERT_USE_STDERR
#include <assert.h>
//#include "printf.h"
#include <Logging.h>

void __assert (const char *func, const char *file, int line, const char *failedexpr)
{
    while(true) {
        if (func == NULL)
            Log.Error("Assertion failed: %s:%d  %s"CR, file, line, failedexpr);
        else
            Log.Error("Assertion failed: %s:%s:%d  %s"CR, file, func,line, failedexpr);

        delay(4000); // 4s
    }
    /* NOTREACHED */
}