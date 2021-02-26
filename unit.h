#ifndef _UNITTEST_H
#define _UNITTEST_H

#include <stdio.h>
#include <string.h>

#define TEST_LABEL_WIDTH 20
#define ASSERT_FAIL_RETURN 1

#define COLOR_OK "\033[32;1m"
#define COLOR_ERR "\033[31;1m"
#define COLOR_TEST "\033[34;1m"
#define COLOR_NONE "\033[0m"

#define OK_LEN 2
#define OK " "COLOR_OK"\u2713"COLOR_NONE

#define ERR " "COLOR_ERR"X\n"

// Wraps a test block in pretty printf-s
#define test(name, block) \
    printf("=> "COLOR_TEST name COLOR_NONE); \
    for (size_t i = sizeof(name); \
         i < TEST_LABEL_WIDTH || i % OK_LEN; \
         i++) putchar(' '); \
    printf(" |"); \
    do block while (0); \
    printf("\n");

// Prints an error and returns if the condition is false
#define assert(cond) \
    if (cond) printf(OK); \
    else { \
        printf(ERR"Assertion failed: "COLOR_NONE"%s (%s:%d)\n", \
               #cond, __FILE__, __LINE__); \
        return ASSERT_FAIL_RETURN; \
    }

// Prints an error and returns if the two values are not equal
#define assert_eq(a, b, fmt) \
    if (a == b) printf(OK); \
    else { \
        printf(ERR"Equality assertion failed (%s:%d):\n" \
               "Left:  "fmt"\n" \
               "Right: "fmt"\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }

// Prints an error and returns if the two C-strings are not equal
#define assert_streq(a, b) \
    if (!strcmp(a, b)) printf(OK); \
    else { \
        printf(ERR"Equality assertion failed (%s:%d):\n" \
               "Left:  %s\n" \
               "Right: %s\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }

// Asserts equality for a custom type
//  a, b    - values to compare
//  cmp     - comparison function
//  fmt     - printf format
//  fmtargs - printf format args macro
#define assert_custom_eq(a, b, cmp, fmt, fmtargs) \
    if (cmp(a, b)) printf(OK); \
    else { \
        printf(ERR"Equality assertion failed (%s:%d):\n" \
               "Left:  "fmt"\n" \
               "Right: "fmt"\n", __FILE__, __LINE__, \
               fmtargs(a), fmtargs(b)); \
        return ASSERT_FAIL_RETURN; \
    }

#endif // _UNITTEST_H
