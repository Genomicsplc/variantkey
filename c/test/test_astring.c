// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "../src/astring.c"

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_aztoupper()
{
    int errors = 0;
    int i, c;
    for (i=97 ; i <= 122; i++)
    {
        c = aztoupper(i);
        if (c != (i - 32))
        {
            fprintf(stderr, "%s : Wrong uppercase value for %d - expecting %d, got %d\n", __func__, i, (i - 32), c);
            ++errors;
        }
    }
    c = aztoupper(96);
    if (c != 96)
    {
        fprintf(stderr, "%s : Wrong uppercase value - expecting 96, got %d\n", __func__, c);
        ++errors;
    }
    return errors;
}

int test_prepend_char()
{
    int errors = 0;
    char original[5] =   "BCD";
    char expected[5] = "ABCD";
    size_t size = 3;
    prepend_char('A', original, &size);
    if (size != 4)
    {
        fprintf(stderr, "%s : Expected size 4, got %lu\n", __func__, size);
        ++errors;
    }
    if (strcmp(original, expected) != 0)
    {
        fprintf(stderr, "%s : Expected %s, got %s\n", __func__, expected, original);
        ++errors;
    }
    return errors;
}


int main()
{
    int errors = 0;

    errors += test_aztoupper();
    errors += test_prepend_char();

    return errors;
}
