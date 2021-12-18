//
// Created by Shae Bolt on 12/1/2021.
//
#include <fstream>
#include <iostream>
#include <fmt/core.h>
#include <enkimi.h>
#include <vector>
#include <cstdint>
#include <limits>
#include <filesystem>
#include <gsl/span>
#include <cxxopts.hpp>

struct RegionSizeInfo{
    std::uint64_t region_size_x;
    std::uint64_t region_size_y;
    std::uint64_t region_size_z;
    std::uint64_t chunk_size_x;
    std::uint64_t chunk_size_y;
    std::uint64_t chunk_size_z;
    [[nodiscard]]
    std::uint64_t calc_chunk_size()const{
        return chunk_size_x * chunk_size_y * chunk_size_z;
    }
    [[nodiscard]]
    std::uint64_t calc_region_size()const{
        return region_size_x * region_size_y * region_size_z;
    }
    [[nodiscard]]
    std::uint64_t calc_total_size() const{
        return calc_region_size() * calc_chunk_size();
    }
    [[nodiscard]]
    std::uint64_t calc_linear_region_idx(std::uint64_t x, std::uint64_t y, std::uint64_t z) const{
        return y * region_size_x * region_size_z + z * region_size_x + x;
    }
    [[nodiscard]]
    std::uint64_t calc_linear_chunk_idx(std::uint64_t x, std::uint64_t y, std::uint64_t z) const{
        return y * chunk_size_x * chunk_size_z + z * chunk_size_x + x;
    }
};


constexpr std::uint64_t mc_region_size_x = 32;
constexpr std::uint64_t mc_region_size_y = 32;
constexpr std::uint64_t mc_region_size = mc_region_size_x * mc_region_size_y;


constexpr std::uint64_t mc_chunk_section_size = 16;
constexpr std::uint64_t mc_chunk_size_x = 16;
constexpr std::uint64_t mc_chunk_size_y = 16;
constexpr std::uint64_t mc_chunk_size_z = 16;
constexpr std::uint64_t mc_chunk_size =
        mc_chunk_size_x * mc_chunk_size_y * mc_chunk_size_z *
        mc_chunk_section_size;

constexpr std::uint64_t standard_chunk_size = 32;
constexpr std::uint64_t new_chunk_size_x = standard_chunk_size;
constexpr std::uint64_t new_chunk_size_y = standard_chunk_size;
constexpr std::uint64_t new_chunk_size_z = standard_chunk_size;
constexpr std::uint64_t new_chunk_size =
        new_chunk_size_x * new_chunk_size_y * new_chunk_size_z;

constexpr std::uint64_t new_region_size_x =
        (mc_region_size_x * mc_chunk_size_x) / new_chunk_size_x;
constexpr std::uint64_t new_region_size_y =
        (mc_chunk_section_size * mc_chunk_size_y) / new_chunk_size_y;
constexpr std::uint64_t new_region_size_z =
        (mc_region_size_y * mc_chunk_size_z) / new_chunk_size_z;

const RegionSizeInfo new_region_size_info{new_region_size_x,new_region_size_y,new_region_size_z,new_chunk_size_x,new_chunk_size_y,new_chunk_size_z};

struct Index3D {
    std::uint64_t x;
    std::uint64_t y;
    std::uint64_t z;

    [[nodiscard]]
    static Index3D from_mc_region_index(std::uint64_t x,
                                        std::uint64_t y,
                                        std::uint64_t section,
                                        std::uint64_t chunk_x,
                                        std::uint64_t chunk_y,
                                        std::uint64_t chunk_z) {
        auto x_idx = chunk_x + x * mc_chunk_size_x;
        auto y_idx = chunk_y + section * mc_chunk_size_y;
        auto z_idx = chunk_z + y * mc_chunk_size_z;
        return Index3D{x_idx, y_idx, z_idx};
    }

