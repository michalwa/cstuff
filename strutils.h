#ifndef _STRUTILS_H
#define _STRUTILS_H

#include <stddef.h>
#include <stdbool.h>

// Attributes of a string slice
typedef enum {
    // Various functions will print warnings to stderr if a string
    // is not marked with this flag
    STR_VALID = 0x1,
    // Whether a string is allocated on the heap and can be free()-d
    STR_HEAP  = 0x2,
} StringFlags;

// String slice (pointer to data + length + metadata)
typedef struct {
    StringFlags flags;
    size_t bufsz; // buffer byte length (only for STR_HEAP)
    size_t len;   // string byte length
    char  *str;
} String;

/* * * * * * * CREATION * * * * * * */

// Creates a string by directly referencing the given string with the
// specified byte length. Use only when proven safe and sound!
String str_nref(const char *str, size_t len);

// Creates a string by directly referencing the given nul-terminated string.
// Use only when proven safe and sound!
String str_ref(const char *str);

// Allocates a string with the bytes of the given string and the given byte length.
// Requires str_free()
String str_nalloc(const char *str, size_t len);

// Allocates a string with the bytes and length of the given nul-terminated string.
// Requires str_free()
String str_alloc(const char *str);

// Allocates a copy of the given string on the heap
// Requires str_free()
String str_clone(String str);

/* * * * * * * PRINTING * * * * * * */

#define BYTE_BIN_FMT "%c%c%c%c%c%c%c%c"
#define BYTE_BIN_FMT_ARGS(byte) \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

#define STR_FMT "%.*s"
#define STR_FMT_ARGS(s) (int)s.len, s.str

#define STR_DEBUG_FMT \
    "String {\n" \
    "  flags = "BYTE_BIN_FMT",\n" \
    "  bufsz = %zu,\n" \
    "  len   = %zu,\n" \
    "  str   = \"%.*s\"\n" \
    "}"
#define STR_DEBUG_FMT_ARGS(s) \
    BYTE_BIN_FMT_ARGS(s.flags), \
    s.bufsz, s.len, (int)s.len, s.str

// Creates a dynamically allocated string using sprintf
String str_fmt(size_t bufsz, const char *fmt, ...);

/* * * * * * * CONSUMPTION * * * * * * */

// Frees the buffer allocated for the given string
// Calling this with a non-heap-allocated string will do nothing.
void str_free(String str);

// Converts the string into a nul-terminated string.
// The string buffer gets allocated on the heap
// (unless string is luckily already nul-terminated)
char *cstr(const String str);

/* * * * * * * TRANSFORMATION * * * * * * */

// Creates a string by taking a slice of the given string (by reference).
// Use only if proven safe and sound!
String str_slice_ref(String str, size_t offset, size_t len);

// Allocates a string by taking a slice of the given string
String str_slice(String str, size_t offset, size_t len);

/* * * * * * * MUTATION * * * * * * */

// Appends the character to the given string
void str_push(char c, String *str);

// Appends the suffix to the given string
void str_pushs(String suffix, String *str);

// Pops a character off the end of the given string into `out`
// Returns false if the string is empty and a character cannot be popped
bool str_pop(String *str, char *out);

// Pops a string of length `n` off the end of the given string into `out`
// Returns false if the string is shorter than `n` and such a string cannot be popped
// The popped string is allocated in a new buffer and requires str_free()
bool str_popn(String *str, size_t n, String *out);

/* * * * * * * INSPECTION * * * * * * */

// Returns true if the two strings are equal and false otherwise
bool str_eq(String a, String b);

// Counts the number of occurences of the given byte in the string
int str_count(char c, const String str);

// Counts the number of occurencess of the given needle string in
// the haystack string
int str_counts(const String needle, const String haystack);

// Tells whether the given string starts with the given prefix
bool str_startswith(String prefix, String str);

// Tells whether the given string ends with the given suffix
bool str_endswith(String suffix, String str);

#endif // _STRUTILS_H
