parse_test_bin = executable('parse_test', 'sp_parser_test.c', 
    objects : sp_obj,
    include_directories : inc)

macro_test_bin = executable('macro_test', 'sp_macro_test.c',
    objects : sp_obj,
    include_directories : inc)

structpack_test_bin = executable('structpack_test', 'structpack_test.c',
    objects : sp_obj,
    include_directories : inc)

test('parser test', parse_test_bin)
test('macro test', macro_test_bin)
test('structpack test', structpack_test_bin)