    [[nodiscard]]
    std::uint64_t to_new_linear_index(std::uint64_t chunk_size_x,
                                      std::uint64_t chunk_size_y,
                                      std::uint64_t chunk_size_z,
                                      std::uint64_t region_size_x,
                                      std::uint64_t region_size_y,
                                      std::uint64_t region_size_z) const {
        const auto chunk_size = chunk_size_x * chunk_size_y * chunk_size_z;
        auto chunk_x = x % chunk_size_x;
        auto chunk_y = y % chunk_size_y;
        auto chunk_z = z % chunk_size_z;
        auto region_x = x / chunk_size_x;
        auto region_y = y / chunk_size_y;
        auto region_z = z / chunk_size_z;
        auto linear_chunk_idx =
                chunk_y * chunk_size_x * chunk_size_z + chunk_z * chunk_size_x +
                chunk_x;
        auto linear_region_idx = region_y * region_size_x * region_size_z +
                                 region_z * region_size_x + region_x;
        return linear_region_idx * chunk_size + linear_chunk_idx;
    }
};

std::uint64_t mc_region_idx_to_custom_linear_idx(std::uint64_t x,
                                                 std::uint64_t y,
                                                 std::uint64_t section,
                                                 std::uint64_t chunk_x,
                                                 std::uint64_t chunk_y,
                                                 std::uint64_t chunk_z) {
    return Index3D::from_mc_region_index(x, y, section, chunk_x, chunk_y,
                                         chunk_z).to_new_linear_index(
            new_chunk_size_x, new_chunk_size_y,
            new_chunk_size_z, new_region_size_x, new_region_size_y,
            new_region_size_z);
}


std::uint64_t linear_x(std::uint64_t input_index) {
    return input_index;
}

std::uint64_t linear_z(std::uint64_t input_index) {
    auto y_index = input_index / (new_chunk_size_x * new_chunk_size_z);
    auto z_index = (input_index / (new_chunk_size_x)) % new_chunk_size_z;
    auto x_index = (input_index % new_chunk_size_x);
    return y_index * new_chunk_size_x * new_chunk_size_z +
           x_index * new_chunk_size_z + z_index;
}

std::uint64_t linear_y(std::uint64_t input_index) {
    auto y_index = input_index / (new_chunk_size_x * new_chunk_size_z);
    auto z_index = (input_index / (new_chunk_size_x)) % new_chunk_size_z;
    auto x_index = (input_index % new_chunk_size_x);
    return x_index * new_chunk_size_y * new_chunk_size_z +
           y_index * new_chunk_size_z + z_index;
}


std::vector<std::uint32_t> new_chunk_data_from_mc(std::string file_path, bool homogenize_solid_voxels = false) {
    FILE *file = fopen(file_path.c_str(), "rb");
    // auto file = std::ofstream(file_path, std::ios::in | std::ios::binary);
    if (!file) {
        fmt::print("File not open!\n");
        std::terminate();
    }

    enkiRegionFile region_file = enkiRegionFileLoad(file);

    std::vector<std::uint32_t> new_chunk_data(
            new_region_size_x * new_region_size_y * new_region_size_z *
            new_chunk_size, 0);

    for (int region_y = 0; region_y < 32; ++region_y) {
//        if(region_y >= 2){
//            break;
//        }
        for (int region_x = 0; region_x < 32; ++region_x) {
//            if(region_x >= 2){
//                break;
//            }
            auto region_i = region_y * 32 + region_x;
            enkiNBTDataStream stream;
            enkiInitNBTDataStreamForChunk(region_file, region_i, &stream);
            if (stream.dataLength) {
                enkiChunkBlockData aChunk = enkiNBTReadChunk(&stream);
                for (int section = 0;
                     section < ENKI_MI_NUM_SECTIONS_PER_CHUNK; ++section) {
                    if (aChunk.sections[section]) {
                        enkiMICoordinate sectionOrigin = enkiGetChunkSectionOrigin(
                                &aChunk, section);
                        enkiMICoordinate sPos;
                        // note order x then z then y iteration for cache efficiency
                        for (sPos.y = 0; sPos.y <
                                         ENKI_MI_NUM_SECTIONS_PER_CHUNK; ++sPos.y) {
                            for (sPos.z = 0; sPos.z <
                                             ENKI_MI_NUM_SECTIONS_PER_CHUNK; ++sPos.z) {
                                for (sPos.x = 0; sPos.x <
                                                 ENKI_MI_NUM_SECTIONS_PER_CHUNK; ++sPos.x) {

                                    auto idx = mc_region_idx_to_custom_linear_idx(
                                            region_x, region_y, section,
                                            sPos.x,
                                            sPos.y, sPos.z);
                                    uint8_t voxel = enkiGetChunkSectionVoxel(
                                            &aChunk, section, sPos);
                                    if(homogenize_solid_voxels){
                                        if(voxel != 0){
                                            new_chunk_data[idx] = 1;
                                        }else{
                                            new_chunk_data[idx] = 0;
                                        }
                                    }else{
                                        new_chunk_data[idx] = voxel;
                                    }
                                }
                            }
                        }
                    }
                }
                enkiNBTRewind(&stream);
            }
            enkiNBTFreeAllocations(&stream);
        }
    }
    enkiRegionFileFreeAllocations(&region_file);

    fclose(file);
    return new_chunk_data;
}





