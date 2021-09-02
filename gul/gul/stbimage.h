//
// Created by Shae Bolt on 9/2/2021.
//

#ifndef VULKANWORKSPACE_STBIMAGE_H
#define VULKANWORKSPACE_STBIMAGE_H

#include <stb_image.h>
#include <vulkan/vulkan.h>
#include <string>

namespace gul {

    class StbImage {
    public:
        enum class Channels : int {
            automatic = STBI_default,
            grey = STBI_grey,
            grey_alpha = STBI_grey_alpha,
            rgb = STBI_rgb,
            rgb_alpha = STBI_rgb_alpha
        };

        [[nodiscard]]
        VkDeviceSize getDeviceSize() const;

        [[nodiscard]]
        VkExtent2D getExtent2D() const;

        [[nodiscard]]
        VkExtent3D getExtent3D() const;

        [[nodiscard]]
        const stbi_uc *getData() const;

        [[nodiscard]]
        int getChannels() const;

        stbi_uc &
        operator()(std::size_t row, std::size_t col, std::size_t channel);

        const stbi_uc &
        operator()(std::size_t row, std::size_t col, std::size_t channel) const;

        stbi_uc &
        operator()(std::size_t frame, std::size_t row, std::size_t col,
                   std::size_t channel);

        const stbi_uc &
        operator()(std::size_t frame, std::size_t row, std::size_t col,
                   std::size_t channel) const;

        StbImage(const StbImage &rhs);

        StbImage &operator=(const StbImage &rhs);

        StbImage(StbImage &&rhs) noexcept;

        StbImage &operator=(StbImage &&rhs) noexcept;

        ~StbImage();

        StbImage(stbi_uc *pixels, int width, int height, int channels);

        StbImage(stbi_uc *pixels, int width, int height, int frames,
                 int channels);

        StbImage() = default;

    private:
        stbi_uc *m_pixels = nullptr;
        int m_width = 1;
        int m_height = 1;
        int m_frames = 1;
        int m_channels = 0;

    };

    void load(const std::string &file_path, StbImage &image,
              StbImage::Channels desired_channels = StbImage::Channels::automatic,
              bool is_3d_rectangle = false);

//    void transferDataToImage(VkDevice device, VkPhysicalDevice physicalDevice,
//                             VkQueue queue, VkCommandPool commandPool,
//                             VkImage image, const StbImage &host_data,
//                             VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


}


#endif //VULKANWORKSPACE_STBIMAGE_H
