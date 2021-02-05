#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unit.h"
#include "strutils.h"


#define with(type, var, alloc, block) do { \
    type *var = alloc; \
    do block while(0); \
    free(var); \
} while(0);

#define assert_string_eq(a, b) \
    assert_custom_eq(a, b, str_eq, STR_FMT, STR_FMT_ARGS);


#define STR_MIN_BUFSZ 0x80

int main() {
    const char *s1 = "Hello, world!";
    const char *s2 = "Hello";

    String str1 = str_ref(s1);     // "Hello, world!"
    String str2 = str_nref(s1, 5); // "Hello"
    String str3 = str_ref("l");    // "l"

    String heap = str_alloc("Hello, world!");

    // Demo print
    printf("str_ref(s1) = "STR_DEBUG_FMT"\n", STR_DEBUG_FMT_ARGS(str1));
    printf("str_alloc() = "STR_DEBUG_FMT"\n", STR_DEBUG_FMT_ARGS(heap));

    // Test equality comparison first, so we can use it in assertions
    test("str_eq", {
        assert(str_eq(str1, str1));
        assert(!str_eq(str1, str2));
        assert(!str_eq(str_ref("Hello"), str_ref("Hellr")));
    });

    test("str_lpos", {
        assert_eq(0,  str_lpos(str1, str1, 0), "%d");
        assert_eq(2,  str_lpos(str3, str1, 0), "%d");
        assert_eq(10, str_lpos(str3, str1, 4), "%d");

        assert_eq(-1, str_lpos(str_ref("love"), str1, 0), "%d");
        assert_eq(3,  str_lpos(str_ref("lo"), str1, 0), "%d");
    });

    test("str_rpos", {
        assert_eq(0,  str_rpos(str1, str1, 0), "%d");
        assert_eq(10, str_rpos(str3, str1, 0), "%d");
        assert_eq(3,  str_rpos(str3, str1, 4), "%d");

        assert_eq(-1, str_rpos(str_ref("love"), str1, 0), "%d");
        assert_eq(3,  str_rpos(str_ref("lo"), str1, 0), "%d");
    });

    test("cstr", {
        String str4 = str_alloc(s1);
        String str5 = str_nalloc(s1, 5);

        with(char, s, cstr(str1), assert_streq(s1, s));
        with(char, s, cstr(heap), assert_streq(s1, s));

        with(char, s, cstr(str2), assert_streq(s2, s));
        with(char, s, cstr(str4), assert_streq(s1, s));
        with(char, s, cstr(str5), assert_streq(s2, s));

        str_free(str4);
        str_free(str5);
    });

    test("str_slice", {
        String slice = str_slice(str1, 0, 5);
        assert_string_eq(str_ref("Hello"), str_slice_ref(str1, 0, 5));
        assert_string_eq(str_ref("Hello"), slice);
        str_free(slice);

        slice = str_slice(heap, 0, 5);
        assert_string_eq(str_ref("Hello"), str_slice_ref(heap, 0, 5));
        assert_string_eq(str_ref("Hello"), slice);
        str_free(slice);

        slice = str_slice(str1, 7, 100);
        assert_string_eq(str_ref("world!"), str_slice_ref(str1, 7, 100));
        assert_string_eq(str_ref("world!"), slice);
        str_free(slice);

        slice = str_slice(str1, 5, 0);
        assert_string_eq(str_ref(""), str_slice_ref(str1, 5, 0));
        assert_string_eq(str_ref(""), slice);
        str_free(slice);
    });

    test("str_count", {
        assert_eq(0, str_count('a', str1), "%d");
        assert_eq(0, str_count('a', str2), "%d");
        assert_eq(3, str_count('l', str1), "%d");
        assert_eq(2, str_count('o', str1), "%d");
        assert_eq(1, str_count('!', str1), "%d");
        assert_eq(2, str_count('l', str2), "%d");
    });

    test("str_counts", {
        assert_eq(1, str_counts(str2, str1, 0), "%d");
        assert_eq(3, str_counts(str3, str1, 0), "%d");
        assert_eq(2, str_counts(str3, str2, 0), "%d");

        assert_eq(1, str_counts(str_ref("foofoo"),
                                str_ref("foofoofoo"), 0), "%d");

        assert_eq(2, str_counts(str_ref("foofoo"),
                                str_ref("foofoofoo"), STR_COUNT_OVERLAP), "%d");
    });

    test("str_startswith", {
        assert(str_startswith(str2, str1));
        assert(str_startswith(str1, str1));
        assert(str_startswith(str2, str2));

        assert(!str_startswith(str1, str2));
        assert(!str_startswith(str3, str1));
    });

    test("str_endswith", {
        assert(str_endswith(str_ref("world!"), str1));
        assert(str_endswith(str1, str1));
        assert(str_endswith(str2, str2));

        assert(!str_endswith(str2, str1));
        assert(!str_endswith(str1, str2));
    });

    test("str_fmt", {
        with(char, s, cstr(str_fmt(10, "")), assert_streq("", s));
        with(char, s, cstr(str_fmt(100, "Hello, %s!", "world")),
            assert_streq("Hello, world!", s));
    });

    test("str_push", {
        String str = str_alloc("Hello, wor");
        String clone = str_clone(str);

        str_push('l', &str);
        str_push('d', &str);
        str_push('!', &str);

        assert_string_eq(str_ref("Hello, world!"), str);
        assert_string_eq(str_ref("Hello, wor"), clone);

        str_free(str);
        str_free(clone);
    });

    test("str_push (overflow)", {
        String str = str_alloc("");
        for (size_t i = 0; i < STR_MIN_BUFSZ; i++) str_push('#', &str);

        assert_eq(str.bufsz, (size_t)STR_MIN_BUFSZ, "%zu");

        str_push('.', &str);

        assert_eq(str.bufsz, (size_t)STR_MIN_BUFSZ * 2, "%zu");

        str_free(str);
    });

    test("str_pushs", {
        String str = str_alloc("Hello");
        String clone = str_clone(str);

        str_pushs(str_ref(", "),    &str);
        str_pushs(str_ref("world"), &str);
        str_pushs(str_ref("!"),     &str);

        assert(str_eq(str_ref("Hello, world!"), str));

        // Should do nothing
        str_pushs(str_ref(""), &str);

        assert_string_eq(str_ref("Hello, world!"), str);
        assert_string_eq(str_ref("Hello"), clone);

        str_free(str);
        str_free(clone);
    });

    test("str_pop", {
        String str = str_alloc("foo");

        char c;

        assert(str_pop(&str, &c));
        assert_eq('o', c, "%c");

        assert(str_pop(&str, &c));
        assert_eq('o', c, "%c");

        assert(str_pop(&str, &c));
        assert_eq('f', c, "%c");

        assert(!str_pop(&str, NULL));

        str_free(str);
    });

    test("str_popn", {
        String str = str_alloc("Hello, world!");

        String s;

        assert(str_popn(&str, 1, &s));
        assert_string_eq(str_ref("!"), s);
        assert_string_eq(str_ref("Hello, world"), str);
        str_free(s);

        assert(str_popn(&str, 5, NULL));

        // Should pop empty string and not change the original string
        assert(str_popn(&str, 0, &s));
        assert_string_eq(str_ref(""), s);
        assert_string_eq(str_ref("Hello, "), str);
        str_free(s);

        assert(!str_popn(&str, 10, NULL));

        assert(str_popn(&str, 7, &s));
        assert_string_eq(str_ref("Hello, "), s);
        assert_string_eq(str_ref(""), str);
        str_free(s);

        assert(!str_popn(&str, 1, NULL));

        str_free(str);
    });

    test("str_insert", {
        String str = str_alloc("Helloworld");

        str_insert(',', 5, &str);
        assert_string_eq(str_ref("Hello,world"), str);

        str_insert(' ', 6, &str);
        assert_string_eq(str_ref("Hello, world"), str);

        str_insert('!', 100, &str);
        assert_string_eq(str_ref("Hello, world!"), str);

        str_free(str);
    });

    test("str_inserts", {
        String str = str_alloc("world");

        str_inserts(str_ref("Hello, "), 0, &str);
        assert_string_eq(str_ref("Hello, world"), str);

        // Should do nthing
        str_inserts(str_ref(""), 3, &str);
        assert_string_eq(str_ref("Hello, world"), str);

        str_inserts(str_ref("!"), 100, &str);
        assert_string_eq(str_ref("Hello, world!"), str);

        str_free(str);
    });

    test("str_replace_slice", {
        String str = str_alloc("Hello, world!");

        str_replace_slice(7, 5, str_ref("life"), &str);
        assert_string_eq(str_ref("Hello, life!"), str);

        str_free(str);
    });

    test("str_replace", {
        String orig = str_ref("Hello, foo foo bar!");
        String str = str_clone(orig);

        str_replace(str_ref("zoo"), str_ref(""), &str, 0);
        assert_string_eq(orig, str);

        str_replace(str_ref("foo"), str_ref("bar"), &str, 0);
        assert_string_eq(str_ref("Hello, bar foo bar!"), str);

        str_replace(str_ref("bar"), str_ref("baz"), &str, STR_REPLACE_ALL);
        assert_string_eq(str_ref("Hello, baz foo baz!"), str);

        str_replace(str_ref("baz"), str_ref("foo"), &str, STR_REPLACE_REVERSE);
        assert_string_eq(str_ref("Hello, baz foo foo!"), str);

        str_free(str);
    });

    String to_strip = str_ref(" . foo bar . ");

    test("str_strip (none)", {
        String str = str_strip(" .", to_strip, 0, NULL);
        assert_string_eq(to_strip, str);
    });

    test("str_strip (left)", {
        String str = str_strip(" .", to_strip, STR_STRIP_LEFT, NULL);
        assert_string_eq(str_ref("foo bar . "), str);
    });

    test("str_strip (right)", {
        String str = str_strip(" .", to_strip, STR_STRIP_RIGHT, NULL);
        assert_string_eq(str_ref(" . foo bar"), str);
    });

    test("str_strip (both)", {
        String str = str_strip(" .", to_strip,
            STR_STRIP_LEFT | STR_STRIP_RIGHT, NULL);
        assert_string_eq(str_ref("foo bar"), str);
    });

    str_free(heap);
}
