#include "unit.h"
#include "utf8.h"

int main() {
    test("utf8_decode", {
        utf8_Decoder d;

        utf8_decoder_init(&d);
        for (char *c = "😀"; *c; c++)
            if (utf8_decode(&d, *c)) break;
        assert_eq(0x1F600, d.codepoint, "%u");

        utf8_decoder_init(&d);
        for (char *c = "z"; *c; c++)
            if (utf8_decode(&d, *c)) break;
        assert_eq(d.codepoint, (int)'z', "%u");
    });

    test("utf8_encode", {
        char buf[9] = {0};

        char *c = buf;
        c = utf8_encode(c, 0x1F600);
        c = utf8_encode(c, 0x1F600);
        *c = '\0';

        assert_streq(buf, "😀😀");
    });

    test("utf8_size", {
        assert_eq(1, (int)utf8_size('z'),     "%d");
        assert_eq(4, (int)utf8_size(0x1F600), "%d");
        assert_eq(2, (int)utf8_size(0x0416),  "%d");
    });

    test("utf8_skip", {
        assert_eq('e', *utf8_skip("ae"), "%c");
        assert_eq('e', *utf8_skip("фe"), "%c");
        assert_eq('e', *utf8_skip("😀e"), "%c");
    });

    test("utf8_pos", {
        char *c = "a😀bфc";
        assert_eq( 'a', *utf8_pos(c, 0), "%c");
        assert_eq(*"😀", *utf8_pos(c, 1), "%c");
        assert_eq( 'b', *utf8_pos(c, 2), "%c");
        assert_eq(*"ф", *utf8_pos(c, 3), "%c");
        assert_eq( 'c', *utf8_pos(c, 4), "%c");
    });

    test("utf8_len", {
        assert_eq(0, (int)utf8_len(""),    "%d");
        assert_eq(1, (int)utf8_len("a"),   "%d");
        assert_eq(1, (int)utf8_len("a\0"), "%d");

        assert_eq(1, (int)utf8_len("😀"),   "%d");
        assert_eq(2, (int)utf8_len("фж"),  "%d");
    });

    test("utf8_nlen", {
        assert_eq(0, (int)utf8_nlen("", 0),    "%d");
        assert_eq(1, (int)utf8_nlen("a", 1),   "%d");
        assert_eq(2, (int)utf8_nlen("a\0", 2), "%d");

        assert_eq(1, (int)utf8_nlen("😀", 4),   "%d");
        assert_eq(2, (int)utf8_nlen("фж", 4),  "%d");
    });
}
