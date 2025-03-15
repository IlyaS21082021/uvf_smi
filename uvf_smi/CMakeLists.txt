project(uvf_smi LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(${CMAKE_PROJECT_NAME} $ENV{CUDA_DIR}/targets/x86_64-linux/include
					  ../../gpu_drivers/nvidia/include)
					  
add_executable(${CMAKE_PROJECT_NAME} main.cpp uvf_smi.cpp)

target_link_libraries(uvf_smi nvidia-ml dl stdc++fs)
