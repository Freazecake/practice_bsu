cmake_minimum_required(VERSION 3.16)
project(QtArkanoid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- Игровая логика + UI, общая библиотека, переиспользуемая тестами ---
add_library(arkanoid_core STATIC
    src/GameEngine.cpp
    src/GameEngine.h
    src/GameObject.h
    src/Ball.h
    src/Paddle.h
    src/Brick.h
    src/PowerUp.h
    src/Vector2D.h
)
target_include_directories(arkanoid_core PUBLIC src)
target_link_libraries(arkanoid_core PUBLIC Qt6::Widgets)

# --- Исполняемое приложение ---
add_executable(QtArkanoid
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/GameWidget.cpp
    src/GameWidget.h
)
target_link_libraries(QtArkanoid PRIVATE arkanoid_core Qt6::Widgets)

# --- Юнит-тесты ---
enable_testing()
add_subdirectory(tests)

# --- Служебная утилита для скриншотов (не часть сдаваемого приложения) ---
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/screenshot_capture.cpp")
    add_executable(screenshot_capture
        tools/screenshot_capture.cpp
        src/MainWindow.cpp
        src/MainWindow.h
        src/GameWidget.cpp
        src/GameWidget.h
    )
    target_link_libraries(screenshot_capture PRIVATE arkanoid_core Qt6::Widgets)
endif()
