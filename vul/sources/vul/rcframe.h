//
// Created by Bolt on 7/7/2023.
//

#ifndef VULKANWORKSPACE_RCFRAME_H
#define VULKANWORKSPACE_RCFRAME_H
#include <vector>
namespace vul {
    class Image;
    class ImageView;
    class Buffer;
    class Framebuffer;

    struct FrameData{
        std::size_t reference_counter;
        std::vector<vul::Image> images;
        std::vector<vul::ImageView> imageViews;
        std::vector<vul::Buffer> buffers;
        std::vector<vul::Framebuffer> framebuffers;
    };
    class RcFrame {
    public:
        RcFrame();
        [[nodiscard]]
        vul::Image* add(vul::Image&& image);
        [[nodiscard]]
        vul::ImageView* add(vul::ImageView&& imageView);
        [[nodiscard]]
        vul::Buffer* add(vul::Buffer&& buffer);
        [[nodiscard]]
        vul::Framebuffer* add(vul::Framebuffer&& framebuffer);
    private:
        FrameData* m_frameData = nullptr;
    };
}
#endif //VULKANWORKSPACE_RCFRAME_H
