//
// Created by Shae Bolt on 5/21/2019.
//

#include "glfwwindow.h"
#include <vul/surface.h>
#include <vul/instance.h>
#include <vul/enums.h>
#include <glm/vec2.hpp>
#include <fmt/core.h>
#include <cstdint>
#include <utility>

gul::GlfwWindow::GlfwWindow(int width, int height, const std::string &title,
                            GLFWmonitor *monitor, GLFWwindow *share) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(width, height, title.c_str(), monitor, share);
    glfwSetWindowUserPointer(m_window, this);
}

gul::GlfwWindow::~GlfwWindow() {
    glfwDestroyWindow(m_window);
}

void gul::GlfwWindow::setFramebufferSizeCallback(
        const gul::GlfwWindow::FramebufferSizeCallback &callback) {
    m_framebuffer_size_callback = callback;
    auto callback_lambda = [](GLFWwindow *window, int width, int height) {
        auto this_window_ptr = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(
                window));
        this_window_ptr->m_framebuffer_size_callback(*this_window_ptr, width,
                                                     height);
    };
    glfwSetFramebufferSizeCallback(m_window, callback_lambda);
}

void gul::GlfwWindow::setMouseButtonCallback(
        const gul::GlfwWindow::MouseButtonCallback &callback) {
    m_mouse_button_callback = callback;
    auto callback_lambda = [](GLFWwindow *window, int button, int action,
                              int mods) {
        auto this_window_ptr = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(
                window));
        this_window_ptr->m_mouse_button_callback(*this_window_ptr, button,
                                                 action, mods);
    };
    glfwSetMouseButtonCallback(m_window, callback_lambda);
}

void
gul::GlfwWindow::steScrollCallback(
        const gul::GlfwWindow::ScrollCallback &callback) {
    m_scroll_callback = callback;
    auto callback_lambda = [](GLFWwindow *window, double xoffset,
                              double yoffset) {
        auto this_window_ptr = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(
                window));
        this_window_ptr->m_scroll_callback(*this_window_ptr, xoffset, yoffset);
    };
    glfwSetScrollCallback(m_window, callback_lambda);
}

void gul::GlfwWindow::setCursorPosCallback(
        const gul::GlfwWindow::CursorPosCallback &callback) {
    m_cursor_pos_callback = callback;
    auto callback_lambda = [](GLFWwindow *window, double xpos, double ypos) {
        auto this_window_ptr = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(
                window));
        this_window_ptr->m_cursor_pos_callback(*this_window_ptr, xpos, ypos);
    };
    glfwSetCursorPosCallback(m_window, callback_lambda);
}


void
gul::GlfwWindow::setKeyCallback(const gul::GlfwWindow::KeyCallback &callback) {
    m_key_callback = callback;
    auto callback_lambda = [](GLFWwindow *window, int key, int scancode,
                              int action,
                              int mods) {
        auto this_window_ptr = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(
                window));
        this_window_ptr->m_key_callback(*this_window_ptr, key, scancode, action,
                                        mods);
    };
    glfwSetKeyCallback(m_window, callback_lambda);
}

bool gul::GlfwWindow::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void gul::GlfwWindow::pollEvents() {
    glfwPollEvents();
}

void gul::GlfwWindow::waitEvents() {
    glfwWaitEvents();
}

glm::ivec2 gul::GlfwWindow::getFramebufferSize() const {
    glm::ivec2 size;
    glfwGetFramebufferSize(m_window, &size.x, &size.y);
    return size;
}

VkExtent2D gul::GlfwWindow::getFramebufferExtent() const {
    auto size = getFramebufferSize();
    return {static_cast<std::uint32_t>(size.x),
            static_cast<std::uint32_t>(size.y)};
}


glm::ivec2 gul::GlfwWindow::getWindowSize() const {
    glm::ivec2 size;
    glfwGetWindowSize(m_window, &size.x, &size.y);
    return size;
}

glm::dvec2 gul::GlfwWindow::getCursorPosition() const {
    glm::dvec2 position;
    glfwGetCursorPos(m_window, &position.x, &position.y);
    return position;
}

void gul::GlfwWindow::setCursorPosition(const glm::dvec2 &position) {
    glfwSetCursorPos(m_window, position.x, position.y);
}

bool gul::GlfwWindow::keyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool gul::GlfwWindow::keyReleased(int key) const {
    return glfwGetKey(m_window, key) == GLFW_RELEASE;
}

bool gul::GlfwWindow::keyRepeat(int key) const {
    return glfwGetKey(m_window, key) == GLFW_REPEAT;
}

std::vector<const char *>
gul::GlfwWindow::getRequiredInstanceExtensions() {
    std::uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions +
                                                         glfwExtensionCount);
    return extensions;
}

GLFWwindow *gul::GlfwWindow::getWindowPtr() {
    return m_window;
}

const GLFWwindow *gul::GlfwWindow::getWindowPtr() const {
    return m_window;
}

vul::Surface
gul::GlfwWindow::createSurface(const vul::Instance &instance,
                               const VkAllocationCallbacks *pAllocator) {
    VkSurfaceKHR surface;
    if (auto result = vul::Result(glfwCreateWindowSurface(instance.get(), m_window, pAllocator, &surface)); result !=
                                                                                                            vul::Result::Success) {
        fmt::print(stderr, "Error: {}\n", vul::to_string(result));
        std::terminate();
    }
    return vul::Surface(instance, surface, pAllocator);
}

bool gul::GlfwWindow::mousePressed(int button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

bool gul::GlfwWindow::mouseReleased(int button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_RELEASE;
}

bool gul::GlfwWindow::mouseRepeat(int button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_REPEAT;
}

gul::GlfwWindow::GlfwWindow(gul::GlfwWindow &&rhs) noexcept:
        m_window(std::exchange(rhs.m_window, nullptr)),
        m_framebuffer_size_callback(std::exchange(rhs.m_framebuffer_size_callback, nullptr)),
        m_mouse_button_callback(std::exchange(rhs.m_mouse_button_callback, nullptr)),
        m_scroll_callback(std::exchange(rhs.m_scroll_callback, nullptr)),
        m_cursor_pos_callback(std::exchange(rhs.m_cursor_pos_callback, nullptr)),
        m_key_callback(std::exchange(rhs.m_key_callback, nullptr)) {

}

gul::GlfwWindow &gul::GlfwWindow::operator=(gul::GlfwWindow &&rhs) noexcept {
    using std::swap;
    std::swap(m_window, rhs.m_window);
    std::swap(m_framebuffer_size_callback, rhs.m_framebuffer_size_callback);
    std::swap(m_mouse_button_callback, rhs.m_mouse_button_callback);
    std::swap(m_scroll_callback, rhs.m_scroll_callback);
    std::swap(m_cursor_pos_callback, rhs.m_cursor_pos_callback);
    std::swap(m_key_callback, rhs.m_key_callback);

    return *this;
}

