cmake_minimum_required(VERSION 3.1.0)

# This version matches the version of minimap2 used
project(minimap2
    VERSION 2.7
    LANGUAGES C
)

include(FindZLIB)

find_package (Threads)

enable_testing()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_DEBUG_POSTFIX "d")

### set some options for building on ARM, if needed. ##########################
option (ARM_BUILD
  "Build on an ARM processor."
  OFF
  )

SET(ARM_TARGETS "arm;aarch64")
list (FIND ARM_TARGETS ${CMAKE_SYSTEM_PROCESSOR} _index)
if (${_index} GREATER -1)
  set(ARM_BUILD "ON")
  endif()

#################################################################### check out minimap2 project ###

find_package(Git REQUIRED)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/clone)
	exec_program(
	    "${GIT_EXECUTABLE}"
	    ${CMAKE_SOURCE_DIR}
	    ARGS "clone https://github.com/lh3/minimap2.git clone"
	)
endif()

exec_program(
    "${GIT_EXECUTABLE}"
    ${CMAKE_SOURCE_DIR}/clone
    ARGS "checkout v${PROJECT_VERSION}"
)
file(COPY
	clone/minimap.h
	clone/bseq.h
	DESTINATION
	${CMAKE_SOURCE_DIR}
)

################################################################## set import and export folder ###

if(DEFINED ENV{BWA_OS})
    set(IMPORT_FOLDER "${CMAKE_SOURCE_DIR}/import/$ENV{BWA_OS}")
elseif(WIN32)
    set(IMPORT_FOLDER "${CMAKE_SOURCE_DIR}/import/win")
elseif(APPLE)
    set(IMPORT_FOLDER "${CMAKE_SOURCE_DIR}/import/osx")
elseif(ARM_BUILD)
    set(IMPORT_FOLDER "${CMAKE_SOURCE_DIR}/import/arm")
else()
    set(IMPORT_FOLDER "${CMAKE_SOURCE_DIR}/import/manylinux")
endif()
message(STATUS "Import folder is ${IMPORT_FOLDER}")

if (DEFINED ONT_CORE_INSTALL_PATH)
    set(CMAKE_INSTALL_PREFIX ${ONT_CORE_INSTALL_PATH})
else()
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
endif()
message(STATUS "Export folder is ${CMAKE_INSTALL_PREFIX}")

#################################################################### define compilation targets ###

include_directories(${IMPORT_FOLDER}/include ${ZLIB_INCLUDE_DIRS})
link_directories(${IMPORT_FOLDER}/lib)

if(ARM_BUILD)
        include_directories(clone/sse2neon)
endif()

add_library(libminimap2
        clone/getopt.c
        clone/kthread.c
        clone/kalloc.c
        clone/ksw2_extz2_sse.c
        clone/ksw2_extd2_sse.c
        clone/ksw2_exts2_sse.c
        clone/ksw2_ll_sse.c
        clone/misc.c
        clone/bseq.c
        clone/sketch.c
        clone/sdust.c
        clone/index.c
        clone/chain.c
        clone/align.c
        clone/hit.c
        clone/map.c
        clone/format.c
        clone/pe.c
        clone/esterr.c
)

add_library(ont_minimap2
	SHARED
		ont_minimap2.c
)

set_target_properties(ont_minimap2
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_executable(minimap2
	clone/main.c
)

# run minimap2 to see if all libraries are found, no functional testing yet
add_test(
    NAME ont_minimap2_test
    CONFIGURATIONS Release Debug RelWithDebInfo
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    COMMAND minimap2 -V
)

########################################################################## compilation settings ###

target_link_libraries(libminimap2
	PRIVATE
		${ZLIB_LIBRARIES}
)

target_link_libraries(minimap2
	PRIVATE
		libminimap2
)

target_link_libraries(ont_minimap2
	PRIVATE
		libminimap2
)

# suppress all warnings (DISABLE THIS DURING DEVELOPMENT!)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -w")

if(WIN32)
	add_definitions(-DHAVE_STRUCT_TIMESPEC)
	add_definitions(-DPTW32_STATIC_LIB)
if (NOT ${CMAKE_C_COMPILER_ID} MATCHES "Intel")
    # Explicitly define __SSE2__ for msvc, as the compiler doesn't define it by default.
	add_definitions(-D__SSE2__)
endif()
	target_link_libraries(libminimap2
		PRIVATE
			debug pthreadVC2d
			optimized pthreadVC2
	)
else()
        if(ARM_BUILD)
                add_definitions(
                          -D__SSE2__
                          -DKSW_SSE2_ONLY
                          -D_FILE_OFFSET_BITS=64
                          -fsigned-char
                )
        endif()
        # The 'm' is not a typo -- we're passing in the linker flag "-lm"
	target_link_libraries(libminimap2
		PRIVATE
                        m
			${CMAKE_THREAD_LIBS_INIT}
	)
endif()

################################################################################ install target ###

file(GLOB IMPORT_HEADER_FILES "${IMPORT_FOLDER}/include/*.h")
file(GLOB IMPORT_LIB_FILES "${IMPORT_FOLDER}/lib/*.dll")

install(
    FILES ont_minimap2.h minimap.h bseq.h ${IMPORT_HEADER_FILES}
    DESTINATION include
)
install(
    FILES ${IMPORT_LIB_FILES}
    DESTINATION lib
)
install(
    TARGETS minimap2 ont_minimap2
	RUNTIME DESTINATION lib
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
