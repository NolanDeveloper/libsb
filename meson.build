project(
  'libsb',
  'c',
  version: '0.1',
  default_options: ['warning_level=3', 'werror=true', 'c_std=c11']
)

include_directories = include_directories('include')

install_headers('include/libsb.h')

libgb_dep = dependency('gb', required: false)
if not libgb_dep.found()
  libgb_proj = subproject('libgb')
  libgb_dep = libgb_proj.get_variable('libgb_dep')
endif

libutf_dep = dependency('utf', required: false)
if not libutf_dep.found()
  libutf_proj = subproject('libutf')
  libutf_dep = libutf_proj.get_variable('libutf_dep')
endif

src = [
  'src/libsb.c'
]

dependencies = [
  libgb_dep,
  libutf_dep,
]

c_compiler = meson.get_compiler('c')

code_with_attribute_format = '''
__attribute__((format(printf, 1, 2)))
int myprint(const char *fmt, ...) {
   (void) fmt;
   return 0;
}
int main() {
   myprint("%d", 42);
}
'''

cpp_flags = []
if c_compiler.compiles(code_with_attribute_format)
  cpp_flags += '-DSUPPORTS_ATTRIBUTE_FORMAT'
endif

libsb = library(
  'sb',
  src,
  install: true,
  dependencies: dependencies,
  include_directories: include_directories,
  c_args: cpp_flags
)

pkg = import('pkgconfig')
pkg.generate(libsb)

libsb_dep = declare_dependency(
  include_directories: include_directories,
  link_with: libsb
)

libsb_test_unit = executable(
  'libsb_test_unit',
  ['test-unit/main.c'],
  link_with: libsb,
  include_directories: include_directories
)

test('unit test', libsb_test_unit)