[[nodiscard]]
std::vector<std::uint32_t> remove_blocks_that_cant_be_seen(const std::vector<std::uint32_t>& chunk_data, const RegionSizeInfo& region_size_info){
    auto new_chunk_data = chunk_data;
    for (std::size_t region_y = 0; region_y < region_size_info.region_size_y; ++region_y) {
        for (std::size_t region_z = 0;
             region_z < region_size_info.region_size_z; ++region_z) {
            for (std::size_t region_x = 0;
                 region_x < region_size_info.region_size_x; ++region_x) {
                auto linear_region_idx = region_size_info.calc_linear_region_idx(
                        region_x, region_y, region_z);
                auto start_idx =
                        linear_region_idx * region_size_info.calc_chunk_size();
                for (std::int64_t chunk_y = 1;
                     chunk_y < region_size_info.chunk_size_y - 1; ++chunk_y) {
                    for (std::int64_t chunk_z = 1; chunk_z <
                                                   region_size_info.chunk_size_y -
                                                   1; ++chunk_z) {
                        for (std::int64_t chunk_x = 1; chunk_x <
                                                       region_size_info.chunk_size_y -
                                                       1; ++chunk_x) {
                            auto linear_cell = region_size_info.calc_linear_chunk_idx(chunk_x, chunk_y, chunk_z);
                            auto cell_position = start_idx +
                                                 region_size_info.calc_linear_chunk_idx(
                                                         chunk_x, chunk_y,
                                                         chunk_z);
                            if (chunk_data[cell_position] == 0) {
                                continue;
                            }
                            auto offset = start_idx +
                                          region_size_info.calc_linear_chunk_idx(
                                                  chunk_x - 1, chunk_y,
                                                  chunk_z);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            offset = start_idx +
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x + 1, chunk_y, chunk_z);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            offset = start_idx +
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x, chunk_y - 1, chunk_z);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            offset = start_idx +
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x, chunk_y + 1, chunk_z);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            offset = start_idx +
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x, chunk_y, chunk_z - 1);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            offset = start_idx +
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x, chunk_y, chunk_z + 1);
                            if (chunk_data[offset] == 0) {
                                continue;
                            }
                            auto taken_idx = linear_cell > 0 ? cell_position - 1 : cell_position + 1;
                            new_chunk_data[cell_position] = new_chunk_data[taken_idx];
                        }
                    }
                }
            }
        }
    }
    return new_chunk_data;
}

[[nodiscard]]
std::vector<std::uint32_t> transpose_z(const std::vector<std::uint32_t>& materials_original, const RegionSizeInfo& region_info){
    std::vector<std::uint32_t> materials;
    materials.reserve(materials_original.size());
    for(std::size_t i = 0; i < materials_original.size(); ++i){
        auto z = i % region_info.chunk_size_z;
        auto x = (i / region_info.chunk_size_z)% region_info.chunk_size_x;
        auto y = (i / (region_info.chunk_size_z * region_info.chunk_size_x));
        auto original_linear = y * (region_info.chunk_size_z * region_info.chunk_size_x) + z * region_info.chunk_size_x + x;
        materials.push_back(materials_original[original_linear]);
    }
    return materials;
}

