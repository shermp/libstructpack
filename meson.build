project('libstructpack', 'c',
    version : '0.1.0',
    license : 'MIT',
    default_options : [
        'c_std=c99',
        'warning_level=3',
        'werror=true',
        'b_ndebug=if-release',
    ])

cc = meson.get_compiler('c')

foreach flag : ['-Wmaybe-uninitialized', '-Wextra-semi']
    if cc.has_argument(flag)
        add_project_arguments(flag, language : 'c')
    endif
endforeach

inc = include_directories('include')

subdir('src')
subdir('test')
