//
// Created by Shae Bolt on 5/21/2019.
//

#ifndef VULKANPROJECT_GLFWWINDOW_H
#define VULKANPROJECT_GLFWWINDOW_H

//#define GLFW_INCLUDE_VULKAN


#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <string>
#include <functional>
#include <vector>
namespace vul{
    class Surface;
    class Instance;
}
namespace gul {
    class GlfwWindow {
    public:

        static void pollEvents();

        static void waitEvents();

        static std::vector<const char *> getRequiredInstanceExtensions();

        bool shouldClose() const;

        glm::ivec2 getFramebufferSize() const;

        VkExtent2D getFramebufferExtent() const;

        glm::ivec2 getWindowSize() const;

        glm::dvec2 getCursorPosition() const;

        void setCursorPosition(const glm::dvec2 &position);

        bool keyPressed(int key) const;

        bool keyReleased(int key) const;

        bool keyRepeat(int key) const;

        GlfwWindow(int width, int height, const std::string &title,
                   GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr);

        GLFWwindow *getWindowPtr();

        ~GlfwWindow();

        using WindowPosCallback = std::function<void(GlfwWindow &, int, int)>;

        using WindowSizeCallback = std::function<void(GlfwWindow &, int, int)>;

        using WindowCloseCallback = std::function<void(GlfwWindow &)>;

        using WindowRefreshCallback = std::function<void(GlfwWindow &)>;

        using WindowFocusCallback = std::function<void(GlfwWindow &, int)>;

        using WindowIconifyCallback = std::function<void(GlfwWindow &, int)>;

        using WindowMaximizeCallback = std::function<void(GlfwWindow &, int)>;

        using FramebufferSizeCallback = std::function<void(GlfwWindow &, int,
                                                           int)>;

        using WindowContentsScaleCallback = std::function<void(GlfwWindow &,
                                                               float,
                                                               float)>;

        using MouseButtonCallback = std::function<void(GlfwWindow &, int, int,
                                                       int)>;

        using CursorPosCallback = std::function<void(GlfwWindow &, double,
                                                     double)>;

        using CursorEnterCallback = std::function<void(GlfwWindow &, int)>;

        using ScrollCallback = std::function<void(GlfwWindow &, double,
                                                  double)>;

        using KeyCallback = std::function<void(GlfwWindow &, int, int, int,
                                               int)>;

        using CharCallback = std::function<void(GlfwWindow &, unsigned int)>;

        using DropCallback = std::function<void(GlfwWindow &, int,
                                                const char **)>;

        using JoystickCallback = std::function<void(GlfwWindow &, int, int)>;

        void
        setFramebufferSizeCallback(const FramebufferSizeCallback &callback);

        void setMouseButtonCallback(const MouseButtonCallback &callback);

        void steScrollCallback(const ScrollCallback &callback);

        void setCursorPosCallback(const CursorPosCallback &callback);

        void setKeyCallback(const KeyCallback &callback);

        [[nodiscard]]
        vul::Surface
        createSurface(const vul::Instance& instance,
                   const VkAllocationCallbacks *pAllocator = nullptr);

    private:
        GLFWwindow *m_window;

        FramebufferSizeCallback m_framebuffer_size_callback;
        MouseButtonCallback m_mouse_button_callback;
        ScrollCallback m_scroll_callback;
        CursorPosCallback m_cursor_pos_callback;
        KeyCallback m_key_callback;

    };
}


#endif //VULKANPROJECT_GLFWWINDOW_H