[[nodiscard]]
std::vector<std::uint32_t> transpose_y(const std::vector<std::uint32_t>& materials_original, const RegionSizeInfo& region_info){
    std::vector<std::uint32_t> materials;
    materials.reserve(materials_original.size());
    for(std::size_t i = 0; i < materials_original.size(); ++i){
        auto y = i % region_info.chunk_size_y;
        auto x = (i / region_info.chunk_size_y)% region_info.chunk_size_x;
        auto z = (i / (region_info.chunk_size_y * region_info.chunk_size_x));
        auto original_linear = y * (region_info.chunk_size_z * region_info.chunk_size_x) + z * region_info.chunk_size_x + x;
        materials.push_back(materials_original[original_linear]);
    }
    return materials;
}

[[nodiscard]]
std::vector<std::uint32_t> clear_internal(const std::vector<std::uint32_t>& materials_original,  const RegionSizeInfo& region_size_info){
    auto materials = materials_original;
    for (std::int64_t chunk_y = 1;
         chunk_y < region_size_info.chunk_size_y - 1; ++chunk_y) {
        for (std::int64_t chunk_z = 1; chunk_z <
                                       region_size_info.chunk_size_y -
                                       1; ++chunk_z) {
            for (std::int64_t chunk_x = 1; chunk_x <
                                           region_size_info.chunk_size_y -
                                           1; ++chunk_x) {
                auto linear_cell = region_size_info.calc_linear_chunk_idx(chunk_x, chunk_y, chunk_z);
                auto cell_position =
                                     region_size_info.calc_linear_chunk_idx(
                                             chunk_x, chunk_y,
                                             chunk_z);
                if (materials_original[cell_position] == 0) {
                    continue;
                }
                auto offset =
                              region_size_info.calc_linear_chunk_idx(
                                      chunk_x - 1, chunk_y,
                                      chunk_z);
                if (materials_original[offset] == 0) {
                    continue;
                }
                offset =
                         region_size_info.calc_linear_chunk_idx(
                                 chunk_x + 1, chunk_y, chunk_z);
                if (materials_original[offset] == 0) {
                    continue;
                }
                offset =
                         region_size_info.calc_linear_chunk_idx(
                                 chunk_x, chunk_y - 1, chunk_z);
                if (materials_original[offset] == 0) {
                    continue;
                }
                offset =
                         region_size_info.calc_linear_chunk_idx(
                                 chunk_x, chunk_y + 1, chunk_z);
                if (materials_original[offset] == 0) {
                    continue;
                }
                offset =
                         region_size_info.calc_linear_chunk_idx(
                                 chunk_x, chunk_y, chunk_z - 1);
                if (materials_original[offset] == 0) {
                    continue;
                }
                offset =
                         region_size_info.calc_linear_chunk_idx(
                                 chunk_x, chunk_y, chunk_z + 1);
                if (materials_original[offset] == 0) {
                    continue;
                }
                auto taken_idx = linear_cell > 0 ? cell_position - 1 : cell_position + 1;
                materials[cell_position] = materials[taken_idx];
            }
        }
    }
    return materials;
}

struct RLEStatistics{
    std::uint64_t rle_size =0;
    std::uint64_t runs_below_thresh=0;
    std::uint64_t run_split_thresh_large = 0;
    std::uint64_t run_split_thresh_small = 0;
    std::uint64_t run_split_thresh_med =0 ;
    std::uint64_t run_split_thresh_large_med = 0;
    RLEStatistics& add(const RLEStatistics& rhs){
        rle_size += rhs.rle_size;
        runs_below_thresh += rhs.runs_below_thresh;
        run_split_thresh_large += rhs.run_split_thresh_large;
        run_split_thresh_small += rhs.run_split_thresh_small;
        run_split_thresh_med += rhs.run_split_thresh_med;
        run_split_thresh_large_med += rhs.run_split_thresh_large_med;
        return *this;
    }
};

