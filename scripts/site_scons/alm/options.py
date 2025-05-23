#
# Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from SCons.Script import AddOption, GetOption


class AlmOptions(object):
    def __init__(self, env):
        self.env = env

    def add_option(self, name, **kwargs):
        if 'dest' not in kwargs:
            kwargs['dest'] = name

        col_width = 30

        help = ''
        if 'help' in kwargs:
           length = len(help)
           if length >= col_width:
               help += '\n' + ' ' * col_width
           else:
               help += ' ' * (col_width - length)
               help += kwargs['help']

        if 'metavar' not in kwargs and kwargs.get('type', None) == 'choice':
             kwargs['metavar'] = '[' + '|'.join(kwargs['choices']) + ']'
             deflt = kwargs.get('default', None)
             help += ' (default: ' + 'none' if deflt == None else deflt  + ')'

        kwargs['help'] = help
        AddOption('--' + name, **kwargs)

    def Setup(self):
        #import pdb
        #pdb.set_trace()
        self.add_option('build-dir',
                        nargs   = 1,
                        default = 'build',
                        type    = 'str',
                        #metavar = 'DIR',
                        help    = 'Build directory'
        )

        self.add_option('prefix',
                        nargs   = 1,
                        type    = 'str',
                        default = 'install',
                        help    = """Specify an install prefix directory \
                        the directory will be create if non-existant
                        """
        )

        self.add_option('verbose',
                        nargs   = 1,
                        type    = 'int',
                        default = 0,
                        help    = 'Print full tool command lines'
        )

        self.add_option('debug_mode',
                        nargs   = 1,
                        default = 'none',
                        type    = 'choice',
                        choices = ['none', 'libs', 'tests', 'all'],
                        help    = 'Debug mode'
        )

        self.add_option('libabi',
                        nargs   = 1,
                        default = 'aocl',
                        type    = 'choice',
                        choices = ['aocl', 'libm', 'svml', 'glibc','msvc'],
                        help = """Compile tests to call a particular abi \
                        aocl  - AOCL functions prefixed with 'amd_*' \
                        glibc - GLIBC functions, prefixed with '__ieee_*' \
                        libm  - C99 Standard library functions. \
                        svml  - Intel OneAPI library functions \
                        """
        )

        self.add_option('arch_config',
                        nargs   = 1,
                        default = 'none',
                        type    = 'choice',
                        choices = ['none', 'avx512'],
                        help    = 'Configure for avx512'
        )

        #address sanitizer usage
        self.add_option('use_asan',
                        nargs = 1,
                        default = 0,
                        type =  int,
                        help = "Compile to use address sanitizer"
        )

        #provide libau_cpuid install path
        self.add_option('aocl_utils_install_path',
                        nargs = 1,
                        default = '/usr/local/',
                        type = str,
                        metavar = 'DIR',
                        help = 'Provide AOCL UTILS install path',
        )

        #link libau_cpuid static/dynamic library, default is static linking
        self.add_option('aocl_utils_link',
                        nargs = 1,
                        default = 1,
                        type = int,
                        help = 'libau_cpuid static/dynamic linking',
        )

        self.add_option('developer',
                        nargs   = 1,
                        type    = 'int',
                        default = 0,
                        help    = 'Enable Developer mode'
        )

        self.add_option('toolchain-base',
                        nargs   = 1,
                        type    = 'str',
                        metavar = 'DIR',
                        help    = """Used as path for all programs with toolchain
                        --toolchain-base=/usr/local/toolchain/
                        CC will be used as /usr/local/toolchain/bin/gcc
                        if --compiler=gcc
                        """
        )

    def GetOption(self, opt):
        return GetOption(opt)
