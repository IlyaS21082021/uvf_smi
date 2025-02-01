cmake_minimum_required(VERSION 3.5)

project(uvf_smi_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(uvf_smi_client main.cpp
    uvf_smi_client.h uvf_smi_client.cpp)

include_directories(uvf_smi_client /usr/local/cuda-11.4/include)

include(GNUInstallDirs)
install(TARGETS uvf_smi_client
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(uvf_smi_client nvidia-ml dl)
