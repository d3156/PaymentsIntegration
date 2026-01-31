cmake_minimum_required(VERSION 3.16)
project(PaymentsModel VERSION 1.1.0 LANGUAGES CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/workspace.cmake")
create_target(model)

dependency("../../EasyHttpLib" EasyHttpLib)
