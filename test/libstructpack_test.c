#include <stdio.h>

#include "../src/libstructpack.c"

static int rv = 0;

#define SP_TEST_ASSERT(cond, str) if ((cond)) {        \
        printf("[\033[1;32mPASS\033[0m] %s\n", (str)); \
    } else {                                           \
        printf("[\033[1;31mFAIL\033[0m] %s\n", (str)); \
        rv = 1;                                        \
    }                                                  

int main(void) {
    const char *fmt_str;
    /* Good format string */
    fmt_str = ">4s h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) == SP_OK, "parse good format string");

    /* Invalid chars */
    fmt_str = ">4s h i 4(4P Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid character");
    
    /* Check invalid endian char location */
    fmt_str = "4s >h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid endian location");
    
    /* Test beginning with '(' */
    fmt_str = "(4s) h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid opening '('");
    
    /* Test invalid ending character */
    fmt_str = ">4s h i 4(4B Q)]";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "pare invalid end character");
    
    /* Test max depth detection */
    fmt_str = ">4s h i 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q))))))))))";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse beyond max group depth");
    
    return rv;
}