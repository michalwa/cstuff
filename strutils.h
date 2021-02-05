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
char *cstr(String str);

/* * * * * * * TRANSFORMATION * * * * * * */

// Creates a string by taking a slice of the given string (by reference).
// Use only if proven safe and sound!
String str_slice_ref(String str, size_t offset, size_t len);

// Allocates a string by taking a slice of the given string
String str_slice(String str, size_t offset, size_t len);

// Flags for str_strip()
typedef enum {
    // Strip from the left/beginning
    STR_STRIP_LEFT  = 0x1,
    // Strip from the right/end
    STR_STRIP_RIGHT = 0x2,
    // Allocate the new string on the heap
    STR_STRIP_HEAP  = 0x4,
} StrStripFlags;

// Strips characters included in the given C-string from the given string
// Returns the number of stripped characters to `out`
// If both STR_STRIP_LEFT and STR_STRIP_RIGHT is given in the flags,
// returns the total number of characters stripped
String str_strip(const char *chs, String str, StrStripFlags flags, int *out);

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

// Inserts a character into the string at the specified position
// If the position is outside the bounds of the string, the character is appended
// to the end of the string
void str_insert(char c, size_t pos, String *str);

// Inserts a string into another string at the specified position
// If the position is outside the bounds of the string, the infix is appended
// to the end of the string
void str_inserts(String infix, size_t pos, String *str);

// Replaces the specified substring in the given string with a replacement string
void str_replace_slice(size_t offset, size_t len, String repl, String *str);

/* * * * * * * INSPECTION * * * * * * */

// Returns true if the two strings are equal and false otherwise
bool str_eq(String a, String b);

// Counts the number of occurences of the given byte in the string
int str_count(char c, String str);

typedef enum {
    // Count overlapping substring occurences
    STR_COUNT_OVERLAP = 0x1,
} StrCountFlags;

// Counts the number of occurencess of the given needle string in the haystack string
int str_counts(String needle, String haystack, StrCountFlags flags);

// Tells whether the given string starts with the given prefix
bool str_startswith(String prefix, String str);

// Tells whether the given string ends with the given suffix
bool str_endswith(String suffix, String str);

#endif // _STRUTILS_H
