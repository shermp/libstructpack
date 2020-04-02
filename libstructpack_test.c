#include <stdio.h>

#include "libstructpack.c"

#define PRINT_PASS(x) printf("\033[1;32m[PASS]\033[0m %s\n", x) 
#define PRINT_FAIL(x) printf("\033[1;31m[FAIL]\033[0m %s\n", x) 

int main(void) {
    const char *fmt_str;
    /* Good format string */
    fmt_str = ">4s h i 4(4B Q)";
    if (validate_format_str(fmt_str) != SP_OK) {
        PRINT_FAIL("Failed parsing good format string");
    } else {
        PRINT_PASS("Successfully parsed good format string");
    }
    /* Invalid chars */
    fmt_str = ">4s h i 4(4P Q)";
    if (validate_format_str(fmt_str) == SP_OK) {
        PRINT_FAIL("Successfully parsed invalid character");
    } else {
        PRINT_PASS("Invalid character detected");
    }
    /* Check invalid endian char location */
    fmt_str = "4s >h i 4(4B Q)";
    if (validate_format_str(fmt_str) == SP_OK) {
        PRINT_FAIL("Successfully parsed invalid endian char location");
    } else {
        PRINT_PASS("Incorrect endian location detected");
    }
    /* Test beginning with '(' */
    fmt_str = "(4s) h i 4(4B Q)";
    if (validate_format_str(fmt_str) == SP_OK) {
        PRINT_FAIL("Successfully parsed '(' starting at beginning of format string");
    } else {
        PRINT_PASS("Incorrect '(' location detected");
    }
    /* Test invalid ending character */
    fmt_str = ">4s h i 4(4B Q)]";
    if (validate_format_str(fmt_str) == SP_OK) {
        PRINT_FAIL("Successfully parsed incorrect character at end of format string");
    } else {
        PRINT_PASS("Incorrect end character detected");
    }
    /* Test max depth detection */
    fmt_str = ">4s h i 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q))))))))))";
    if (validate_format_str(fmt_str) == SP_OK) {
        PRINT_FAIL("Successfully parsed beyond max group depth");
    } else {
        PRINT_PASS("Detected exceeded max group depth");
    }
}