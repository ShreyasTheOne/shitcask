 
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)


add_executable(
  shitcask_tests
  test_operations.cpp
)

target_link_libraries(
   shitcask_tests
   lib_shitcask
   GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(shitcask_tests)
