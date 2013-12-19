cmake_minimum_required (VERSION 2.8)
project (3dchess)

option(TEST "If on tests for the project are built." ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

if (MSVC)
    # Add windows target version (Windows XP)
    add_definitions(-D_WIN32_WINNT=0x0501 -DNOMINMAX)

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
endif()

# SDL 2.0 doesn't seem to be widespread under Linux hence build
# it in-tree. Build only what we need though.
set(SDL_SHARED OFF CACHE BOOL "Build SDL2 with shared library support")
set(DIRECTX OFF CACHE BOOL "Build SDL2 with DirectX support")

find_package(OpenGL REQUIRED)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)

find_package(Boost 1.54 REQUIRED COMPONENTS log filesystem date_time thread system program_options serialization)

add_subdirectory(3rdparty/SDL-2.0.1-7804)
add_subdirectory(3rdparty/SOIL)
add_subdirectory(3rdparty/freetype2)

set(ASSIMP_BUILD_STATIC_LIB        ON  CACHE BOOL "Build as static lib")
set(ASSIMP_BUILD_ASSIMP_TOOLS      OFF CACHE BOOL "Don't build tools")

add_subdirectory(3rdparty/assimp)

# Contains GUI related sources and headers
set(GUI_SOURCES
    src/gui/AnimationHelper.h
    src/gui/GuiWindow.h
    src/gui/Menu2D.h
    src/gui/Menu2DItem.h
    src/gui/StateMachine.h
    src/gui/FreeType.h
    src/gui/Mesh.h
    src/gui/AssimpHelper.h
    src/gui/ChessSet.h
    src/gui/Model.h
    src/gui/ObjectHelper.h
	src/gui/GuiObserver.h

    src/gui/AnimationHelper.cpp
    src/gui/GuiWindow.cpp
    src/gui/Menu2D.cpp
    src/gui/Menu2DItem.cpp
    src/gui/StateMachine.cpp
    src/gui/FreeType.cpp
    src/gui/Mesh.cpp
    src/gui/AssimpHelper.cpp
    src/gui/ChessSet.cpp
    src/gui/Model.cpp
    src/gui/ObjectHelper.cpp
	src/gui/GuiObserver.cpp
)

source_group(gui FILES ${GUI_SOURCES})

# Interfaces for the gui
set(GUI_INTERFACE_SOURCES
    src/gui/interface/AbstractState.h
)

source_group(gui\\interface FILES ${GUI_INTERFACE_SOURCES})

# States for the gui
set(GUI_STATE_SOURCES
    src/gui/states/MenuMain.h
    src/gui/states/MenuLoadGame.h
    src/gui/states/MenuOptions.h
    src/gui/states/GamePlay.h

    src/gui/states/MenuMain.cpp
    src/gui/states/MenuLoadGame.cpp
    src/gui/states/MenuOptions.cpp
    src/gui/states/GamePlay.cpp
)

source_group(gui\\states FILES ${GUI_STATE_SOURCES})

# Contains various helpers and other small things
set(MISC_SOURCES
    src/misc/ConsolePlayer.h
    src/misc/ConsolePlayer.cpp

    src/misc/ConsoleObserver.h
    src/misc/ConsoleObserver.cpp

    src/misc/DummyPlayer.h

    src/misc/helper.h
)

source_group(misc FILES ${MISC_SOURCES})

# Game logic sources and headers
set(LOGIC_SOURCES
    src/logic/GameLogic.h
    src/logic/GameLogic.cpp

    src/logic/ChessTypes.h
    src/logic/ChessTypes.cpp
    src/logic/GameState.h
    src/logic/GameState.cpp
    src/logic/ChessBoard.h
    src/logic/ChessBoard.cpp
    src/logic/TurnGenerator.h
    src/logic/TurnGenerator.cpp
    src/logic/Turn.h
    src/logic/Turn.cpp
    src/logic/Evaluators.h
    src/logic/Evaluators.cpp
)

source_group(logic FILES ${LOGIC_SOURCES})

# Interfaces for the game logic
set(LOGIC_INTERFACE_SOURCES
    src/logic/interface/AbstractGameObserver.h
    src/logic/interface/AbstractPlayer.h
    src/logic/interface/AbstractGameLogic.h
    src/logic/interface/AbstractTurnGenerator.h
    src/logic/interface/AbstractEvaluator.h
)

source_group(logic\\interface FILES ${LOGIC_INTERFACE_SOURCES})

# Threaded related tooling for game logic
set(LOGIC_THREADING_SOURCES
    src/logic/threading/ServiceDispatcherThread.h
    src/logic/threading/ServiceDispatcher.h
    src/logic/threading/ObserverDispatcherProxy.h
    src/logic/threading/PlayerDispatcherProxy.h
)

source_group(logic\\threading FILES ${LOGIC_THREADING_SOURCES})

