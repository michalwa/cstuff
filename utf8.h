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

// Returns a decoder with an initial state
void utf8_decoder_init(utf8_Decoder *);

// Decodes subsequent bytes of a string into utf8 codepoints
// NOTE: Decoder has to be initialized to zero before decoding a new string!
bool utf8_decode(utf8_Decoder *, char);

// Encodes subsequent codepoints into utf8 and appends the resulting
// bytes to the given buffer, returning pointers to memory in the buffer
// after the appended bytes
char *utf8_encode(char *, uint32_t);

// Returns the number of utf8 bytes needed to encode a given codepoint
uint8_t utf8_size(uint32_t);

// Increments the pointer until it reaches past the current utf8 sequence
// and returns the resulting pointer
char *utf8_skip(char *);

// Returns a pointer to the first byte of the i-th codepoint of a given string
char *utf8_pos(char *, size_t);

// Returns the number of unicode codepoints in a given null-terminated string
size_t utf8_len(char *);

// Returns the number of unicode codepoints in a given string
size_t utf8_nlen(char *, size_t);

#endif // _UTF8_H
