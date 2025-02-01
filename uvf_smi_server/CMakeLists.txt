cmake_minimum_required(VERSION 3.5)

project(uvf_smi_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uvf_smi_server main.cpp
    uvf_smi_server.h uvf_smi_server.cpp)

include(GNUInstallDirs)
install(TARGETS uvf_smi_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
