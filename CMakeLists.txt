cmake_minimum_required(VERSION 3.2)

project(sshd)

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

if (NOT LIBSSL_FOUND)
	pkg_check_modules(LIBSSL REQUIRED libssl>=1.0.2l)
endif()

if (NOT LIBCRYPTO_FOUND)
	pkg_check_modules(LIBCRYPTO REQUIRED libcrypto>=1.0.2l)
endif()

if (LIBSSL_FOUND)
	pkg_check_modules(LIBSSH REQUIRED libssh>=0.7.3)
endif()

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

include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})
include_directories(SYSTEM ${LIBSSH_INCLUDE_DIRS})
include_directories(SYSTEM ${Boost_INCLUDES})

set(EXPORT_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/include/export
)

add_library(
	${PROJECT_NAME}
	${CMAKE_CURRENT_SOURCE_DIR}/src/channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/channel_callbacks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/client_session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/proxy_channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sshkey.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/types.cpp

	${EXPORT_INCLUDES}/ssh/channel.hh
	${EXPORT_INCLUDES}/ssh/client_session.hh
	${EXPORT_INCLUDES}/ssh/exception.hh
	${EXPORT_INCLUDES}/ssh/session.hh
	${EXPORT_INCLUDES}/ssh/sshkey.hh
	${EXPORT_INCLUDES}/ssh/types.hh
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
		${EXPORT_INCLUDES}
)

target_link_directories(
	${PROJECT_NAME}
	PUBLIC
	${LIBCRYPTO_LIBRARY_DIRS}
	${LIBSSL_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)
target_link_libraries(
	${PROJECT_NAME}
	LINK_PUBLIC
		base64
		typespp
		easyloggingpp
		${Boost_LIBRARIES}
		${LIBSSH_LIBRARIES}
		${LIBSSL_LIBRARIES}
		${LIBCRYPTO_LIBRARIES}
)
