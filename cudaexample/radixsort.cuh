//
// Created by Bolt on 7/12/2023.
//

#ifndef VULKANWORKSPACE_RADIXSORT_CUH
#define VULKANWORKSPACE_RADIXSORT_CUH

#include <cub/agent/single_pass_scan_operators.cuh>
#include <cuda/atomic>
#include <cuda/barrier>
#include <type_traits>
#include <cstdint>
#include <bit>
namespace cuvk {
    //https://stackoverflow.com/a/56932259/
    namespace detail{

        //TODO last std::uint64_t should probably just error, not sure how to deal with that here though.
        template<typename T> struct identity { using type = T; };
    }


    template<std::size_t size>
    using smallest_type_from_size = typename
        std::conditional_t<size <= std::size_t{ std::numeric_limits<std::uint8_t>::max() }, detail::identity<std::uint8_t>,
        std::conditional_t<size <= std::size_t{ std::numeric_limits<std::uint16_t>::max() }, detail::identity<std::uint16_t>,
        std::conditional_t<size <= std::size_t{ std::numeric_limits<std::uint32_t>::max() }, detail::identity<std::uint32_t>,
        std::enable_if<size <= std::size_t{ std::numeric_limits<std::uint64_t>::max() }, std::uint64_t>>>>::type;

    template<typename T, typename input_size_type_t = std::uint32_t>
    class gpu_span {
    public:
        using size_type_t = input_size_type_t;

        __host__
        gpu_span( T *value, size_type_t size) : m_value(value), m_size(size){}

        [[nodiscard]]
        __device__
        T &operator[](size_type_t idx) {
            return m_value[idx];
        }

        [[nodiscard]]
        __device__
        const T &operator[](size_type_t idx) const {
            return m_value[idx];
        }

        [[nodiscard]]
        __host__ __device__
        size_type_t size() const {
            return m_size;
        }

        [[nodiscard]]
        __host__ __device__
        size_type_t empty() const {
            return size() == 0;
        }

    private:
        T *m_value;
        size_type_t m_size;
    };

    template<typename T, std::size_t input_size,  typename input_size_type_t = smallest_type_from_size<input_size * sizeof(T)>>
    class gpu_array_span {
    public:
        using size_type_t = input_size_type_t;
        __host__
        explicit gpu_array_span( T *value) : m_value(value){}
        [[nodiscard]]
        __device__
        T &operator[](size_type_t idx) {
            return m_value[idx];
        }

        [[nodiscard]]
        __device__
        const T &operator[](size_type_t idx) const {
            return m_value[idx];
        }

        [[nodiscard]]
        __host__ __device__
        constexpr size_type_t size() const {
            return m_size;
        }

        [[nodiscard]]
        __host__ __device__
        constexpr size_type_t empty() const {
            return size() == 0;
        }

    private:
        static constexpr size_type_t m_size = input_size;
        T *m_value;
    };



    template<typename T, std::size_t input_size,  typename input_size_type_t = smallest_type_from_size<input_size * sizeof(T)>>
    class kernel_local_array_span {
    public:
        using size_type_t = input_size_type_t;
        __device__
        explicit kernel_local_array_span( T *value) : m_value(value){}
        [[nodiscard]]
        __device__
        T &operator[](size_type_t idx) {
            return m_value[idx];
        }

