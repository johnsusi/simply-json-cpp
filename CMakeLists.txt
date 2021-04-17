cmake_minimum_required(VERSION 3.14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(simply-json
  VERSION 0.99.1
  LANGUAGES CXX)

option(DISABLE_TESTING    "Disable building tests" OFF)
option(DISABLE_INSTALLING "Disable installing"     OFF)
option(DISABLE_PACKAGING  "Disable packaging"      OFF)

add_library(${PROJECT_NAME} src/json.cpp src/json.h)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_STANDARD              17
    CXX_STANDARD_REQUIRED     YES
    CXX_EXTENSIONS            OFF
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

IF (NOT DISABLE_INSTALLING)
  include(EnableInstalling)
endif()

if (NOT DISABLE_TESTING)
  include(EnableTesting)
endif()

if (NOT DISABLE_PACKAGING)
  include(EnablePackaging)
endif()


add_executable(test-runner src/test-runner.cpp)
target_link_libraries(test-runner PRIVATE ${PROJECT_NAME})
set_target_properties(test-runner
  PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        OFF
)