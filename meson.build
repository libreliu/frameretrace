# Copyright © 2019 Intel Corporation

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

project('frameretrace', 'cpp', default_options : ['buildtype=debug',
                                                  'default_library=static'])

cpp = meson.get_compiler('cpp')

if cpp.get_argument_syntax() == 'msvc'
  add_project_arguments(
    '-D_USE_MATH_DEFINES',
    '-DNOMINMAX',
    '-D_CRT_SECURE_NO_DEPRECATE',
    '-D_CRT_SECURE_NO_WARNINGS',
    '-D_CRT_NONSTDC_NO_WARNINGS',
    '-D_SCL_SECURE_NO_DEPRECATE',
    '-D_SCL_SECURE_NO_WARNINGS',
    '-DWIN32',
    '-wd4018', # signed/unsigned mismatch
    '-wd4063', # not a valid value for switch of enum
    '-wd4100', # unreferenced formal parameter
    '-wd4127', # conditional expression is constant
    '-wd4244', # conversion from 'type1' to 'type2', possible loss of data
    '-wd4267', # conversion from 'type1' to 'type2', possible loss of data
    '-wd4505', # unreferenced local function has been removed
    '-wd4512', # assignment operator could not be generated
    '-wd4577', # 'noexcept' used with no exception handling mode specified
    '-wd4800', # forcing value to bool 'true' or 'false' (performance warning)
    '-wd4146', # unary minus operator applied to unsigned type
    language : ['cpp']
  )
elif cpp.get_argument_syntax() == 'gcc'
  add_project_arguments(
    cpp.get_supported_arguments([
                      '-fno-exceptions',
    ]),
    language : ['cpp']
  )

  # TODO: mingw specfiic bits
  # if get_option('sse4_2')
  #   add_project_arguments('-DENABLE_SSE42', '-msse4.2', langauge : ['cpp'])
  # endif
endif

qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: [
                       'Core',
                       'Gui',
                       'Qml',
                       'Quick',
                       'Widgets',
                       'OpenGL',
                       'Concurrent',
                     ])

cpp = meson.get_compiler('cpp')
libdl = cpp.find_library('dl', required : false)
libpng = dependency('libpng', required : true, fallback :['libpng', 'png_dep'])
libwaffle = dependency('waffle-1', required : false)
libx11 = cpp.find_library('X11', required : false)
libz = dependency('zlib', required : true, fallback : ['zlib', 'zlib_dep'])
protobuf_dep = subproject('protobuf').get_variable('protobuf_dep')
protoc = subproject('protobuf').get_variable('protoc')

proj_apitrace = subproject('apitrace')


if cpp.get_argument_syntax() == 'msvc'
  proj_getopt = subproject('getopt')
  idep_getopt = proj_getopt.get_variable('idep_getopt')
  dep_mhook = subproject('mhook').get_variable('dep_mhook')
else
  idep_getopt = dependency('', required : false)
  dep_mhook = dependency('', required : false)
endif

dep_apitrace = proj_apitrace.get_variable('dep_apitrace')

gtest_dep = dependency('gtest', fallback : ['gtest', 'gtest_dep'])

#cmake = import('cmake')
add_project_arguments('-DHAVE_BACKTRACE=0', language : 'cpp')
dep_md5 = subproject('md5').get_variable('dep_md5')
dep_brotli_enc = dependency('libbrotlienc', fallback : ['google-brotli', 'brotli_encoder_dep'])
dep_brotli_dec = dependency('libbrotlidec', fallback : ['google-brotli', 'brotli_decoder_dep'])
dep_khr = subproject('khronos').get_variable('dep_khr')

subdir('src')