struct GlobalRLEStatistics{
    std::uint64_t total_rle_size =0;
    std::uint64_t total_runs_below_thresh=0;
    std::uint64_t total_run_split_thresh_large = 0;
    std::uint64_t total_run_split_thresh_small = 0;
    std::uint64_t total_run_split_thresh_med =0 ;
    std::uint64_t total_run_split_thresh_large_med = 0;
    std::uint64_t max_large_med = 0;
    std::uint64_t max_rle_chunk_size=0;
    std::uint64_t min_rle_chunk_size=std::numeric_limits<std::uint64_t>::max();
    GlobalRLEStatistics& add(const RLEStatistics& rhs){
        total_rle_size += rhs.rle_size;
        total_runs_below_thresh += rhs.runs_below_thresh;
        total_run_split_thresh_large += rhs.run_split_thresh_large;
        total_run_split_thresh_small += rhs.run_split_thresh_small;
        total_run_split_thresh_med += rhs.run_split_thresh_med;
        total_run_split_thresh_large_med += rhs.run_split_thresh_large_med;
        max_large_med = std::max(max_large_med, rhs.run_split_thresh_large_med);
        max_rle_chunk_size = std::max(max_rle_chunk_size, rhs.rle_size);
        min_rle_chunk_size = std::min(max_rle_chunk_size, rhs.rle_size);
        return *this;
    }
    [[nodiscard]]
    std::string create_info_string(std::uint64_t current_region_count, std::uint64_t chunk_count){
        double total_chunk_count = current_region_count * chunk_count;
        return fmt::format("average rle_sections {}, max {}, min {}\n, average individual run size {}, small rle_section ratio {}, large,small rle sections {}, large {}, small {}\n"
                   "med total {}, large med {}, med {}\n"
                   "max large med {}\n",
                   total_rle_size / total_chunk_count,
                   max_rle_chunk_size,
                   min_rle_chunk_size,
                   (new_region_size_info.calc_total_size() * current_region_count) / static_cast<double>(total_rle_size),
                   static_cast<double>(total_runs_below_thresh) / total_rle_size,
                   (total_run_split_thresh_large + total_run_split_thresh_small) / total_chunk_count,
                   (total_run_split_thresh_large) / total_chunk_count,
                   (total_run_split_thresh_small) / total_chunk_count,
                   (total_run_split_thresh_large_med + total_run_split_thresh_med + total_run_split_thresh_small) / total_chunk_count,
                   (total_run_split_thresh_large_med) / total_chunk_count,
                   (total_run_split_thresh_med) / total_chunk_count,
                   max_large_med);
    }
};

