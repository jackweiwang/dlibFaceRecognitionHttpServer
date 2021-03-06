cmake_minimum_required (VERSION 2.8)
cmake_policy(VERSION 2.8)
project(libonion)

# Some tweak parameters
SET(ONION_USE_SSL true CACHE BOOL "Use SSL, requires GnuTLS")
SET(ONION_USE_PAM true CACHE BOOL "Compile PAM handler. Needs libpam")
SET(ONION_USE_PTHREADS true CACHE BOOL "Compile with pthreads support. Needed for O_THREAD and O_POOL modes")
SET(ONION_USE_PNG true CACHE BOOL "Adds support for simple image handler")
SET(ONION_USE_JPEG true CACHE BOOL "Adds support for simple image handler")
SET(ONION_USE_XML2 true CACHE BOOL "Adds support for XML2 lib, which is needed for WebDAV handler")
SET(ONION_USE_SYSTEMD true CACHE BOOL "Adds simple support for systemd")
SET(ONION_USE_SQLITE3 true CACHE BOOL "Adds support for sqlite3 sessions")
SET(ONION_USE_REDIS true CACHE BOOL "Adds support for redis sessions")
SET(ONION_USE_GC true CACHE BOOL "Compile Boehm GC examples")
SET(ONION_USE_TESTS true CACHE BOOL "Compile the tests")
SET(ONION_EXAMPLES true CACHE BOOL "Compile the examples")
SET(ONION_USE_BINDINGS_CPP true CACHE BOOL "Compile the CPP bindings")
SET(ONION_POLLER default CACHE string "Default poller to use: default | epoll | libev | libevent")
########

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

execute_process(
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/git-version-gen" "${CMAKE_CURRENT_SOURCE_DIR}"
	OUTPUT_VARIABLE ONION_VERSION)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" ONION_VERSION_MAJOR ${ONION_VERSION})
string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\..*" "\\1" ONION_VERSION_MINOR ${ONION_VERSION})
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)\\..*" "\\1" ONION_VERSION_PATCH ${ONION_VERSION})

message(STATUS "Onion version is ${ONION_VERSION}")

include_directories (${PROJECT_SOURCE_DIR}/src/)
include_directories (${PROJECT_BINARY_DIR}/src/)


if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  SET(ONION_USE_PAM false)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

SET(LIBPATH /usr/lib /usr/local/lib)

IF (NOT DEFINED CMAKE_INSTALL_BINDIR)
SET(CMAKE_INSTALL_BINDIR bin)
ENDIF (NOT DEFINED CMAKE_INSTALL_BINDIR)

IF (NOT DEFINED CMAKE_INSTALL_LIBDIR)
SET(CMAKE_INSTALL_LIBDIR lib)
ENDIF (NOT DEFINED CMAKE_INSTALL_LIBDIR)

IF (NOT DEFINED CMAKE_INSTALL_MANDIR)
SET(CMAKE_INSTALL_MANDIR share/man/man1/)
ENDIF (NOT DEFINED CMAKE_INSTALL_MANDIR)

IF (NOT DEFINED INCLUDE_PATH)
SET(INCLUDE_PATH /usr/include)
ENDIF (NOT DEFINED INCLUDE_PATH)


