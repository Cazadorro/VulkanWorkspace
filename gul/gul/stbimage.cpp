//
// Created by Shae Bolt on 4/25/2020.
//

#include "stbimage.h"
#include <glm/glm.hpp>
#include <stdexcept>


VkDeviceSize gul::StbImage::getDeviceSize() const {
    return m_frames * m_width * m_height * sizeof(stbi_uc) * m_channels;
}

VkExtent2D gul::StbImage::getExtent2D() const {
    return {static_cast<std::uint32_t>(m_width),
            static_cast<std::uint32_t>(m_height)};
}

VkExtent3D gul::StbImage::getExtent3D() const {
    return {static_cast<std::uint32_t>(m_width),
            static_cast<std::uint32_t>(m_height),
            static_cast<std::uint32_t>(m_frames)};
}

const stbi_uc *gul::StbImage::getData() const {
    return m_pixels;
}

int gul::StbImage::getChannels() const {
    return m_channels;
}

gul::StbImage::~StbImage() {
    stbi_image_free(m_pixels);
}

gul::StbImage::StbImage(stbi_uc *pixels, int width, int height, int channels)
        : m_pixels(pixels), m_width(width), m_height(height), m_frames(1),
          m_channels(channels) {
}

gul::StbImage::StbImage(stbi_uc *pixels, int width, int height, int frames,
                        int channels)         : m_pixels(pixels), m_width(width), m_height(height), m_frames(frames),
                                                m_channels(channels) {
}

stbi_uc &gul::StbImage::operator()(std::size_t row, std::size_t col,
                                   std::size_t channel) {
    return m_pixels[row * (m_width * m_channels) + (col * channel) + channel];
}

const stbi_uc &gul::StbImage::operator()(std::size_t row, std::size_t col,
                                         std::size_t channel) const {
    return m_pixels[row * (m_width * m_channels) + (col * channel) + channel];
}

stbi_uc &
gul::StbImage::operator()(std::size_t frame, std::size_t row, std::size_t col,
                          std::size_t channel) {
    return m_pixels[frame * (m_height * m_width * m_channels) +
                    row * (m_width * m_channels) + (col * channel) + channel];
}

const stbi_uc &
gul::StbImage::operator()(std::size_t frame, std::size_t row, std::size_t col,
                          std::size_t channel) const {
    return m_pixels[frame * (m_height * m_width * m_channels) +
                    row * (m_width * m_channels) + (col * channel) + channel];
}


gul::StbImage::StbImage(const gul::StbImage &rhs) : m_width(rhs.m_width),
                                                    m_height(rhs.m_height),
                                                    m_frames(rhs.m_frames),
                                                    m_channels(rhs.m_channels) {
    memcpy(m_pixels, rhs.m_pixels, static_cast<size_t>(rhs.getDeviceSize()));
}

gul::StbImage &gul::StbImage::operator=(const gul::StbImage &rhs) {
    if (&rhs != this) {
        stbi_image_free(m_pixels);
        m_width = rhs.m_width;
        m_height = rhs.m_height;
        m_frames = rhs.m_frames;
        m_channels = rhs.m_channels;
        memcpy(m_pixels, rhs.m_pixels,
               static_cast<size_t>(rhs.getDeviceSize()));
    }
    return *this;
}


gul::StbImage::StbImage(gul::StbImage &&rhs) noexcept: m_width(rhs.m_width),
                                                       m_height(rhs.m_height),
                                                       m_frames(rhs.m_frames),
                                                       m_channels(
                                                               rhs.m_channels) {
    using std::swap;
    swap(m_pixels, rhs.m_pixels);
}

gul::StbImage &gul::StbImage::operator=(gul::StbImage &&rhs) noexcept {
    using std::swap;
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_frames = rhs.m_frames;
    m_channels = rhs.m_channels;

    swap(m_pixels, rhs.m_pixels);
    return *this;
}




void gul::load(const std::string &file_path, gul::StbImage &image,
               StbImage::Channels desired_channels, bool is_3d_rectangle) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(file_path.c_str(), &texWidth,
                                &texHeight, &texChannels,
                                static_cast<int>(desired_channels));

    if (static_cast<int>(desired_channels) != 0) {
        texChannels = static_cast<int>(desired_channels);
    }

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    if(is_3d_rectangle) {
        int texFrames;
        if(texWidth < texHeight){
            if(texHeight % texWidth != 0){
                throw std::runtime_error("not all frames in 3d texture filled!");
            }
            texFrames = texHeight/texWidth;
            texHeight = texWidth;

        }else{
            if(texWidth % texHeight != 0){
                throw std::runtime_error("not all frames in 3d texture filled!");
            }
            texFrames = texWidth/texHeight;
            texWidth = texHeight;
        }
        image = StbImage(pixels, texWidth, texHeight, texFrames, texChannels);
    }else{
        image = StbImage(pixels, texWidth, texHeight, texChannels);
    }
}
