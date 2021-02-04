#ifndef _STRUTILS_H
#define _STRUTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

// Minimum buffer size
#define STR_MIN_BUFSZ 0x80

typedef enum {
    // Various functions will print warnings to stderr if a string
    // is not marked with this flag
    STR_VALID = 0x1,
    // Whether a string is allocated on the heap and can be free()-d
    STR_HEAP  = 0x2,
} StringFlags;

// String slice (pointer to data + length)
typedef struct {
    StringFlags flags;
    size_t bufsz; // buffer byte length (only for STR_HEAP)
    size_t len;   // string byte length
    char  *str;
} String;

/* * * * * * * Private Utilities * * * * * * */

#define FLAGS_ALL(actual, expected) ((actual & expected) == expected)
#define STR_CHECK_VALID(str, scope) \
    if (!(str.flags & STR_VALID)) \
        fprintf(stderr, "Invalid "#str" passed to "#scope"\n");

// Computes the optimal buffer size for a string with the given length
size_t str_bufsz(size_t len) {
    if (len + 1 <= STR_MIN_BUFSZ)
        return STR_MIN_BUFSZ;

    // Least power of two greater than len
    size_t bufsz = 1;
    for (; len; len >> 1) { bufsz << 1; }
    return bufsz;
}

// Allocates more memory if needed to store a string of the given desired length
// Doesn't check whether the string is valid or allocated on the heap!
void str_ensure_buf(String *str, size_t len) {
    if (len > str->bufsz) {
        str->bufsz *= 2;
        str->str = realloc(str->str, str->bufsz);
    }
}

/* * * * * * * CREATION * * * * * * */

// Creates a string by directly referencing the given string with the
// specified byte length. Use only when proven safe and sound!
String str_nref(const char *str, size_t len) {
    return (String){
        .flags = STR_VALID,
        .bufsz = 0,
        .len   = len,
        .str   = (char *)str,
    };
}

// Creates a string by directly referencing the given nul-terminated string.
// Use only when proven safe and sound!
String str_ref(const char *str) {
    return str_nref(str, strlen(str));
}

// Allocates a string with the bytes of the given string and the given byte length.
// Requires str_free()
String str_nalloc(const char *str, size_t len) {
    size_t bufsz = str_bufsz(len);
    return (String){
        .flags = STR_VALID | STR_HEAP,
        .bufsz = bufsz,
        .len   = len,
        .str   = memcpy(malloc(bufsz), str, len),
    };
}

// Allocates a string with the bytes and length of the given nul-terminated string.
// Requires str_free()
String str_alloc(const char *str) {
    return str_nalloc(str, strlen(str));
}

// Allocates a copy of the given string on the heap
// Requires str_free()
String str_clone(String str) {
    return str_nalloc(str.str, str.len);
}

/* * * * * * * PRINTING * * * * * * */

#define STR_FMT "%.*s"
#define STR_FMT_ARGS(s) (int)s.len, s.str
#define STR_DEBUG_FMT "{ flags = %x, bufsz = %zu, len = %zu, str = %.*s }"
#define STR_DEBUG_FMT_ARGS(s) (int)s.flags, s.bufsz, s.len, (int)s.len, s.str

// Creates a dynamically allocated string using sprintf
String str_fmt(size_t bufsz, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    String str;
    str.flags = STR_VALID | STR_HEAP;
    str.bufsz = str_bufsz(bufsz);
    str.str   = malloc(str.bufsz);
    str.len   = vsnprintf(str.str, str.bufsz, fmt, args);

    if (str.len == -1)
        fprintf(stderr, "str_fmt buffer capacity exceeded\n");

    va_end(args);

    return str;
}

/* * * * * * * CONSUMPTION * * * * * * */

// Frees the buffer allocated for the given string
// Calling this with a non-heap-allocated string will do nothing.
void str_free(String str) {
    if (!FLAGS_ALL(str.flags, STR_VALID | STR_HEAP)) return;

    memset(str.str, 0, str.len);
    free(str.str);

    str.flags = 0;
    str.bufsz = 0;
    str.len   = 0;
    str.str   = NULL;
}

// Converts the string into a nul-terminated string.
// The string buffer gets allocated on the heap
// (unless string is luckily already nul-terminated)
char *cstr(const String str) {
    STR_CHECK_VALID(str, cstr);

    // This would improve performance by preventing heap allocation, if the string
    // is ready to be returned as-is, but may cause problems if the user code free()-s
    // the returned pointer, so I remove it for convenience.
    //
    // if (str.str[str.len - 1] == '\0')
    //     return str.str;

    return strncpy(malloc(str.len + 1), str.str, str.len);
}

/* * * * * * * MUTATION * * * * * * */

// Appends the character to the given string
void str_append(char c, String *str) {
    if (!FLAGS_ALL(str->flags, STR_VALID | STR_HEAP))
        fprintf(stderr, "Invalid string passed to str_append\n");

    str_ensure_buf(str, str->len + 1);
    str->str[str->len++] = c;
}

// Appends the suffix to the given string
void str_appends(String suffix, String *str) {
    if (!FLAGS_ALL(str->flags, STR_VALID | STR_HEAP))
        fprintf(stderr, "Invalid string passed to str_appends\n");

    str_ensure_buf(str, str->len + suffix.len);
    strncpy(str->str + str->len, suffix.str, suffix.len);
    str->len += suffix.len;
}

/* * * * * * * TRANSFORMATION * * * * * * */

// Creates a string by taking a slice of the given string (by reference).
// Use only if proven safe and sound!
String str_slice_ref(String str, size_t offset, size_t len) {
    STR_CHECK_VALID(str, str_slice_ref);

    if (offset + len > str.len)
        fprintf(stderr, "Slice len out of bounds: "STR_FMT" [%zu, %zu]\n",
                STR_FMT_ARGS(str), offset, len);

    return str_nref(str.str + offset, len);
}

// Allocates a string by taking a slice of the given string
String str_slice(String str, size_t offset, size_t len) {
    STR_CHECK_VALID(str, str_slice);

    if (offset + len > str.len)
        fprintf(stderr, "Slice len out of bounds: "STR_FMT" [%zu, %zu]\n",
                STR_FMT_ARGS(str), offset, len);

    return str_nalloc(str.str + offset, len);
}

/* * * * * * * ANALYSIS * * * * * * */

// Returns true if the two strings are equal and false otherwise
bool str_eq(String a, String b) {
    if (a.len != b.len) return false;
    return !strncmp(a.str, b.str, a.len);
}

// Counts the number of occurences of the given byte in the string
int str_count(char c, const String str) {
    STR_CHECK_VALID(str, str_count);

    int n = 0;
    for (size_t i = 0; i < str.len; str.str[i++] == c ? n++ : 0);
    return n;
}

// Counts the number of occurencess of the given needle string in
// the haystack string
int str_counts(const String needle, const String haystack) {
    STR_CHECK_VALID(needle,   str_counts);
    STR_CHECK_VALID(haystack, str_counts);

    if (needle.len == 0 || haystack.len == 0)
        return 0;

    int n = 0;

    for (size_t i = 0; i <= haystack.len - needle.len; i++) {
        bool eq = true;

        for (size_t j = 0; j < needle.len; j++) {
            if (haystack.str[i + j] != needle.str[j]) {
                eq = false;
                break;
            }
        }

        if (eq) n++;
    }

    return n;
}

// Tells whether the given string starts with the given prefix
bool str_startswith(String prefix, String str) {
    STR_CHECK_VALID(prefix, str_startswith);
    STR_CHECK_VALID(str,    str_startswith);

    if (prefix.len > str.len) return false;

    return !strncmp(str.str, prefix.str, prefix.len);
}

// Tells whether the given string ends with the given suffix
bool str_endswith(String suffix, String str) {
    STR_CHECK_VALID(suffix, str_startswith);
    STR_CHECK_VALID(str,    str_startswith);

    if (suffix.len > str.len) return false;

    return !strncmp(str.str + str.len - suffix.len,
                    suffix.str, suffix.len);
}

#undef FLAGS_ALL
#undef STR_CHECK_VALID
#endif // _STRUTILS_H
