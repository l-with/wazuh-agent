find_package(GTest CONFIG REQUIRED)

add_executable(agent_test agent_test.cpp)
target_link_libraries(agent_test PRIVATE Agent GTest::gtest)
add_test(NAME AgentTest COMMAND agent_test)

add_executable(task_manager_test task_manager_test.cpp)
target_link_libraries(task_manager_test PRIVATE Agent GTest::gtest)
add_test(NAME TaskManagerTest COMMAND task_manager_test)

add_executable(register_test register_test.cpp)
target_link_libraries(register_test PRIVATE Agent GTest::gtest)
add_test(NAME RegisterTest COMMAND register_test)

add_executable(signal_handler_test signal_handler_test.cpp)
target_link_libraries(signal_handler_test PRIVATE Agent GTest::gtest)
add_test(NAME SignalHandlerTest COMMAND signal_handler_test)
