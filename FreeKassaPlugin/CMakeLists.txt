cmake_minimum_required(VERSION 3.16)
project(FreeKassaPlugin VERSION 1.0.1 LANGUAGES CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/workspace.cmake")
create_target(plugin)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/status/Page.html PAGE_HTML)
string(REPLACE "\\" "\\\\" PAGE_HTML "${PAGE_HTML}")
string(REPLACE "\"" "\\\"" PAGE_HTML "${PAGE_HTML}")
string(REPLACE "\n" "\\n\"\n\"" PAGE_HTML "${PAGE_HTML}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/status/embedded_page.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/embedded_page.cpp @ONLY)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/embedded_page.cpp)

dependency("../PaymentsModel" PaymentsModel)
dependency("../../MetricsModel" MetricsModel)
dependency("../../EasyHttpLib" EasyHttpLib)

find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
target_link_libraries(FreeKassaPlugin PUBLIC OpenSSL::SSL OpenSSL::Crypto)

find_package(Boost 1.8 REQUIRED COMPONENTS json thread system)
target_link_libraries(FreeKassaPlugin PRIVATE Boost::system Boost::thread Boost::json)