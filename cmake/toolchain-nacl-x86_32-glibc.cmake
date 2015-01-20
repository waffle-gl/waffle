#
# NaCl toolchain file for 32bit x86 using glibc C library
#

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_SYSTEM_NAME "Linux")
    set(nacl_host_os "linux")
else()
    message(FATAL_ERROR "TODO: NaCl support on ${CMAKE_HOST_SYSTEM_NAME}")
endif()

set(nacl_target_arch "i686")
set(nacl_ports "glibc_x86_32")
set(nacl_toolchain "${nacl_host_os}_x86_glibc")

# setup paths for nacl
set(nacl_root ${nacl_sdk_path}/${nacl_version})
set(nacl_toolpath ${nacl_root}/toolchain/${nacl_toolchain}/bin)

# setup compilers from toolchain
set(CMAKE_C_COMPILER ${nacl_toolpath}/${nacl_target_arch}-nacl-gcc)
set(CMAKE_CXX_COMPILER ${nacl_toolpath}/${nacl_target_arch}-nacl-g++)

set(CMAKE_FIND_ROOT_PATH ${nacl_root})

# for FIND_LIBRARY|INCLUDE use ${nacl_root} only,
# for helper programs during build time, use host
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# setup nacl includes and required libraries
set(nacl_INCLUDE_DIRS ${nacl_INCLUDE_DIRS} ${nacl_sdk_path}/${nacl_version}/include)
set(nacl_LIBS ${nacl_sdk_path}/${nacl_version}/lib/${nacl_ports}/${CMAKE_BUILD_TYPE})
set(nacl_LDFLAGS
    -L${nacl_LIBS}
    -lppapi_cpp
    -lppapi
    -lpthread
    -ldl
    )
