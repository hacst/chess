cmake_minimum_required (VERSION 2.6)
project (3dchess)

option(TEST "If on tests for the project are built." ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

if (MSVC)
    # Ensure we use the multithreaded runtime
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
       if(${flag_var} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
       endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
elseif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# Enable C++11 and stricter warning handling. Also enable debug symbols.
	set(MY_FLAGS "--std=c++11 -pedantic -Werror -Wall -g")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MY_FLAGS}")
endif()

# Sources in the 3dchess project
set(SOURCES
    src/3dchess.cpp
)

# Headers in the 3dchess project
set(HEADERS
)

add_executable(3dchess ${SOURCES} ${HEADERS})

if (TEST)
    # The officially recommended way of integrating these is to compile
    # them with your project instead of relying on them being available
    # externally so do that
    add_subdirectory(3rdparty/gmock-1.7.0)

    enable_testing()

    include_directories(${gtest_SOURCE_DIR}/include)

    add_executable(ai_test test/ai_test.cpp)
    target_link_libraries(ai_test gmock_main)

    add_test(NAME AI-Testset COMMAND ai_test)
endif()
