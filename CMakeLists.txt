cmake_minimum_required(VERSION 3.16)
project(ConsoleCalculator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(calc
    src/main.cpp
    src/Parser.cpp
    src/PluginManager.cpp
)
target_include_directories(calc PRIVATE include)

if(WIN32)
    target_link_libraries(calc PRIVATE)
else()
    message(FATAL_ERROR "This project uses the Windows DLL API (LoadLibrary/GetProcAddress). Build on Windows.")
endif()

function(add_calc_plugin target source)
    add_library(${target} SHARED ${source})
    target_include_directories(${target} PRIVATE include)
    set_target_properties(${target} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    )
endfunction()

add_calc_plugin(plugin_sin plugins/sin.cpp)
add_calc_plugin(plugin_ln plugins/ln.cpp)
add_calc_plugin(plugin_sqrt plugins/sqrt.cpp)

add_custom_command(TARGET calc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:calc>/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:plugin_sin>" "$<TARGET_FILE_DIR:calc>/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:plugin_ln>" "$<TARGET_FILE_DIR:calc>/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:plugin_sqrt>" "$<TARGET_FILE_DIR:calc>/plugins"
)
