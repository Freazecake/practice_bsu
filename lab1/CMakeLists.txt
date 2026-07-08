cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

set(APP_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/Fraction.cpp
        src/Fraction.h
        src/laba10.h
        src/laba7.h
)

set(CORE_SOURCES
        src/mainwindow.cpp
        src/mainwindow.h
        src/Fraction.cpp
        src/Fraction.h
        src/laba10.h
        src/laba7.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(untitled
        MANUAL_FINALIZATION
        ${APP_SOURCES}
    )
else()
    if(ANDROID)
        add_library(untitled SHARED ${APP_SOURCES})
    else()
        add_executable(untitled ${APP_SOURCES})
    endif()
endif()

target_link_libraries(untitled PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(untitled PRIVATE src)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.untitled)
endif()
set_target_properties(untitled PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS untitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(untitled)
endif()

enable_testing()

function(add_qt_test TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE} ${CORE_SOURCES})
    target_link_libraries(${TEST_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test)
    target_include_directories(${TEST_NAME} PRIVATE src)
    
    set_target_properties(${TEST_NAME} PROPERTIES WIN32_EXECUTABLE FALSE)
    
    if(MINGW)
        target_link_options(${TEST_NAME} PRIVATE "-mconsole")
    endif()
    
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_qt_test(check_lab1 tests/test_lab1.cpp)
add_qt_test(check_lab2 tests/test_lab2.cpp)
add_qt_test(check_lab3 tests/test_lab3.cpp)