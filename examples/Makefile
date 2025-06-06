#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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


# the compiler to use
CC      = gcc
CFLAGS  = -march=native -g

# if running avx512 variants
ARCH = avx2
ifeq ($(ARCH), avx512)
    CFLAGS += -mavx512f
    CFLAGS += -D__AVX512__
endif

# use static or dynamic library to link
LIB = dynamic #default value

#header
header_path = $(abspath ../include/external/)
aocl_utils_header_path = $(AOCL_UTILS_INSTALL_PATH)/include/alci
INC         = -I$(header_path)
INC         += -I$(aocl_utils_header_path)

#path to libalm.so
lib_path = $(abspath ../build/aocl-release/src/)
aocl_utils_lib_path = $(AOCL_UTILS_INSTALL_PATH)/lib
LFLAGS   := -L$(lib_path)
LFLAGS   += -L$(aocl_utils_lib_path)

ifeq ($(LIB), static)
    LFLAGS += -static
endif

LFLAGS += -lalm -lm -lau_cpuid -lstdc++

#src path
SOURCES = ./src/*.c

#exe file name
TARGET = ./test_libm

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SOURCES) $(LFLAGS)

clean:
	rm -rf $(TARGET)
