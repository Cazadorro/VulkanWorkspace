//
// Created by Bolt on 7/7/2023.
//

#include "rcframe.h"
#include "vul/image.h"
#include "vul/imageview.h"
#include "vul/buffer.h"
#include "vul/framebuffer.h"

vul::RcFrame::RcFrame() : m_frameData(new FrameData{}) {
    m_frameData->reference_counter = 0;
}

vul::Image *vul::RcFrame::add(vul::Image &&image) {
    m_frameData->images.emplace_back(std::move(image));
    return &m_frameData->images.back();
}

vul::ImageView *vul::RcFrame::add(vul::ImageView &&imageView) {
    m_frameData->imageViews.emplace_back(std::move(imageView));
    return &m_frameData->imageViews.back();
}

vul::Buffer *vul::RcFrame::add(vul::Buffer &&buffer) {
    m_frameData->buffers.emplace_back(std::move(buffer));
    return &m_frameData->buffers.back();
}

vul::Framebuffer *vul::RcFrame::add(vul::Framebuffer &&framebuffer) {
    m_frameData->framebuffers.emplace_back(std::move(framebuffer));
    return &m_frameData->framebuffers.back();
}
