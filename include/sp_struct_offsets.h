#ifndef SP_STRUCT_OFFSETS_H
#define SP_STRUCT_OFFSETS_H

#include <stddef.h>

/* The following macros help make populating an offset list less tedious */

#define SP_ADD_STRUCT_OFFSET_1(arr, index, type, m1) (arr)[(index)] = offsetof(type, m1)

#define SP_ADD_STRUCT_OFFSET_2(arr, index, type, m1, m2) \
    SP_ADD_STRUCT_OFFSET_1((arr), (index), type, m1); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 1, type, m2)

#define SP_ADD_STRUCT_OFFSET_3(arr, index, type, m1, m2, m3) \
    SP_ADD_STRUCT_OFFSET_2((arr), (index), type, m1, m2); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 2, type, m3)

#define SP_ADD_STRUCT_OFFSET_4(arr, index, type, m1, m2, m3, m4) \
    SP_ADD_STRUCT_OFFSET_3((arr), (index), type, m1, m2, m3); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 3, type, m4)

#define SP_ADD_STRUCT_OFFSET_5(arr, index, type, m1, m2, m3, m4, m5) \
    SP_ADD_STRUCT_OFFSET_4((arr), (index), type, m1, m2, m3, m4); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 4, type, m5)

#define SP_ADD_STRUCT_OFFSET_6(arr, index, type, m1, m2, m3, m4, m5, m6) \
    SP_ADD_STRUCT_OFFSET_5((arr), (index), type, m1, m2, m3, m4, m5); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 5, type, m6)

#define SP_ADD_STRUCT_OFFSET_7(arr, index, type, m1, m2, m3, m4, m5, m6, m7) \
    SP_ADD_STRUCT_OFFSET_6((arr), (index), type, m1, m2, m3, m4, m5, m6); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 6, type, m7)

#define SP_ADD_STRUCT_OFFSET_8(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8) \
    SP_ADD_STRUCT_OFFSET_7((arr), (index), type, m1, m2, m3, m4, m5, m6, m7); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 7, type, m8)

#define SP_ADD_STRUCT_OFFSET_9(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9) \
    SP_ADD_STRUCT_OFFSET_8((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 8, type, m9)

#define SP_ADD_STRUCT_OFFSET_10(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10) \
    SP_ADD_STRUCT_OFFSET_9((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9); \
    SP_ADD_STRUCT_OFFSET_1((arr), (index) + 9, type, m10)

#endif // SP_STRUCT_OFFSETS_H
