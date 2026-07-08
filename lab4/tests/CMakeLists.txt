find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Gui)

add_executable(VectorEditorTests
    tst_scene.h
    tst_scene.cpp
    ../scene.h
    ../scene.cpp
    ../shape.h
    ../shape.cpp
    ../shapes.h
    ../shapes.cpp
)

target_include_directories(VectorEditorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(VectorEditorTests PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Gui)

add_test(NAME VectorEditorTests COMMAND VectorEditorTests)
