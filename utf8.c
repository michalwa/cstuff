#include "utf8.h"

void utf8_decoder_init(utf8_Decoder *d) {
    d->state = 0;
    d->codepoint = 0;
}

bool utf8_decode(utf8_Decoder *d, char byte) {
    // If last codepoint was decoded, begin new one
    if (!d->state) {
        // Decode header (number of following bytes)
        for (; byte & 0x80; byte <<= 1) d->state++;
        // Store remaining header bits into codepoint
        d->codepoint = byte >> d->state;
        // Decrement number of expected following bytes by 1
        if (d->state) d->state--;
        // Return true if this was just a 7-bit ASCII char
        return !d->state;
    }

    // Get next 6 bits from utf8 sequence byte
    d->codepoint <<= 6;
    d->codepoint |= byte & 0x3F;

    // Return true whenever number of expected following bytes reaches 0
    return !--d->state;
}

char *utf8_encode(char *buffer, uint32_t codepoint) {
    // 7-bit ASCII bytes just get appended to the buffer
    if (codepoint < 0x80) {
        *buffer++ = codepoint;
        return buffer;
    }

    // Determine number of required utf8 bytes
    uint8_t len = 2;
    // `max' keeps track of the maximum value that can be stored in `len' utf8 bytes
    for (uint64_t max = 1 << 11; codepoint >= max; max <<= 5) len++;

    // Write header
    char header = (0xFF00 >> len);
    *buffer++ = header;

    // Write bytes (in reverse)
    buffer += len - 1;
    for (uint8_t i = 0; i < len - 1; i++) {
        // 10.. ....
        //   ^ 6 bits of codepoint
        *--buffer = 0x80 | (codepoint & 0x3F);
        codepoint >>= 6;
    }

    // Write remaining bits to header
    *--buffer |= codepoint;

    return buffer + len;
}

inline uint8_t utf8_size(uint32_t codepoint) {
    char buf[4];
    return utf8_encode(buf, codepoint) - buf;
}

#include <stdio.h>

inline char *utf8_skip(char *c) {
    // Skip "10xxxxxx" bytes, so the pointer either ends up
    // at a "0xxxxxxx" (ascii) or a "11xxxxxx" (utf8 header)
    for (c++; (*c & 0xC0) == 0x80; c++);
    return c;
}

inline char *utf8_pos(char *c, size_t i) {
    // Same as utf8_skip, but decrements `i' instead of instantly
    // exiting the loop, and exits the loop only when `i' is 0
    while (i--) c = utf8_skip(c);
    return c;
}

inline size_t utf8_len(char *c) {
    size_t len = 0;

    // Same as utf8_pos, but increments length until the pointer reaches
    // the null byte
    while (*c) if ((*c++ & 0xC0) != 0x80) len++;
    return len;
}

inline size_t utf8_nlen(char *str, size_t sz) {
    size_t len = 0;
    char *c = str;

    // Same as utf8_pos, but increments length until the pointer reaches
    // end of string (str + sz)
    while ((size_t)(c - str) < sz)
        if ((*c++ & 0xC0) != 0x80) len++;

    return len;
}