# Core functionality outside of the three big classes
set(CORE_SOURCES
    src/core/GameConfiguration.h
    src/core/GameConfiguration.cpp
)

source_group(core FILES ${CORE_SOURCES})

# Artificial intelligence headers and sources
set(AI_SOURCES
    src/ai/AIPlayer.h
    src/ai/AIPlayer.cpp
    src/ai/Negamax.h
    src/ai/Negamax.cpp
)

source_group(ai FILES ${AI_SOURCES})

# Sources in the 3dchess project
set(EVERYTHINGBUTGUI_SOURCES
    ${MISC_SOURCES}
    ${LOGIC_SOURCES}
    ${LOGIC_INTERFACE_SOURCES}
    ${LOGIC_THREADING_SOURCES}
    ${CORE_SOURCES}
    ${AI_SOURCES}
)

set(SOURCES
    ${GUI_SOURCES}
    ${GUI_INTERFACE_SOURCES}
    ${GUI_STATE_SOURCES}
    ${GUI_FIGURE_SOURCES}
    ${EVERYTHINGBUTGUI_SOURCES}
)

# Library dependencies
set(EVERYTHINGBUTGUI_LIBRARIES
    ${Boost_LIBRARIES}
)

set(LIBRARIES
    SDL2-static
    ${OPENGL_LIBRARIES}
    SOIL
    freetype
    assimp
    ${EVERYTHINGBUTGUI_LIBRARIES}
)

# 3rd-Party headers
include_directories(SYSTEM
    ${SDL2_SOURCE_DIR}/include
    ${OpenGL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${SOIL_SOURCE_DIR}/include
    ${freetype_SOURCE_DIR}/include
    ${assimp_SOURCE_DIR}/include
)

include_directories(
    src
)

# copy resources folders
file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Enable C++11 and stricter warning handling. Also enable debug symbols.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -pedantic -Werror -Wall -g")
elseif(MSVC)
    # Enable warnings as errors on MSVC.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
endif()

add_executable(3dchess src/3dchess.cpp ${SOURCES})
target_link_libraries(3dchess SDL2main ${LIBRARIES})

if (TEST)

    # Consolechess binary
    set(CONSOLECHESS_SOURCES
        test/other/consolechess.cpp
    )

    add_executable(consolechess ${CONSOLECHESS_SOURCES} ${EVERYTHINGBUTGUI_SOURCES})
    target_link_libraries(consolechess ${EVERYTHINGBUTGUI_LIBRARIES})


    # The officially recommended way of integrating these is to compile
    # them with your project instead of relying on them being available
    # externally so do that
    add_subdirectory(3rdparty/gmock-1.7.0)

    enable_testing()

    include_directories(SYSTEM
                        ${gtest_SOURCE_DIR}/include
                        ${gmock_SOURCE_DIR}/include)

    #
    # MISC tests for the odd helper things in the project
    #
    set(MISC_TEST_SOURCES
        test/misc_test.cpp
    )
    source_group(test FILES ${MISC_TEST_SOURCES})

    add_executable(misc_test ${MISC_TEST_SOURCES} ${EVERYTHINGBUTGUI_SOURCES})
    target_link_libraries(misc_test gmock_main ${EVERYTHINGBUTGUI_LIBRARIES})
    add_test(NAME Misc-Testset COMMAND misc_test)

    #
    # AI tests for testing the AI algorithms
    #
    set(AI_TEST_SOURCES
        test/ai/AIPlayer_test.cpp
        test/ai/Negamax_test.cpp
    )

    add_executable(ai_test ${AI_TEST_SOURCES} ${EVERYTHINGBUTGUI_SOURCES})
    target_link_libraries(ai_test gmock_main ${EVERYTHINGBUTGUI_LIBRARIES})
    add_test(NAME AI-Testset COMMAND ai_test)

    #
    # LOGIC tests for testing game logic, turn generation etc.
    #
    set(LOGIC_TEST_SOURCES
        test/logic/GameState_test.cpp
        test/logic/ChessBoard_test.cpp
        test/logic/TurnGenerator_test.cpp
    )

    add_executable(logic_test ${LOGIC_TEST_SOURCES} ${EVERYTHINGBUTGUI_SOURCES})
    target_link_libraries(logic_test gmock_main ${EVERYTHINGBUTGUI_LIBRARIES})
    add_test(NAME Logic-Testset COMMAND logic_test)

    #
    # INTEGRATION tests for testing several components in conjunction
    #
    set(INTEGRATION_TEST_SOURCES
        test/integration/Gen_Alg_Eval_test.cpp
    )

    add_executable(integration_test ${INTEGRATION_TEST_SOURCES} ${EVERYTHINGBUTGUI_SOURCES})
    target_link_libraries(integration_test gmock_main ${EVERYTHINGBUTGUI_LIBRARIES})
    add_test(NAME Integration_test-Testset COMMAND integration_test)

endif()
