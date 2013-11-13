cmake_minimum_required (VERSION 2.6)
project (3dchess)

option(TEST "If on tests for the project are built." ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

if (MSVC)
    # Add windows target version (Windows XP)
    add_definitions(-D_WIN32_WINNT=0x0501)
    
    # Ensure we use the multithreaded runtime
    # http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
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
endif()

# SDL 2.0 doesn't seem to be widespread under Linux hence build
# it in-tree. Build only what we need though.
set(SDL_SHARED OFF CACHE BOOL "Build SDL2 with shared library support")
set(DIRECTX OFF CACHE BOOL "Build SDL2 with DirectX support")

add_subdirectory(3rdparty/SDL-2.0.1-7804)
add_subdirectory(3rdparty/SOIL)
add_subdirectory(3rdparty/freetype2)

find_package(OpenGL REQUIRED)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON) 

find_package(Boost 1.54 REQUIRED COMPONENTS log filesystem date_time thread system program_options serialization)


# Sources in the 3dchess project
set(SOURCES
    src/ConsolePlayer.cpp
    src/GameLogic.cpp
    src/chesstypes.cpp
    src/ConsolePlayer.cpp
    src/ConsoleObserver.cpp
    src/GameConfiguration.cpp
    src/AnimationHelper.cpp
    src/GuiWindow.cpp
    src/Menu2D.cpp
    src/Menu2DItem.cpp
)

# Headers in the 3dchess project
set(HEADERS
    src/helper.h
    src/chesstypes.h
    src/AbstractGameObserver.h
    src/AbstractPlayer.h
    src/AbstractGameLogic.h
    src/ConsolePlayer.h
    src/GameLogic.h
    src/DummyPlayer.h
    src/ConsolePlayer.h
    src/ConsoleObserver.h
    src/GameConfiguration.h
    src/ServiceDispatcherThread.h
    src/ServiceDispatcher.h
    src/PlayerDispatcherProxy.h
    src/AnimationHelper.h
    src/GuiWindow.h
    src/Menu2D.h
    src/Menu2DItem.h
)

# Library dependencies
set(LIBRARIES
    SDL2-static
    ${OPENGL_LIBRARIES}
    ${Boost_LIBRARIES}
    SOIL
    freetype
)

# 3rd-Party headers
include_directories(SYSTEM
    ${SDL2_SOURCE_DIR}/include
    ${OpenGL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${SOIL_SOURCE_DIR}/include
    ${freetype_SOURCE_DIR}/include
)

include_directories(
    src
)

link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

add_executable(3dchess src/3dchess.cpp ${SOURCES} ${HEADERS})
target_link_libraries(3dchess SDL2main ${LIBRARIES})

if (TEST)
    # The officially recommended way of integrating these is to compile
    # them with your project instead of relying on them being available
    # externally so do that
    add_subdirectory(3rdparty/gmock-1.7.0)

    enable_testing()

    include_directories(SYSTEM
                        ${gtest_SOURCE_DIR}/include
                        ${gmock_SOURCE_DIR}/include)

    add_executable(ai_test test/ai_test.cpp ${SOURCES} ${HEADERS})
    target_link_libraries(ai_test gmock_main ${LIBRARIES})
    add_test(NAME AI-Testset COMMAND ai_test)


    add_executable(misc_test test/misc_test.cpp ${SOURCES} ${HEADERS})
    target_link_libraries(misc_test gmock_main ${LIBRARIES})
    add_test(NAME Misc-Testset COMMAND misc_test)


    add_executable(logic_test test/logic_test.cpp ${SOURCES} ${HEADERS})
    target_link_libraries(logic_test gmock_main ${LIBRARIES})
    add_test(NAME Logic-Testset COMMAND logic_test)

endif()
