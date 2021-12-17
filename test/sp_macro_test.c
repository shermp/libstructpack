#include <sp_struct_offsets.h>

#include "sp_test.h"

struct s_test {
    int mem1;
    int mem2;
    int mem3;
    int mem4;
    int mem5;
    int mem6;
    int mem7;
    int mem8;
    int mem9;
    int mem10;
    int mem11;
    int mem12;
    int mem13;
    int mem14;
    int mem15;
};

static int rv = 0;

int main(void) {
    size_t offset_arr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    SP_ADD_STRUCT_OFFSET_10(offset_arr, 0, struct s_test, mem1, mem2, mem3, mem4, mem5, mem6, mem7, mem8, mem9, mem10);
    for (size_t i = 0; i < 10; ++i) {
        SP_TEST_ASSERT(rv, offset_arr[i] == (sizeof(int) * i), "test offset_10 macro");
    }
    SP_ADD_STRUCT_OFFSET_5(offset_arr, 10, struct s_test, mem11, mem12, mem13, mem14, mem15);
    for (size_t i = 10; i < 15; ++i) {
        SP_TEST_ASSERT(rv, offset_arr[i] == (sizeof(int) * i), "test offset_5 macro");
    }

    return rv;
}
