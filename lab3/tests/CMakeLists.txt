find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Core)

add_executable(ProductCardStoreTests
    tst_productcardstore.h
    tst_productcardstore.cpp
    ../productcard.h
    ../productcard.cpp
    ../productcardstore.h
    ../productcardstore.cpp
)

target_include_directories(ProductCardStoreTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(ProductCardStoreTests PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core)

add_test(NAME ProductCardStoreTests COMMAND ProductCardStoreTests)
