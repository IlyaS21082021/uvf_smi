project(uvf_smi_server LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_executable(uvf_smi_server main.cpp uvf_smi_server.cpp)

target_link_libraries(uvf_smi_server pthread)
