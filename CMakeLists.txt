cmake_minimum_required(VERSION 3.16)
project(PathfindingVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/SFML-3.1.0/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(PathfindingVisualizer
    src/main.cpp
    src/Grid.cpp
    src/UI.cpp
)

add_custom_command(TARGET PathfindingVisualizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:PathfindingVisualizer>/assets
)

target_link_libraries(PathfindingVisualizer
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

if(WIN32)
    add_custom_command(TARGET PathfindingVisualizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/SFML-3.1.0/bin/sfml-graphics-d-3.dll"
            $<TARGET_FILE_DIR:PathfindingVisualizer>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/SFML-3.1.0/bin/sfml-window-d-3.dll"
            $<TARGET_FILE_DIR:PathfindingVisualizer>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/SFML-3.1.0/bin/sfml-system-d-3.dll"
            $<TARGET_FILE_DIR:PathfindingVisualizer>
    )
endif()