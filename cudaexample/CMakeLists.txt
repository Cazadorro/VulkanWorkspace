cmake_minimum_required(VERSION 3.20)
project(CudaExample LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_executable(cudaaddexample
        cudaexample.cu radixsort.cu radixsort.cuh)
target_include_directories(cudaaddexample
        PRIVATE
        ./
        )
#target_link_libraries(AddExample
#        PRIVATE
#        )


target_compile_options(cudaaddexample PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -g>)

set_target_properties(cudaaddexample PROPERTIES CUDA_ARCHITECTURES "89")
