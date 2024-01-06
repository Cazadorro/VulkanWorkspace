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

namespace glfw {
    enum class Key : int {
        Unknown = GLFW_KEY_UNKNOWN,
        Space = GLFW_KEY_SPACE,
        Apostrophe = GLFW_KEY_APOSTROPHE,
        Comma = GLFW_KEY_COMMA,
        Minus = GLFW_KEY_MINUS,
        Period = GLFW_KEY_PERIOD,
        Slash = GLFW_KEY_SLASH,
        Num0 = GLFW_KEY_0,
        Num1 = GLFW_KEY_1,
        Num2 = GLFW_KEY_2,
        Num3 = GLFW_KEY_3,
        Num4 = GLFW_KEY_4,
        Num5 = GLFW_KEY_5,
        Num6 = GLFW_KEY_6,
        Num7 = GLFW_KEY_7,
        Num8 = GLFW_KEY_8,
        Num9 = GLFW_KEY_9,
        Semicolon = GLFW_KEY_SEMICOLON,
        Equal = GLFW_KEY_EQUAL,
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,
        LeftBracket = GLFW_KEY_LEFT_BRACKET,
        Backslash = GLFW_KEY_BACKSLASH,
        RightBracket = GLFW_KEY_RIGHT_BRACKET,
        GraveAccent = GLFW_KEY_GRAVE_ACCENT,
        World1 = GLFW_KEY_WORLD_1,
        World2 = GLFW_KEY_WORLD_2,

        Escape = GLFW_KEY_ESCAPE,
        Enter = GLFW_KEY_ENTER,
        Tab = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
        Insert = GLFW_KEY_INSERT,
        Delete = GLFW_KEY_DELETE,
        Right = GLFW_KEY_RIGHT,
        Left = GLFW_KEY_LEFT,
        Down = GLFW_KEY_DOWN,
        Up = GLFW_KEY_UP,
        PageUp = GLFW_KEY_PAGE_UP,
        PageDown = GLFW_KEY_PAGE_DOWN,
        Home = GLFW_KEY_HOME,
        End = GLFW_KEY_END,
        CapsLock = GLFW_KEY_CAPS_LOCK,
        ScrollLock = GLFW_KEY_SCROLL_LOCK,
        NumLock = GLFW_KEY_NUM_LOCK,
        PrintScreen = GLFW_KEY_PRINT_SCREEN,
        Pause = GLFW_KEY_PAUSE,
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12,
        F13 = GLFW_KEY_F13,
        F14 = GLFW_KEY_F14,
        F15 = GLFW_KEY_F15,
        F16 = GLFW_KEY_F16,
        F17 = GLFW_KEY_F17,
        F18 = GLFW_KEY_F18,
        F19 = GLFW_KEY_F19,
        F20 = GLFW_KEY_F20,
        F21 = GLFW_KEY_F21,
        F22 = GLFW_KEY_F22,
        F23 = GLFW_KEY_F23,
        F24 = GLFW_KEY_F24,
        F25 = GLFW_KEY_F25,
        Keypad0 = GLFW_KEY_KP_0,
        Keypad1 = GLFW_KEY_KP_1,
        Keypad2 = GLFW_KEY_KP_2,
        Keypad3 = GLFW_KEY_KP_3,
        Keypad4 = GLFW_KEY_KP_4,
        Keypad5 = GLFW_KEY_KP_5,
        Keypad6 = GLFW_KEY_KP_6,
        Keypad7 = GLFW_KEY_KP_7,
        Keypad8 = GLFW_KEY_KP_8,
        Keypad9 = GLFW_KEY_KP_9,
        KeypadDecimal = GLFW_KEY_KP_DECIMAL,
        KeypadDivide = GLFW_KEY_KP_DIVIDE,
        KeypadMultiply = GLFW_KEY_KP_MULTIPLY,
        KeypadSubtract = GLFW_KEY_KP_SUBTRACT,
        KeypadAdd = GLFW_KEY_KP_ADD,
        KeypadEnter = GLFW_KEY_KP_ENTER,
        KeypadEqual = GLFW_KEY_KP_EQUAL,
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        LeftControl = GLFW_KEY_LEFT_CONTROL,
        LeftAlt = GLFW_KEY_LEFT_ALT,
        LeftSuper = GLFW_KEY_LEFT_SUPER,
        RightShift = GLFW_KEY_RIGHT_SHIFT,
        RightControl = GLFW_KEY_RIGHT_CONTROL,
        RightAlt = GLFW_KEY_RIGHT_ALT,
        RightSuper = GLFW_KEY_RIGHT_SUPER,
        Menu = GLFW_KEY_MENU,
    };
}
namespace vul {
    class Surface;

    class Instance;
}
namespace gul {
    class GlfwWindow {
    public:

        static void pollEvents();

        static void waitEvents();

        [[nodiscard]]
        static std::vector<const char *> getRequiredInstanceExtensions();

        [[nodiscard]]
        bool shouldClose() const;

        [[nodiscard]]
        glm::ivec2 getFramebufferSize() const;

        [[nodiscard]]
        VkExtent2D getFramebufferExtent() const;

        [[nodiscard]]
        glm::ivec2 getWindowSize() const;

        [[nodiscard]]
        glm::dvec2 getCursorPosition() const;

        void setCursorPosition(const glm::dvec2 &position);

        [[nodiscard]]
        bool keyPressed(int key) const;

        [[nodiscard]]
        bool keyReleased(int key) const;

        [[nodiscard]]
        bool keyRepeat(int key) const;

        [[nodiscard]]
        bool mousePressed(int button) const;

        [[nodiscard]]
        bool mouseReleased(int button) const;

        [[nodiscard]]
        bool mouseRepeat(int button) const;

        GlfwWindow(int width, int height, const std::string &title,
                   GLFWmonitor *monitor = nullptr, GLFWwindow *share = nullptr);

        GlfwWindow(GlfwWindow&& rhs) noexcept;
        GlfwWindow& operator=(GlfwWindow&& rhs) noexcept;
        GlfwWindow(const GlfwWindow& rhs) = delete;
        GlfwWindow& operator=(const GlfwWindow& rhs) = delete;
        GLFWwindow *getWindowPtr();

        const GLFWwindow *getWindowPtr() const;

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
        createSurface(const vul::Instance &instance,
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
