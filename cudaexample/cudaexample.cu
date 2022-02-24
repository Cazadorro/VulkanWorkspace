#include <iostream>
#include <vector>

// function to add the elements of two arrays
__global__
void addConstant(float * matrix, unsigned width, unsigned height, float addend) {
    unsigned y_idx = (blockIdx.y * blockDim.y) + threadIdx.y;
    unsigned x_idx = (blockIdx.x * blockDim.x) + threadIdx.x;
    if(y_idx >= height || x_idx >= width){
        return;
    }
    unsigned unique_idx = (y_idx * width + x_idx);
    matrix[unique_idx] += addend;
}

int main(void){
    std::uint32_t width = 64;
    std::uint32_t height = 64;

    std::vector<float> host_matrix(width*height, 1.0);
    float* device_matrix;
    cudaMalloc(&device_matrix, host_matrix.size() * sizeof(float));
    cudaMemcpy(device_matrix, host_matrix.data(), host_matrix.size() * sizeof(float), cudaMemcpyHostToDevice);

    dim3 block_size {32,32,1};
    dim3 grid_size{static_cast<std::uint32_t>(std::ceil(
            width / float(block_size.x))),
                   static_cast<std::uint32_t>(std::ceil(
                           height / float(block_size.y))),1};
    addConstant<<<grid_size, block_size>>>(device_matrix, 64, 64, 10.f);

    cudaDeviceSynchronize();

    cudaMemcpy(host_matrix.data(), device_matrix, host_matrix.size() * sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(device_matrix);

    std::cout << "matrix size = " << host_matrix.size() << std::endl;
    for (const auto& value : host_matrix) {
        std::cout << value << std::endl;
    }
    return 0;
}