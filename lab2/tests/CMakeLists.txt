find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Gui)

add_executable(WarehouseModelTests
    tst_warehousemodel.h
    tst_warehousemodel.cpp
    ../warehousemodel.h
    ../warehousemodel.cpp
)

target_include_directories(WarehouseModelTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(WarehouseModelTests PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Gui)

add_test(NAME WarehouseModelTests COMMAND WarehouseModelTests)
