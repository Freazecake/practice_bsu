cmake_minimum_required(VERSION 3.16)
project(VectorEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(VectorEditor
    main.cpp
    mainwindow.h
    mainwindow.cpp
    canvas.h
    canvas.cpp
    scene.h
    scene.cpp
    shape.h
    shape.cpp
    shapes.h
    shapes.cpp
)

target_link_libraries(VectorEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

enable_testing()
add_subdirectory(tests)
