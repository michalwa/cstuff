#ifndef _UNITTEST_H
#define _UNITTEST_H

#define TEST_LABEL_WIDTH 20
#define ASSERT_FAIL_RETURN 1

#define COLOR_OK "\033[32;1m"
#define COLOR_ERR "\033[31;1m"
#define COLOR_NONE "\033[0m"

// Wraps a test block in pretty printf-s
#define test(name, block) \
    printf("=> "name); \
    for (size_t i = sizeof(name); i < TEST_LABEL_WIDTH || i % 5; i++) \
        printf(" "); \
    do block while (0); \
    printf("\n");

// Prints an error and returns if the condition is false
#define assert(cond) \
    if (cond) printf(" | "COLOR_OK"OK"COLOR_NONE); \
    else { \
        printf("\n"COLOR_ERR"Assertion failed: "COLOR_NONE"%s (%s:%d)\n", \
               #cond, __FILE__, __LINE__); \
        return ASSERT_FAIL_RETURN; \
    }

// Prints an error and returns if the two values are not equal
#define assert_eq(a, b, fmt) \
    if (a == b) printf(" | "COLOR_OK"OK"COLOR_NONE); \
    else { \
        printf("\n"COLOR_ERR"Equality assertion failed (%s:%d):\n" \
               "Left:  "fmt"\n" \
               "Right: "fmt"\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }

// Prints an error and returns if the two values are equal
#define assert_neq(a, b, fmt) \
    if (a != b) printf(" | "COLOR_OK"OK"COLOR_NONE); \
    else { \
        printf("\n"COLOR_ERR"Inequality assertion failed (%s:%d):\n" \
               "Left:  "fmt"\n" \
               "Right: "fmt"\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }

// Prints an error and returns if the two C-strings are not equal
#define assert_streq(a, b) \
    if (!strcmp(a, b)) printf(" | "COLOR_OK"OK"COLOR_NONE); \
    else { \
        printf("\n"COLOR_ERR"Equality assertion failed (%s:%d):\n" \
               "Left:  %s\n" \
               "Right: %s\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }


// Prints an error and returns if the two C-strings are equal
#define assert_strneq(a, b) \
    if (strcmp(a, b)) printf(" | "COLOR_OK"OK"COLOR_NONE); \
    else { \
        printf("\n"COLOR_ERR"Inequality assertion failed (%s:%d):\n" \
               "Left:  %s\n" \
               "Right: %s\n", __FILE__, __LINE__, a, b); \
        return ASSERT_FAIL_RETURN; \
    }

#endif // _UNITTEST_H
