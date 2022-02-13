cmake_minimum_required(VERSION 3.10.2)

project(tennicam_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)


################
# Dependencies #
################

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(mpi_cmake_modules REQUIRED)
find_package(pybind11 REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(shared_memory REQUIRED)
find_package(real_time_tools REQUIRED)
find_package(o80 REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(ZeroMQPP REQUIRED)
find_package(json_helper REQUIRED)
find_package(signal_handler REQUIRED)
ament_export_dependencies(
  mpi_cmake_modules
  pybind11
  shared_memory
  real_time_tools
  json_helper
  signal_handler
  o80)

# prepare to export all needed targets
set(all_targets)
set(all_target_exports)


###########
# Library #
###########

add_library( ${PROJECT_NAME} SHARED
  src/ball.cpp
  src/transform.cpp
  src/driver.cpp
  src/driver_config.cpp
  src/dummy_server.cpp
  src/standalone.cpp)
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${PC_LIBZMQ_INCLUDE_DIRS})
ament_target_dependencies(${PROJECT_NAME}
  shared_memory
  o80)
target_link_libraries(${PROJECT_NAME} shared_memory::shared_memory)
target_link_libraries(${PROJECT_NAME} real_time_tools::real_time_tools)
target_link_libraries(${PROJECT_NAME} o80::o80)
target_link_libraries(${PROJECT_NAME} json_helper::json_helper)
target_link_libraries(${PROJECT_NAME} ${ZeroMQ_LIBRARY})
target_link_libraries(${PROJECT_NAME} ZeroMQPP::ZeroMQPP)
ament_export_interfaces(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
list(APPEND all_targets ${PROJECT_NAME})
list(APPEND all_target_exports export_${PROJECT_NAME})


####################
# Executable (C++) #
####################

add_executable(tennicam_client_dummy_server src/run_dummy_server.cpp)
set(all_targets ${all_targets} tennicam_client_dummy_server)
target_include_directories(
  tennicam_client_dummy_server
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(tennicam_client_dummy_server ${PROJECT_NAME})
target_link_libraries(tennicam_client_dummy_server signal_handler::signal_handler)
       
install(TARGETS tennicam_client_dummy_server RUNTIME DESTINATION bin)


########################
# Executables (python) #
########################

install_scripts(
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_print.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_display.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_display_robot_mirroring.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_transform_update.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_logger.py
  ${CMAKE_CURRENT_LIST_DIR}/bin/tennicam_client_replay.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
  )


#######################
# Configuration files #
#######################

get_filename_component(TENNICAM_CLIENT_CONFIG_DIR config ABSOLUTE)
# installing files in /opt/mpi-is/ if exists
if (EXISTS "/opt/mpi-is/")
  install(DIRECTORY
    "${TENNICAM_CLIENT_CONFIG_DIR}" DESTINATION /opt/mpi-is/tennicam_client)
endif()
# installing in workspace/install
install(DIRECTORY
  "${TENNICAM_CLIENT_CONFIG_DIR}" DESTINATION opt/mpi-is/)


###################
# Python wrappers #
###################

add_library(${PROJECT_NAME}_py MODULE srcpy/wrappers.cpp)
target_link_libraries(${PROJECT_NAME}_py PRIVATE pybind11::module)
target_link_libraries(${PROJECT_NAME}_py PRIVATE ${PYTHON_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_py PRIVATE ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_py
  PROPERTIES PREFIX "" SUFFIX "${PYTHON_MODULE_EXTENSION}"
  OUTPUT_NAME ${PROJECT_NAME}_wrp)
target_include_directories(
  ${PROJECT_NAME}_py
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include> SYSTEM
  PUBLIC ${PYTHON_INCLUDE_DIRS})
_ament_cmake_python_get_python_install_dir()
install(TARGETS ${PROJECT_NAME}_py DESTINATION ${PYTHON_INSTALL_DIR})


######################
# Python Native code #
######################

ament_python_install_package(${PROJECT_NAME} PACKAGE_DIR python/${PROJECT_NAME})


######################
# Install and export #
######################

install(DIRECTORY include/ DESTINATION include)
install(
  TARGETS ${all_targets}
  EXPORT ${all_target_exports}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES
  DESTINATION include)
ament_package()


##############
# Unit tests #
##############

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_${PROJECT_NAME}_cpp tests/main.cpp tests/unit_tests.cpp)
  target_link_libraries(test_${PROJECT_NAME}_cpp ${PROJECT_NAME})
endif()