IF (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
SET(CMAKE_INSTALL_INCLUDEDIR include/onion)
ENDIF(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)

if (${ONION_POLLER} STREQUAL "default")
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set (ONION_POLLER "epoll")
	else(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set (ONION_POLLER "libev")
	endif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
endif (${ONION_POLLER} STREQUAL "default")

message(STATUS "Using ${ONION_POLLER} as poller")

if (${ONION_USE_SSL})
	find_package(GnuTLS)
	find_package(GCrypt)
	if (GNUTLS_FOUND AND GCRYPT_FOUND)
		set(GNUTLS_ENABLED true)
		message(STATUS "SSL support is compiled in.")
	else(GNUTLS_FOUND AND GCRYPT_FOUND)
		message("Gnutls or gcrypt not found. SSL support is not compiled in.")
	endif(GNUTLS_FOUND AND GCRYPT_FOUND)
endif(${ONION_USE_SSL})

if (${ONION_USE_SQLITE3})
    find_package(SQLite3)
	if (SQLITE3_FOUND)
		set(SQLITE3_ENABLED true)
		message(STATUS "SQLite session support is compiled in.")
	else(SQLITE3_FOUND)
		message("SQLite session support is not compiled in.")
	endif(SQLITE3_FOUND)
endif(${ONION_USE_SQLITE3})

if (${ONION_USE_REDIS})
    find_package(HiRedis)
	if(HIREDIS_FOUND)
		set(REDIS_ENABLED true)
		message(STATUS "Redis session support is compiled in.")
	else(HIREDIS_FOUND)
		message("Hiredis not found. Redis session support is not compiled in.")
	endif(HIREDIS_FOUND)
endif(${ONION_USE_REDIS})

if (${ONION_USE_PTHREADS})
	find_library(PTHREADS_LIB NAMES pthread PATH ${LIBPATH})
	if(PTHREADS_LIB)
		message(STATUS "pthreads found. Threading support is compiled in.")
		set(PTHREADS true)
	else(PTHREADS_LIB)
		message("pthreads not found. Threading is not supported.")
	endif(PTHREADS_LIB)
endif(${ONION_USE_PTHREADS})

if (${ONION_USE_XML2})
	find_package(LibXml2)
	if(LIBXML2_FOUND)
		message(STATUS "WebDAV support is compiled in.")
		set(XML2_ENABLED true)
	else(LIBXML2_FOUND)
		message("libxml2 not found. WebDAV is not supported.")
	endif(LIBXML2_FOUND)
endif(${ONION_USE_XML2})

if (${ONION_USE_PAM})
	find_package(PAM)
	if (PAM_FOUND)
		set(PAM_ENABLED true)
		message(STATUS "PAM support is compiled in handlers.")
	else (PAM_FOUND)
		message("No PAM support.")
  endif (PAM_FOUND)
endif (${ONION_USE_PAM})

if (${ONION_USE_PNG})
	find_package(PNG)
	if (PNG_FOUND)
		set(PNG_ENABLED true)
		message(STATUS "libpng found. png support is compiled in at extras.")
		find_package(Cairo)
		if (CAIRO_FOUND)
			set(CAIRO_ENABLED true)
			message(STATUS "Adding cairo examples.")
		else (CAIRO_FOUND)
			message(WARNING "Some examples are disabled as you dont have cairo: cairo")
		endif (CAIRO_FOUND)
	else (PNG_LIB)
		message("libpng not found. No png support.")
  endif (PNG_FOUND)
endif (${ONION_USE_PNG})

if (${ONION_USE_JPEG})
	find_package(JPEG)
	if (JPEG_FOUND)
		set(JPEG_ENABLED true)
		message(STATUS "jpeg support is compiled in at extras.")
	else (JPEG_FOUND)
		message("libjpeg not found. No jpeg support.")
  endif (JPEG_FOUND)
endif (${ONION_USE_JPEG})

if (${ONION_USE_GC})
	find_package(BoehmGC)
	if (BOEHM_GC_FOUND)
		message(STATUS "compiling Boehm GC examples")
		set(GC_ENABLED true)
	else (BOEHM_GC_FOUND)
		message(STATUS "NOT compiling Boehm GC examples")
	endif (BOEHM_GC_FOUND)
endif (${ONION_USE_GC})


find_package(CURL)
if(CURL_FOUND)
	message(STATUS "Some extra test are compiled.")
	set(CURL true)
else(CURL_FOUND)
	message("curl not found. Some examples wil not be built.")
endif(CURL_FOUND)

if (${ONION_USE_SYSTEMD})
    find_package(Systemd)
	if (SYSTEMD_FOUND)
		set(SYSTEMD_ENABLED true)
		message(STATUS "Systemd support compiled in.")
	else(SYSTEMD_FOUND)
		message("Systemd support is not compiled in. Install libsystemd-dev | systemd-devel")
	endif(SYSTEMD_FOUND)
endif(${ONION_USE_SYSTEMD})

FIND_PROGRAM(MD2MAN md2man-roff)
if(MD2MAN)
  message(STATUS "md2man-roff found, compiling manpages")
else(MD2MAN)
  message("md2man-roff NOT found, NOT compiling manpages. Install it via 'gem install md2man'")
endif(MD2MAN)


# defines
if (GNUTLS_ENABLED)
	add_definitions(-DHAVE_GNUTLS)
	add_definitions(${GNUTLS_DEFINITIONS})
endif(GNUTLS_ENABLED)
if (PTHREADS)
	add_definitions(-DHAVE_PTHREADS)
endif(PTHREADS)
if (PAM_ENABLED)
	add_definitions(-DHAVE_PAM)
endif(PAM_ENABLED)
if (XML2_ENABLED)
	add_definitions(-DHAVE_WEBDAV)
	add_definitions(${LIBXML2_DEFINITIONS})
endif(XML2_ENABLED)
if (SYSTEMD_ENABLED)
	add_definitions(-DHAVE_SYSTEMD)
	add_definitions(${SYSTEMD_DEFINITIONS})
endif (SYSTEMD_ENABLED)
if (SQLITE3_ENABLED)
	add_definitions(-DHAVE_SQLITE3)
	add_definitions(${SQLITE3_DEFINITIONS})
endif (SQLITE3_ENABLED)
if (REDIS_ENABLED)
	add_definitions(-DHAVE_REDIS)
endif (REDIS_ENABLED)
add_definitions(-D_BSD_SOURCE)
add_definitions(-D_DEFAULT_SOURCE)
add_definitions(-D_POSIX_C_SOURCE=200112L)
add_definitions(-D_ISOC99_SOURCE)
if(PNG_ENABLED)
	add_definitions(-DPNG_ENABLED)
	add_definitions(${PNG_DEFINITIONS})
endif(PNG_ENABLED)
if(JPEG_ENABLED)
	add_definitions(-DJPEG_ENABLED)
endif(JPEG_ENABLED)


add_definitions(-DONION_VERSION="${ONION_VERSION}")

# pkg-config
include(GNUInstallDirs)
configure_file("onion.pc.in" "onion.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/onion.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")


##
IF (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-D__DEBUG__)
	SET(CMAKE_C_FLAGS "-Wall -O0 -fPIC ${CMAKE_C_FLAGS}")
	SET(CMAKE_CXX_FLAGS "-Wall -O0 -fPIC ${CMAKE_CXX_FLAGS}")
ELSE (${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET(CMAKE_C_FLAGS "-Wall -O2 -fPIC ${CMAKE_C_FLAGS}")
	SET(CMAKE_CXX_FLAGS "-Wall -O2 -fPIC ${CMAKE_CXX_FLAGS}")
ENDIF (${CMAKE_BUILD_TYPE} MATCHES "Debug")


project (onion)

INCLUDE(CPackConfig.cmake)
INCLUDE(CPack)

add_subdirectory(src)
add_subdirectory(tools)

SET(OTEMPLATE otemplate)
SET(OPACK opack)

if (${ONION_EXAMPLES})
	add_subdirectory(examples)
endif (${ONION_EXAMPLES})

if (${ONION_USE_TESTS})
	enable_testing()
	add_subdirectory(tests)
endif(${ONION_USE_TESTS})

add_subdirectory(manpages)

file(GLOB_RECURSE ALL_C_H_FILES RELATIVE ${CMAKE_SOURCE_DIR}
	"src/*.c" "src/*.h" "src/*.cpp" "src/*.hpp"
	"examples/*.c" "examples/*.h"
	"tools/*.c" "tools/*.h"
)
add_custom_target(indent
	COMMAND indent -linux -i2 -nut -brf ${ALL_C_H_FILES}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
