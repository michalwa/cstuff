#include "unit.h"
#include "utf8.h"

int main() {
    test("utf8_len", {
        assert_eq(utf8_len("", 0),    0, "%d");
        assert_eq(utf8_len("a", 1),   1, "%d");
        assert_eq(utf8_len("a\0", 2), 2, "%d");

        assert_eq(utf8_len("😀", 4), 1, "%d");
        assert_eq(utf8_len("фж", 4), 2, "%d");
    });

    test("utf8_decode", {
        utf8_Decoder d = {0};

        for (char *c = "😀"; *c; c++)
            if (utf8_decode(&d, *c)) break;

        assert_eq(d.codepoint, 0x1F600, "%u");
    });

    test("utf8_encode", {
        char buf[5] = {0};

        utf8_encode(buf, 0x1F600);

        assert_streq(buf, "😀");
    });
}
