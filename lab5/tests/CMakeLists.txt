find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(test_gameengine test_gameengine.cpp)
target_link_libraries(test_gameengine PRIVATE arkanoid_core Qt6::Test Qt6::Widgets)

add_test(NAME GameEngineTests COMMAND test_gameengine)
