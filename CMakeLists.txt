cmake_minimum_required(VERSION 3.2)

project(sshd LANGUAGES C CXX)

include(CheckCSourceCompiles)
include(CheckCCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CheckCXXCompilerFlag)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibSSH REQUIRED libssh)
pkg_check_modules(OpenSSL REQUIRED openssl)

check_include_file_cxx(iostream HAVE_IOSTREAM)
check_include_file_cxx(memory HAVE_MEMORY)
check_include_file_cxx(functional HAVE_FUNCTIONAL)
check_include_file_cxx(string HAVE_STRING)
check_include_file_cxx(fstream HAVE_FSTREAM)
check_include_file_cxx(iostream HAVE_FSTREAM)
check_include_file_cxx(thread HAVE_THREAD)
check_include_file_cxx(atomic HAVE_ATOMIC)
check_include_file_cxx(array HAVE_ARRAY)
check_include_file_cxx(map HAVE_MAP)
check_include_file_cxx(list HAVE_LIST)
check_include_file_cxx(queue HAVE_QUEUE)
check_include_file_cxx(vector HAVE_VECTOR)
check_include_file_cxx(algorithm HAVE_ALGORITHM)
check_include_file_cxx(exception HAVE_EXCEPTION)
check_include_file_cxx(stdexcept HAVE_STDEXCEPT)
check_include_file_cxx(mutex HAVE_MUTEX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${LIBSSH_INCLUDE_DIRS})
include_directories(${Boost_INCLUDES})

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${LibSSH_LIBRARY_DIRS})
link_directories(${OpenSSL_LIBRARY_DIRS})

set(EXPORT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/export
)

add_library(
	${PROJECT_NAME} STATIC

	${CMAKE_CURRENT_SOURCE_DIR}/src/channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/channel_callbacks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/client_session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/proxy_channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sshkey.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/types.cpp

	${EXPORT_INCLUDES}/ssh/channel.hpp
	${EXPORT_INCLUDES}/ssh/client_session.hpp
	${EXPORT_INCLUDES}/ssh/exception.hpp
	${EXPORT_INCLUDES}/ssh/session.hpp
	${EXPORT_INCLUDES}/ssh/sshkey.hpp
	${EXPORT_INCLUDES}/ssh/types.hpp
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${EXPORT_INCLUDES}
	${OpenSSL_INCLUDE_DIRS}
	${LibSSH_INCLUDE_DIRS}
	${Boost_INCLUDES}
)

target_link_libraries(
	${PROJECT_NAME}
	base64
	typespp
	${Boost_LIBRARIES}
	${LibSSH_LIBRARIES}
	ssh_threads
    ${OpenSSL_LIBRARIES}
)