struct RLE{
    std::vector<std::uint64_t> rle_end_position;
    std::vector<std::uint64_t> rle_count;
    std::vector<std::uint64_t> rle_material;
    [[nodiscard]]
    std::size_t size() const{
        return rle_count.size();
    }
    [[nodiscard]]
    static RLE from_range(const gsl::span<std::uint32_t>& materials, const RegionSizeInfo& region_info = {}, bool homogenize_hidden_blocks = false){
        RLE rle{};
        std::uint32_t old_chunk_id = materials[linear_x(0)];
        rle.rle_material.push_back(old_chunk_id);
        std::uint64_t current_rle_size = 1;
        for (std::size_t i = 1; i < materials.size(); ++i) {
            auto new_chunk_id = materials[linear_x(i)];
            if (new_chunk_id != old_chunk_id) {
                auto previous_position = rle.rle_end_position.empty() ? 0 : rle.rle_end_position.back();
                rle.rle_end_position.push_back(previous_position + current_rle_size);
                rle.rle_count.push_back(current_rle_size);
                rle.rle_material.push_back(new_chunk_id);
                old_chunk_id = new_chunk_id;
                current_rle_size = 0;
            }

            current_rle_size += 1;
        }
        auto previous_position = rle.rle_end_position.empty() ? 0 : rle.rle_end_position.back();
        rle.rle_end_position.push_back(previous_position + current_rle_size);
        rle.rle_count.push_back(current_rle_size);
        if(rle.rle_end_position.size() != rle.rle_count.size() || rle.rle_count.size() != rle.rle_material.size()){
            throw std::runtime_error("Sizes must match");
        }
        return rle;
    }
    [[nodiscard]]
    static RLE from_range(std::vector<std::uint32_t> materials){
        return RLE::from_range(gsl::span(materials));
    }
    [[nodiscard]]
    std::tuple<RLE,RLE> split_thresh(std::uint64_t threshold) const{
        RLE small;
        RLE large;
        for(std::size_t i = 0; i < rle_count.size(); ++i){
            if(rle_count[i] <= threshold){
                small.rle_count.push_back(rle_count[i]);
                small.rle_end_position.push_back(rle_end_position[i]);
                small.rle_material.push_back(rle_material[i]);
            }else{
                large.rle_count.push_back(rle_count[i]);
                large.rle_end_position.push_back(rle_end_position[i]);
                large.rle_material.push_back(rle_material[i]);
            }
        }
        RLE large_temp;
        for(std::size_t i = 0; i < large.rle_count.size(); ++i){
            if(large_temp.rle_count.empty() || large.rle_material[i] != large_temp.rle_material.back()){
                large_temp.rle_count.push_back(large.rle_count[i]);
                large_temp.rle_end_position.push_back(large.rle_end_position[i]);
                large_temp.rle_material.push_back(large.rle_material[i]);
            }else{ //large.rle_material[i] == large_temp.rle_material.back()){
                large_temp.rle_count.back() += large.rle_count[i];
                large_temp.rle_end_position.back() = large.rle_end_position[i];
            }
        }
        return {small, large_temp};
    }
    [[nodiscard]]
    RLEStatistics calc_rle_statistics(std::uint64_t run_size_thresh, std::uint64_t run_small_split_thresh, std::uint64_t run_med_split_thresh){
        RLEStatistics rle_statistics{};
        auto[small_rle, large_rle] = split_thresh(run_small_split_thresh);
        auto[med_rle, large_left_rle] = large_rle.split_thresh(run_med_split_thresh);
        rle_statistics.run_split_thresh_small = small_rle.rle_count.size();
        rle_statistics.run_split_thresh_large = large_rle.rle_count.size();
        rle_statistics.run_split_thresh_med = med_rle.rle_count.size();
        rle_statistics.run_split_thresh_large_med = large_left_rle.rle_count.size();
        rle_statistics.rle_size = size();
        rle_statistics.runs_below_thresh = std::count_if(
                rle_count.begin(),
                rle_count.end(),
                [run_size_thresh](auto x) {
                    return x < run_size_thresh;
                }
        );
        return rle_statistics;
    }
};

std::size_t number_of_files_in_directory(std::filesystem::path path)
{
    using std::filesystem::directory_iterator;
    return std::distance(directory_iterator(path), directory_iterator{});
}