        [[nodiscard]]
        __device__
        const T &operator[](size_type_t idx) const {
            return m_value[idx];
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t size() const {
            return m_size;
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t empty() const {
            return size() == 0;
        }

    private:
        static constexpr size_type_t m_size = input_size;
        T *m_value;
    };


    template<std::size_t input_width, std::size_t input_height, typename T,
            typename input_size_type_t = smallest_type_from_size<input_width * input_height * sizeof(T)>>
    class kernel_local_array2d_span {
    public:
        using size_type_t = input_size_type_t;
        __device__
        explicit kernel_local_array2d_span( T *value) : m_value(value){}

        __device__
        void fill(T value){
            auto thread_id = threadIdx.x;
            auto assign_index = thread_id;
            while(assign_index < size()){
                m_value[assign_index] = value;
                assign_index += blockDim.x;
            }
            __syncthreads();
        }
        __device__
        void zero(){
            fill(T{0});
        }
        [[nodiscard]]
        __device__
        kernel_local_array_span<T, input_width> operator[](size_type_t idx) {
            return kernel_local_array_span(m_value + input_width * idx);
        }

        [[nodiscard]]
        __device__
        kernel_local_array_span<const T, input_width> operator[](size_type_t idx) const{
            return kernel_local_array_span(m_value + input_width * idx);
        }

        [[nodiscard]]
        __device__  kernel_local_array_span<T, input_width * input_height> linear_view(){
            return kernel_local_array_span(m_value);
        }

        [[nodiscard]]
        __device__  kernel_local_array_span<const T, input_width * input_height> linear_view() const{
            return kernel_local_array_span(m_value);
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t size() const {
            return m_width * m_height;
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t width() const {
            return m_width;
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t height() const {
            return m_height;
        }

        [[nodiscard]]
        __device__
        constexpr size_type_t empty() const {
            return size() == 0;
        }

    private:
        static constexpr size_type_t m_width = input_width;
        static constexpr size_type_t m_height = input_height;
        T *m_value;
    };


    static constexpr std::uint32_t warp_size = 32;
    static constexpr std::uint32_t log_warp_size = std::countr_zero(warp_size);


    //can use width,
    template<typename T, std::uint32_t iterations = log_warp_size>
    //exclusive prefix sum.
    __device__ T hills_steele_warp_prefix_sum(T value, std::uint32_t initial_mask = 0xFFFFFFFF, std::uint32_t width = warp_size){
        //hypothetically should be better than Bleloch, appears to be what CUB uses?
        //upsweep
        std::uint32_t mask = initial_mask;
        for(std::uint32_t d = 0; d < iterations; ++d){
            //have to do it this way when we use warp width, otherwise won't have proper ability
            mask = mask & (mask << (1 << d));
            //shift mask over by 2^d each time,
            //TODO how does the mask work here if width is not the same as warpsize?
            // std::uint32_t mask = initial_mask << (1 << (d));

            //shuffle up looks *backwards* in the warp (shuffling *up* from earlier entries) use 2^D to look for values.
            T warp_value = __shfl_up_sync(mask, value, 1 << (d), width);
            value += warp_value;
        }
        return value;
    }

    //can use width,
    template<typename T, std::uint32_t iterations = log_warp_size>
    //exclusive prefix sum.
    __device__ T hills_steele_warp_inclusive_prefix_sum(T value, std::uint32_t initial_mask = 0xFFFFFFFF, std::uint32_t width = warp_size){
        return hills_steele_warp_prefix_sum<T, iterations>(value, initial_mask, width) + value;
    }
//TODO could make API use actual memory instead, give pointers and ranges, let function figure out how to split up?

//    //could do 8 scan then 4 scan for non power of 2? or just run 2x, make it easier?
//    //can use width,
//    template<typename T, std::uint32_t element_count>
//    //exclusive prefix sum.
//    __device__ T warp_prefix_sum(T value, std::uint32_t mask){
//        //hypothetically should be better than Bleloch, appears to be what CUB uses?
//        //upsweep
//        std::uint32_t mask = initial_mask;
//        for(std::uint32_t d = 0; d < iterations; ++d){
//            //have to do it this way when we use warp width, otherwise won't have proper ability
//            mask = mask & (mask << (1 << d));
//            //shift mask over by 2^d each time,
//            //TODO how does the mask work here if width is not the same as warpsize?
//            // std::uint32_t mask = initial_mask << (1 << (d));
//
//            //shuffle up looks *backwards* in the warp (shuffling *up* from earlier entries) use 2^D to look for values.
//            T warp_value = __shfl_up_sync(mask, value, 1 << (d), width);
//            value += warp_value;
//        }
//        return value;
//    }


    //TODO won't work with floats, only ints for now.
    static constexpr std::uint32_t digit_bit_mask = 0xFF;
    static constexpr std::uint32_t digit_bit_count = 8;
    static constexpr std::uint32_t digit_value_count = 1 << digit_bit_count; //256
    template<typename T, std::uint32_t elements_per_thread, typename size_type_t = std::uint32_t>
    __global__ void radix_sort_histogram_kernel(gpu_span<const T, size_type_t> input,
                                                  gpu_array_span<size_type_t, digit_value_count * sizeof(T)>
                                                  global_histogram_output) {
        static constexpr std::uint32_t digit_place_count = sizeof(T) / digit_bit_count;
        static constexpr std::uint32_t histogram_place_size = digit_value_count;
        // WARNING, can only add up to 2^16
        static_assert(elements_per_thread * 1024 < (1 << 16),
                      "Possible addition count exceeds the maximum possible number of threads per block");
        static_assert(digit_value_count * sizeof(T) < (1 << 16),
                      "can't have more digits than 2^16");
        __shared__ std::uint16_t shared_digit_histogram[digit_value_count * digit_place_count];
        std::uint32_t thread_id = threadIdx.x;
        std::uint32_t block_id = blockIdx.x;

        std::uint32_t threads_per_block = blockDim.x;
        size_type_t block_offset = block_id * elements_per_thread * blockDim.x;
        size_type_t global_item_idx = block_offset + thread_id;

        //add all histograms up
        while (global_item_idx < input.size()) {
            T value = input[global_item_idx];
            for (std::uint32_t digit_place = 0; digit_place < digit_place_count; ++digit_place) {
                std::uint32_t place_offset = digit_place * histogram_place_size;
                //adding to shared memory first for all digit histograms.
                atomicAdd(&shared_digit_histogram[place_offset + (value & digit_bit_mask)], 1);
                value >> digit_bit_count;
            }
            global_item_idx += threads_per_block;
        }
        __syncthreads();
        std::uint32_t histogram_output_idx = thread_id;
        while (histogram_output_idx < global_histogram_output.size()) {
            atomicAdd(global_histogram_output[histogram_output_idx], shared_digit_histogram[histogram_output_idx]);
            histogram_output_idx += threads_per_block;
        }
    }



    //http://people.maths.ox.ac.uk/~gilesm/cuda/lecs/lec4.pdf
    template<typename T, typename size_type_t = std::uint32_t>
    __global__ void radix_sort_exclusive_sum_histogram_kernel(
            gpu_array_span<size_type_t, digit_value_count * sizeof(T)> global_histogram_output) {
        static constexpr std::uint32_t digit_place_count = sizeof(T) / digit_bit_count;
        static constexpr std::uint32_t histogram_place_size = digit_value_count;
        // WARNING, can only add up to 2^16

        constexpr std::uint32_t shared_warp_digit_prefix_sums_size = histogram_place_size / warp_size;
        __shared__ std::uint32_t shared_warp_digit_prefix_sums[shared_warp_digit_prefix_sums_size];
        std::uint32_t thread_id = threadIdx.x;
        std::uint32_t block_id = blockIdx.x;

        std::uint32_t threads_per_block = blockDim.x;

        //TODO histogram_place_size should be same as blockDim.x
        size_type_t block_offset = block_id * blockDim.x;
        size_type_t global_item_idx = block_offset + thread_id;

        //shouldn't be possible for arbitrary size of global histogram, should also be multiple of warp size (256 x number of digit places in T).
        size_type_t histogram_value = global_histogram_output[global_item_idx];

        size_type_t warp_cum_sum = hills_steele_warp_prefix_sum(histogram_value);
        std::uint32_t warp_thread_id = thread_id % warp_size;
        bool is_first_thread_in_warp = warp_thread_id == 0;
        //blockDim.x should be multiple of warp size
        std::uint32_t warp_index = thread_id / warp_size;
        if(is_first_thread_in_warp){
            shared_warp_digit_prefix_sums[warp_index] = warp_cum_sum;
        }
        __syncthreads();
        if(warp_index == 0){
            if(warp_thread_id < shared_warp_digit_prefix_sums_size) {
                std::uint32_t thread_block_warp_sums = shared_warp_digit_prefix_sums[warp_index];
                auto thread_block_cum_sum = hills_steele_warp_prefix_sum<std::uint32_t, 3>(
                        thread_block_warp_sums);
                shared_warp_digit_prefix_sums[warp_thread_id] = thread_block_cum_sum;
            }
        }
        __syncthreads();

        //now that we've gotten the amount in each warp, and prefix summed them in shared memory, we can read it back out
        //and add it to each of the warp cum sums to get the real digit place.
        std::uint32_t digit_place_prefix_sum = warp_cum_sum + shared_warp_digit_prefix_sums[warp_index];
        global_histogram_output[global_item_idx] = digit_place_prefix_sum;
    }

    enum class OffsetFlag : std::uint32_t{
            FlagNotReady       = 0,
            FlagAggregateReady = 0b01000000'00000000'00000000'00000000u,
            FlagPrefixSumReady = 0b10000000'00000000'00000000'00000000u,
    };
    struct FlagOffset{
        std::uint32_t value = 0;
        FlagOffset* set_flag(OffsetFlag flag){
            value &= (static_cast<std::uint32_t>(flag) | ~0b11000000'00000000'00000000'00000000u);
        }
        [[nodiscard]]
        OffsetFlag get_flag() const{
            return static_cast<OffsetFlag>(value & 0b11000000'00000000'00000000'00000000u);
        }
        [[nodiscard]]
        std::uint32_t get_value() const{
            return value & ~0b11000000'00000000'00000000'00000000u;
        }
    };


    template<typename T, std::uint32_t elements_per_thread, typename size_type_t = std::uint32_t>
    __global__ void radix_sort_chain_scan(
            gpu_array_span<const size_type_t, digit_value_count * (sizeof(T)*8) / digit_bit_count> global_digit_place_prefix_sum,
            gpu_span<const T, size_type_t> input,
            gpu_span<T, size_type_t> output,
            std::uint32_t* atomic_next_virtual_processor_id_ptr,
            gpu_span<FlagOffset, size_type_t> global_prefix_offsets,
            std::uint32_t digit_iteration) {
        static constexpr std::uint32_t digit_place_count = (sizeof(T)*8) / digit_bit_count;
        static constexpr std::uint32_t histogram_place_size = digit_value_count;

        constexpr std::uint32_t shared_warp_digit_prefix_sums_size = histogram_place_size / warp_size;

        __shared__ std::uint32_t virtual_processor_id;
        //for 32bit should be 8 * 256. 8 being warp count?
        __shared__ std::uint16_t digit_prefix_sum_offsets[shared_warp_digit_prefix_sums_size * digit_value_count];
        //TODO offset start by 1 in order to avoid bank conflicts?

        //TODO Keep transpose since values are not accessed necessarily regularly anyway by "core" loop
        // but *are* accessed in regular pattern by block prefix sum loop?
        // ( currently doing 256 rows 8 cols instead of 8 rows x 256 cols)
        auto digit_prefix_sum_offsets_view = kernel_local_array2d_span<digit_value_count, shared_warp_digit_prefix_sums_size, std::uint16_t>(digit_prefix_sum_offsets);
        digit_prefix_sum_offsets_view.zero();
        //need shared to coalesce into local bins
        __shared__ T coalesced_output[elements_per_thread * blockDim.x];

        std::uint32_t thread_id = threadIdx.x;
        std::uint32_t block_id = blockIdx.x;

        if(thread_id == 0){
            virtual_processor_id = atomicAdd(atomic_next_virtual_processor_id_ptr, 1);
        }
        size_type_t block_offset = block_id * elements_per_thread * blockDim.x;

        //we have to figure out local offset from shared,
        //then write out our values into shared? then read them out?
        std::uint32_t warp_thread_id = thread_id % warp_size;
        bool is_first_thread_in_warp = warp_thread_id == 0;
        std::uint32_t warp_index = thread_id / warp_size;
        std::uint16_t local_offsets[elements_per_thread];
        T local_values[elements_per_thread];

        for(std::uint32_t local_item_idx = 0; local_item_idx < elements_per_thread; ++local_item_idx){
            std::uint32_t global_item_idx = block_offset + local_item_idx * blockDim.x;
            if(global_item_idx < input.size()){
                break;
            }
            //it's possible for a thread to be outside the extent of the data here,
            // so we stop it from causing undefined behavior here.
            std::uint32_t mask = __activemask();
            //TODO won't work for floats! need to convert to uint32_t for f32, uint64 for f64, and so on
            T value = input[global_item_idx];
            std::uint32_t value_selected_digit = (value >> (digit_bit_count * digit_iteration)) & digit_bit_mask;
            std::uint32_t warp_digit_vote_totals = 0;
            //TODO Do this twice to avoid local offsets needing to exist?
            for(std::uint32_t bit_digit_index = 0; bit_digit_index < digit_bit_count; ++bit_digit_index){
                std::uint32_t checked_bit = (1 << bit_digit_index);
                std::uint32_t ballot_result = __ballot_sync(mask, (value_selected_digit & checked_bit) != 0);
                warp_digit_vote_totals &= ballot_result;
            }
            //warp == 0? 0x0000.... warp = 1? 0x0000...1, warp= 2? 0b0000....11
            //checks all the threads in the warp *before* current index.
            //std::uint32_t warp_before_thread_mask = ~(0xFFFFFFFFU << warp_thread_id);
            //if (warp_digit_vote_totals & warp_vote_before_inclusive_mask) == 0
            //
            std::uint32_t warp_vote_before_inclusive_mask = ~(0xFFFFFFFFU << (warp_thread_id + 1));
            //find the lowest warp id for the current digit,
            //ffs finds the least significant bit set to 1 1 ->32, so we need to subtract 1 to get the 0 based index.
            std::uint32_t warp_vote_before_bits = warp_digit_vote_totals & warp_vote_before_inclusive_mask;
            std::uint32_t lowest_relevant_warp_id = __ffs(warp_vote_before_bits) - 1;
            std::uint16_t previous_count;
            if(lowest_relevant_warp_id == warp_thread_id){
                //store in local memory to avoid register pressure?
                previous_count = digit_prefix_sum_offsets_view[value_selected_digit][warp_index];
                digit_prefix_sum_offsets[(warp_index * digit_value_count) + value_selected_digit] += __popc(warp_digit_vote_totals);
            }
            __syncwarp();
            previous_count = __shfl_sync(mask, previous_count, lowest_relevant_warp_id);
            //TODO need to get current relative warp index?
            //includes the current thread, so subtract 1.
            std::uint32_t lesser_thread_count = __popc(warp_vote_before_bits) - 1;
            //now get the correct offset count relative to current warp.
            local_offsets[local_item_idx] = previous_count + lesser_thread_count;
            local_values[local_item_idx] = value;
        }


        __syncthreads();
        auto digit_prefix_sum_offsets_linear_view = digit_prefix_sum_offsets_view.linear_view();

        //TODO handle non pow2 or even just not 8
        auto block_digit_prefix_sum_idx = thread_id;
        //this performs prefix sums for each value.
        //do we actually want to exclusive prefix, given that we need that last value to figure out the final output?
        //should be on warp boundary?
        while(block_digit_prefix_sum_idx > digit_prefix_sum_offsets_linear_view.size()){
            auto value = digit_prefix_sum_offsets_linear_view[block_digit_prefix_sum_idx];
            //TODO handle non pow2 or even just not 8
            auto prefix_sum_result = hills_steele_warp_inclusive_prefix_sum(value, 0b11111110'11111110'11111110'11111110, 8);
            digit_prefix_sum_offsets_linear_view[block_digit_prefix_sum_idx] = prefix_sum_result;
            block_digit_prefix_sum_idx += blockDim.x;
            //if we are at the end of the value list!
            //TODO handle non pow2 or even just not 8 should I just bite the bullet and inprefix sum?
        }
        __syncthreads();

        //writing the values back out to the coalesced location
        for(std::uint32_t local_item_idx = 0; local_item_idx < elements_per_thread; ++local_item_idx) {
            std::uint32_t global_item_idx = block_offset + local_item_idx * blockDim.x;
            if (global_item_idx < input.size()) {
                break;
            }
            auto offset = local_offsets[local_item_idx];
            auto value = local_values[local_item_idx];
            std::uint32_t value_selected_digit = (value >> (digit_bit_count * digit_iteration)) & digit_bit_mask;
            if(warp_index > 0){ //inclusive prefix sum given , but exclusive offset needed, if first warp index, then always 0 for exclusive prefix sum
                auto prefix_sum = digit_prefix_sum_offsets_view[value_selected_digit][warp_index - 1];
                auto block_index = offset + prefix_sum;
                coalesced_output[block_index] = value;
            }else{
                auto block_index = offset + 0;
                coalesced_output[block_index] = value;
            }

        }
        __syncthreads();
        std::uint32_t workgroup_aggregate = 0;
        //first virtual processor puts zero offset, since it's the first.  b
        if(thread_id < digit_value_count) {
            cuda::atomic_ref<FlagOffset, cuda::thread_scope_device> atomic_prefix_offset(
                    &global_prefix_offsets[virtual_processor_id * digit_value_count + thread_id]
            );
            FlagOffset flag_offset;
            //get the actual total value here.
            //TODO handle non pow2 or even just not 8
            flag_offset.value = digit_prefix_sum_offsets_view[thread_id][7];
            if(virtual_processor_id == 0) {
                flag_offset.set_flag(
                        OffsetFlag::FlagPrefixSumReady);
                // since there's nothing behind, we can say the prefix sum is ready.
                atomic_prefix_offset.store(flag_offset, cuda::memory_order_relaxed);
            }else{
                flag_offset.set_flag(OffsetFlag::FlagAggregateReady); // otherwise we can only say that this specific sum is ready.

                atomic_prefix_offset.store(flag_offset, cuda::memory_order_relaxed);

                auto next_virtual_processor_id = virtual_processor_id - 1;
                //if global item has been totally set? just grab, if not, keep going backwards and grabbing more.
                while (true) {
                    cuda::atomic_ref<FlagOffset, cuda::thread_scope_device> next_atomic_prefix_offset(
                            &global_prefix_offsets[next_virtual_processor_id * digit_value_count + thread_id]
                    );

                    auto prefix_offset = next_atomic_prefix_offset.load(cuda::memory_order_relaxed);
                    if (prefix_offset.get_flag() == OffsetFlag::FlagNotReady) {
                        __threadfence_block();
                        continue;
                    } else if (prefix_offset.get_flag() == OffsetFlag::FlagAggregateReady) {
                        workgroup_aggregate += prefix_offset.get_value();
                        if(next_virtual_processor_id == 0){
                            FlagOffset final_flag_offset{workgroup_aggregate};
                            flag_offset.set_flag(OffsetFlag::FlagPrefixSumReady);
                            atomic_prefix_offset.store(final_flag_offset, cuda::memory_order_relaxed);
                            break;
                        }
                    } else {// if(prefix_offset.get_flag() == OffsetFlag::FlagPrefixSumReady){
                        workgroup_aggregate += prefix_offset.get_value();
                        FlagOffset final_flag_offset{workgroup_aggregate};
                        flag_offset.set_flag(OffsetFlag::FlagPrefixSumReady);
                        atomic_prefix_offset.store(final_flag_offset, cuda::memory_order_relaxed);
                        break;
                    }
                }
            }
        }
        __syncthreads();
        for(std::uint32_t local_item_idx = 0; local_item_idx < elements_per_thread; ++local_item_idx) {
            std::uint32_t global_item_idx = block_offset + local_item_idx * blockDim.x;
            if (global_item_idx < input.size()) {
                break;
            }
            auto shared_coalesced_index = elements_per_thread * thread_id + local_item_idx;
            auto value = coalesced_output[shared_coalesced_index];
            std::uint32_t value_selected_digit = (value >> (digit_bit_count * digit_iteration)) & digit_bit_mask;
            auto global_digit_place_prefix_sum_value = global_digit_place_prefix_sum[digit_value_count * digit_iteration + value_selected_digit];
            output[global_digit_place_prefix_sum_value + shared_coalesced_index] = value;
        }
        //Now we should be done sorting?
        //TODO Now we need to communicate with other blocks

        //8 warps hypothetically, so that means we should probably split up 8 prefix sums at *once*.





    }
}
#endif //VULKANWORKSPACE_RADIXSORT_CUH
