#include "unit.h"
#include "utf8.h"

int main() {
    test("utf8_decode", {
        utf8_Decoder d;

        utf8_decoder_init(&d);
        for (char *c = "😀"; *c; c++)
            if (utf8_decode(&d, *c)) break;
        assert_eq(d.codepoint, 0x1F600, "%u");

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
        assert_eq((int)utf8_size('z'),     1, "%d");
        assert_eq((int)utf8_size(0x1F600), 4, "%d");
        assert_eq((int)utf8_size(0x0416),  2, "%d");
    });

    test("utf8_skip", {
        assert_eq(*utf8_skip("ae"), 'e', "%c");
        assert_eq(*utf8_skip("фe"), 'e', "%c");
        assert_eq(*utf8_skip("😀e"), 'e', "%c");
    });

    test("utf8_pos", {
        char *c = "a😀bфc";
        assert_eq(*utf8_pos(c, 0),  'a', "%c");
        assert_eq(*utf8_pos(c, 1), *"😀", "%c");
        assert_eq(*utf8_pos(c, 2),  'b', "%c");
        assert_eq(*utf8_pos(c, 3), *"ф", "%c");
        assert_eq(*utf8_pos(c, 4),  'c', "%c");
    });

    test("utf8_len", {
        assert_eq((int)utf8_len("", 0),    0, "%d");
        assert_eq((int)utf8_len("a", 1),   1, "%d");
        assert_eq((int)utf8_len("a\0", 2), 2, "%d");

        assert_eq((int)utf8_len("😀", 4),   1, "%d");
        assert_eq((int)utf8_len("фж", 4),  2, "%d");
    });
}
