#include "strutils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* * * * * * * Private Utilities * * * * * * */

#define FLAGS_ALL(actual, expected) ((actual & expected) == expected)
#define STR_CHECK_VALID(str, scope) \
    if (!(str.flags & STR_VALID)) \
        fprintf(stderr, "Invalid "#str" passed to "#scope"\n");

// Minimum char buffer size
#define STR_MIN_BUFSZ 0x80

// Computes the optimal buffer size for a string with the given length
size_t str_bufsz(size_t len) {
    if (len + 1 <= STR_MIN_BUFSZ)
        return STR_MIN_BUFSZ;

    // Least power of two greater than len
    size_t bufsz = 1;
    for (; len; len >> 1, bufsz << 1);
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

String str_nref(const char *str, size_t len) {
    return (String){
        .flags = STR_VALID,
        .bufsz = 0,
        .len   = len,
        .str   = (char *)str,
    };
}

String str_ref(const char *str) {
    return str_nref(str, strlen(str));
}

String str_nalloc(const char *str, size_t len) {
    size_t bufsz = str_bufsz(len);
    return (String){
        .flags = STR_VALID | STR_HEAP,
        .bufsz = bufsz,
        .len   = len,
        .str   = memcpy(malloc(bufsz), str, len),
    };
}

String str_alloc(const char *str) {
    return str_nalloc(str, strlen(str));
}

String str_clone(String str) {
    return str_nalloc(str.str, str.len);
}

/* * * * * * * PRINTING * * * * * * */

String str_fmt(size_t bufsz, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    String str;
    str.flags = STR_VALID | STR_HEAP;
    str.bufsz = str_bufsz(bufsz + 1); // space for null byte
    str.str   = malloc(str.bufsz);
    str.len   = vsnprintf(str.str, str.bufsz, fmt, args);

    if (str.len < 0)
        fprintf(stderr, "str_fmt buffer capacity exceeded\n");

    va_end(args);

    return str;
}

/* * * * * * * CONSUMPTION * * * * * * */

void str_free(String str) {
    if (!FLAGS_ALL(str.flags, STR_VALID | STR_HEAP)) return;

    memset(str.str, 0, str.len);
    free(str.str);
}

char *cstr(String str) {
    STR_CHECK_VALID(str, cstr);

    // This would improve performance by preventing heap allocation, if the string
    // is ready to be returned as-is, but may cause problems if the user code free()-s
    // the returned pointer, so I remove it for convenience.
    //
    // if (str.str[str.len - 1] == '\0')
    //     return str.str;

    return strncpy(malloc(str.len + 1), str.str, str.len);
}

/* * * * * * * TRANSFORMATION * * * * * * */

String str_slice_ref(String str, size_t offset, size_t len) {
    STR_CHECK_VALID(str, str_slice_ref);

    if (offset + len > str.len)
        fprintf(stderr, "Slice len out of bounds: "STR_FMT" [%zu, %zu]\n",
                STR_FMT_ARGS(str), offset, len);

    return str_nref(str.str + offset, len);
}

String str_slice(String str, size_t offset, size_t len) {
    STR_CHECK_VALID(str, str_slice);

    if (offset + len > str.len)
        fprintf(stderr, "Slice len out of bounds: "STR_FMT" [%zu, %zu]\n",
                STR_FMT_ARGS(str), offset, len);

    return str_nalloc(str.str + offset, len);
}

/* * * * * * * MUTATION * * * * * * */

void str_push(char c, String *str) {
    if (!FLAGS_ALL(str->flags, STR_VALID | STR_HEAP))
        fprintf(stderr, "Invalid string passed to str_append\n");

    str_ensure_buf(str, str->len + 1);
    str->str[str->len++] = c;
}

void str_pushs(String suffix, String *str) {
    if (!FLAGS_ALL(str->flags, STR_VALID | STR_HEAP))
        fprintf(stderr, "Invalid string passed to str_appends\n");

    str_ensure_buf(str, str->len + suffix.len);
    strncpy(str->str + str->len, suffix.str, suffix.len);
    str->len += suffix.len;
}

bool str_pop(String *str, char *out) {
    if (str->len == 0) return false;
    if (out) *out = str->str[--str->len];
    return true;
}

bool str_popn(String *str, size_t n, String *out) {
    if (str->len < n) return false;
    if (out) *out = str_slice(*str, str->len - n, n);
    str->len -= n;
    return true;
}

/* * * * * * * INSPECTION * * * * * * */

bool str_eq(String a, String b) {
    if (a.len != b.len) return false;
    return !strncmp(a.str, b.str, a.len);
}

int str_count(char c, String str) {
    STR_CHECK_VALID(str, str_count);

    int n = 0;
    for (size_t i = 0; i < str.len; str.str[i++] == c ? n++ : 0);
    return n;
}

int str_counts(String needle, String haystack, StrCountFlags flags) {
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

        if (eq) {
            if (!(flags & STR_COUNT_OVERLAP))
                i += needle.len - 1;

            n++;
        }
    }

    return n;
}

bool str_startswith(String prefix, String str) {
    STR_CHECK_VALID(prefix, str_startswith);
    STR_CHECK_VALID(str,    str_startswith);

    if (prefix.len > str.len) return false;

    return !strncmp(str.str, prefix.str, prefix.len);
}

bool str_endswith(String suffix, String str) {
    STR_CHECK_VALID(suffix, str_startswith);
    STR_CHECK_VALID(str,    str_startswith);

    if (suffix.len > str.len) return false;

    return !strncmp(str.str + str.len - suffix.len,
                    suffix.str, suffix.len);
}
