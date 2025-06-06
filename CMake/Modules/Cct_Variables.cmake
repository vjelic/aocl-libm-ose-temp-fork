#
# Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#--------------------------
# Enables Caching variables
#--------------------------
if(NOT DEFINED CUST_PROJ_CXX_STD)
  set(CUST_PROJ_CXX_STD "14")
endif()

set(PROJECT_NAME "libm" CACHE STRING "${CUST_PROJ_NAME}")
set(PROJECT_PREFIX "aocl" CACHE STRING "${CUST_PROJ_PREFIX}")

set(${PROJECT_PREFIX}_PROJ_PORT ${CUST_PROJ_BUGREPORT})
set(${PROJECT_PREFIX}_PROJ_DESCRIPTION ${CUST_PROJ_DESCRIPTION})
set(${PROJECT_PREFIX}_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${CUST_PROJ_SOURCE_DIR})

set(${PROJECT_PREFIX}_VENDOR_NAME ${VENDOR_NAME})
set(${PROJECT_PREFIX}_VENDOR_CONTACT ${VENDOR_CONTACT})

set(${PROJECT_PREFIX}_PROJ_GBENCH_TAG ${CUST_PROJ_GBENCH_TAG})
set(${PROJECT_PREFIX}_PROJ_URL ${CUST_PROJ_URL})

set(${PROJECT_PREFIX}_PROJ_GTEST_TAG ${CUST_PROJ_GTEST_TAG})
set(${PROJECT_PREFIX}_PROJ_GBENCH_TAG ${CUST_PROJ_GBENCH_TAG})

set(${PROJECT_PREFIX}_OPTION_BUILD_TESTS ${OPTION_BUILD_TESTS})
set(${PROJECT_PREFIX}_OPTION_ENABLE_SLOW_TESTS ${OPTION_ENABLE_SLOW_TESTS})
set(${PROJECT_PREFIX}_OPTION_ENABLE_BROKEN_TESTS ${OPTION_ENABLE_BROKEN_TESTS})

if (DEFINED ${PROJECT_PREFIX}_OPTION_ENABLE_SLOW_TESTS AND NOT DEFINED ${PROJECT_PREFIX}_OPTION_BUILD_TESTS)
    message(FATAL_ERROR "TESTS are not enabled")
endif()

set(${PROJECT_PREFIX}_OPTION_ENABLE_ASSERTIONS ${OPTION_ENABLE_ASSERTIONS})

set(${PROJECT_PREFIX}_OPTION_BUILD_DOCS ${OPTION_BUILD_DOCS})
set(${PROJECT_PREFIX}_OPTION_BUILD_EXAMPLES ${OPTION_BUILD_EXAMPLES})

set(${PROJECT_PREFIX}_OPTION_ENABLE_ASAN ${OPTION_ENABLE_ASAN})


set(${PROJECT_PREFIX}_OPTION_ENABLE_COVERAGE ${OPTION_ENABLE_COVERAGE})

#--------------------------
# PACKAGE and EXPORT Settings
#--------------------------
set(${PROJECT_PREFIX}_OPTION_EXPORT_NAMESPACE ${OPTION_EXPORT_NAMESPACE})
