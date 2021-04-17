enable_testing()

file(GLOB TESTS "${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cpp")
add_executable(unit-tests src/test.cpp ${TESTS})
target_link_libraries(unit-tests PRIVATE ${PROJECT_NAME})
set_target_properties(unit-tests
  PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        OFF
)

include(CTest)
include(Catch)
catch_discover_tests(unit-tests)
