#include <stdio.h>
#include "unit.h"
#include "strutils.h"

#define with(type, var, alloc, block) do { \
    type *var = alloc; \
    do block while(0); \
    free(var); \
} while(0);

int main() {
    const char *s1 = "Hello, world!";
    const char *s2 = "Hello";

    String str1 = str_ref(s1);     // "Hello, world!"
    String str2 = str_nref(s1, 5); // "Hello"
    String str3 = str_ref("l");    // "l"

    // Demo print
    printf(STR_DEBUG_FMT"\n", STR_DEBUG_FMT_ARGS(str1));

    // Test equality comparison first, so we can use it in assertions
    test("str_eq", {
        assert(str_eq(str1, str1));
        assert(!str_eq(str1, str2));
        assert(!str_eq(str_ref("Hello"), str_ref("Hellr")));
    });

    test("cstr", {
        String str4 = str_alloc(s1);
        String str5 = str_nalloc(s1, 5);

        with(char, s, cstr(str1), assert_streq(s1, s));
        with(char, s, cstr(str2), assert_streq(s2, s));
        with(char, s, cstr(str4), assert_streq(s1, s));
        with(char, s, cstr(str5), assert_streq(s2, s));

        str_free(str4);
        str_free(str5);
    });

    test("str_slice", {
        String slice = str_slice(str1, 0, 5);
        assert(str_eq(str_ref("Hello"), str_slice_ref(str1, 0, 5)));
        assert(str_eq(str_ref("Hello"), slice));
        str_free(slice);

        slice = str_slice(str1, 7, 5);
        assert(str_eq(str_ref("world"), str_slice_ref(str1, 7, 5)));
        assert(str_eq(str_ref("world"), slice));
        str_free(slice);

        slice = str_slice(str1, 5, 0);
        assert(str_eq(str_ref(""), str_slice_ref(str1, 5, 0)));
        assert(str_eq(str_ref(""), slice));
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
        assert_eq(1, str_counts(str2, str1), "%d");
        assert_eq(3, str_counts(str3, str1), "%d");
        assert_eq(2, str_counts(str3, str2), "%d");
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

    test("str_append", {
        String str = str_alloc("Hello, wor");
        String clone = str_clone(str);

        str_append('l', &str);
        str_append('d', &str);
        str_append('!', &str);

        assert(str_eq(str_ref("Hello, world!"), str));
        assert(str_eq(str_ref("Hello, wor"), clone));

        printf("\nOriginal: "STR_DEBUG_FMT"\n", STR_DEBUG_FMT_ARGS(clone));
        printf("Appended: "STR_DEBUG_FMT, STR_DEBUG_FMT_ARGS(str));

        str_free(str);
        str_free(clone);
    });

    test("str_appends", {
        String str = str_alloc("Hello");
        String clone = str_clone(str);

        str_appends(str_ref(", "),    &str);
        str_appends(str_ref("world"), &str);
        str_appends(str_ref("!"),     &str);

        assert(str_eq(str_ref("Hello, world!"), str));
        assert(str_eq(str_ref("Hello"),         clone));

        printf("\nOriginal: "STR_DEBUG_FMT"\n", STR_DEBUG_FMT_ARGS(clone));
        printf("Appended: "STR_DEBUG_FMT, STR_DEBUG_FMT_ARGS(str));

        str_free(str);
        str_free(clone);
    });
}
