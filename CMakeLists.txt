cmake_minimum_required(VERSION 3.16)

project(mygame)

set(CMAKE_CXX_STANDARD 17)

#! ! ! ! ! ! !
option(PRODUCTION_BUILD "Make this a production build!" OFF)
#! ! ! ! ! ! !

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build SFML static
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SFML as static libraries" FORCE)

# imgui-sfml path
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-docking/imgui")

if(MSVC)
    add_compile_options(/arch:AVX2)
endif()

#Force enable SFML modules
set(SFML_BUILD_AUDIO ON CACHE BOOL "Enable SFML Audio" FORCE)
set(SFML_BUILD_NETWORK ON CACHE BOOL "Enable SFML Network" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "Enable SFML Graphics" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "Enable SFML Window" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "Enable SFML System" FORCE)

# Third party libraries
add_subdirectory(thirdparty/SFML-2.6.1)
add_subdirectory(thirdparty/imgui-docking)
add_subdirectory(thirdparty/imgui-sfml-2.6.x)
add_subdirectory(thirdparty/glad)

# -------------------------------------------------------
# SOURCE FILES
# -------------------------------------------------------

file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine/*.h"
)

file(GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.h"
)

file(GLOB_RECURSE SRC_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

set(ALL_SOURCES
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
    ${SRC_SOURCES}
)

# -------------------------------------------------------
# EXECUTABLE
# -------------------------------------------------------

add_executable(${CMAKE_PROJECT_NAME})

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${ALL_SOURCES})

# -------------------------------------------------------
# RESOURCE PATH
# -------------------------------------------------------

if(PRODUCTION_BUILD)

    target_compile_definitions(${CMAKE_PROJECT_NAME}
        PUBLIC RESOURCES_PATH="./resources/"
    )

    target_compile_definitions(${CMAKE_PROJECT_NAME}
        PUBLIC PRODUCTION_BUILD=1
    )

else()

    target_compile_definitions(${CMAKE_PROJECT_NAME}
        PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
    )

    target_compile_definitions(${CMAKE_PROJECT_NAME}
        PUBLIC PRODUCTION_BUILD=0
    )

endif()

# -------------------------------------------------------
# INCLUDE DIRECTORIES
# -------------------------------------------------------

target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
        ${CMAKE_CURRENT_SOURCE_DIR}/game
)

# -------------------------------------------------------
# MSVC SETTINGS
# -------------------------------------------------------

if(MSVC)

    target_compile_definitions(${CMAKE_PROJECT_NAME}
        PUBLIC _CRT_SECURE_NO_WARNINGS
    )

endif()

# -------------------------------------------------------
# LIBRARIES
# -------------------------------------------------------

target_link_libraries(${CMAKE_PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-network
    sfml-audio
    imgui
    ImGui-SFML::ImGui-SFML
    glad
)