enum class RLEOrientation{
    X = 0,
    Y = 1,
    Z = 2
};
int main(int argc, char** argv) {
    cxxopts::Options options("RLE statistics", "Directory to region files");
    options.add_options()
            ("i,input", "Input File directory for region file dir, use quotes around strings with spaces", cxxopts::value<std::string>())
            ("h,help", "Print usage")
            ;

    auto command_options_result = options.parse(argc, argv);
    std::string directory_path("C:\\Users\\Shae Bolt\\Documents\\GitRepositories\\VulkanWorkspace\\chunk_compression_testing\\Broville v11.1\\Broville v11.1\\region\\");
    if (command_options_result.count("input")) {
        directory_path = command_options_result["input"].as<std::string>();
    }
    if (command_options_result.count("help")){
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::uint64_t chunk_count =
            new_region_size_x * new_region_size_y * new_region_size_z;
    std::uint64_t chunk_size = new_chunk_size;
    std::uint64_t total_min_rle_count = 0;

    std::uint64_t run_size_thresh = 256;
    std::uint64_t run_split_thresh = 2;
    std::uint64_t run_split_thresh_med = 16;

    GlobalRLEStatistics rle_stats_x{};
    GlobalRLEStatistics rle_stats_y{};
    GlobalRLEStatistics rle_stats_z{};
    GlobalRLEStatistics rle_stats_min{};
    const std::filesystem::path region_path{directory_path};
    auto region_count = number_of_files_in_directory(region_path);
    std::uint64_t current_region_count = 0;
    for(const auto& dir_entry : std::filesystem::directory_iterator{region_path})
    {
        current_region_count += 1;
        fmt::print("iterating through {}\n", dir_entry.path().string());
        auto region_file_path = dir_entry.path().string();
        auto new_chunk_data = new_chunk_data_from_mc(region_file_path, true);
//        new_chunk_data = remove_blocks_that_cant_be_seen(new_chunk_data,
//                                                         new_region_size_info);
        for (std::size_t region_y = 0;
             region_y < new_region_size_y; ++region_y) {
            for (std::size_t region_z = 0;
                 region_z < new_region_size_z; ++region_z) {
                for (std::size_t region_x = 0;
                     region_x < new_region_size_x; ++region_x) {
                    auto linear_region_idx =
                            region_y * new_region_size_x * new_region_size_z +
                            region_z * new_region_size_x + region_x;
                    auto start_idx = linear_region_idx * chunk_size;
                    RLEStatistics current_min_rle;
                    RLEOrientation min_orientation = RLEOrientation::X;
                    auto chunk_vector = std::vector<std::uint32_t>(new_chunk_data.begin()+start_idx,
                                               new_chunk_data.begin()+start_idx+chunk_size);

                    {
                        auto rle = RLE::from_range(clear_internal(chunk_vector, new_region_size_info));
                        auto rle_stats = rle.calc_rle_statistics(run_size_thresh, run_split_thresh, run_split_thresh_med);
                        rle_stats_x.add(rle_stats);
                        current_min_rle = rle_stats;
                        min_orientation = RLEOrientation::X;
                    }
                    {
                        auto rle = RLE::from_range(
                                clear_internal(transpose_y(chunk_vector, new_region_size_info), new_region_size_info));
                        auto rle_stats = rle.calc_rle_statistics(run_size_thresh, run_split_thresh, run_split_thresh_med);
                        rle_stats_y.add(rle_stats);
                        if(current_min_rle.rle_size > rle_stats.rle_size){
                            current_min_rle = rle_stats;
                            min_orientation = RLEOrientation::Y;
                        }

                    }
                    {
                        auto rle = RLE::from_range(
                                clear_internal(transpose_z(chunk_vector, new_region_size_info), new_region_size_info));
                        auto rle_stats = rle.calc_rle_statistics(run_size_thresh, run_split_thresh, run_split_thresh_med);
                        rle_stats_z.add(rle_stats);
                        if(current_min_rle.rle_size > rle_stats.rle_size){
                            current_min_rle = rle_stats;
                            min_orientation = RLEOrientation::Z;
                        }
                    }
                    rle_stats_min.add(current_min_rle);
                }
            }
        }
        double total_chunk_count = current_region_count * chunk_count;
        fmt::print("MIN\n");
        fmt::print(rle_stats_min.create_info_string(current_region_count, chunk_count));
        fmt::print("X\n");
        fmt::print(rle_stats_x.create_info_string(current_region_count, chunk_count));
        fmt::print("Y\n");
        fmt::print(rle_stats_y.create_info_string(current_region_count, chunk_count));
        fmt::print("Z\n");
        fmt::print(rle_stats_z.create_info_string(current_region_count, chunk_count));
        fmt::print("\n");
    }

    double total_chunk_count = current_region_count * chunk_count;
    fmt::print("MIN\n");
    fmt::print(rle_stats_min.create_info_string(current_region_count, chunk_count));
    fmt::print("X\n");
    fmt::print(rle_stats_x.create_info_string(current_region_count, chunk_count));
    fmt::print("Y\n");
    fmt::print(rle_stats_y.create_info_string(current_region_count, chunk_count));
    fmt::print("Z\n");
    fmt::print(rle_stats_z.create_info_string(current_region_count, chunk_count));
    fmt::print("\n");

    return 0;
}
