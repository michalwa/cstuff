#ifndef _UTF8_H
#define _UTF8_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Holds a state used by utf8_decode
typedef struct {
    uint8_t  state;
    uint32_t codepoint;
} utf8_Decoder;

// Decodes subsequent bytes of a string into utf8 codepoints
// NOTE: Decoder has to be initialized to zero before decoding a new string!
bool utf8_decode(utf8_Decoder *d, char next);

// Encodes subsequent codepoints into utf8 and appends the resulting
// bytes to the given buffer, returning pointers to memory in the buffer
// after the appended bytes
char *utf8_encode(char *buffer, uint32_t codepoint);

// Returns the number of unicode codepoints in a given string
int utf8_len(char *str, size_t len);

#endif // _UTF8_H
