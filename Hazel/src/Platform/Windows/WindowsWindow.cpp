// clang-format off
#include "WindowsWindow.h"

#include "Hazel/Log.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{
static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description)
{
    HAZEL_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::Create(const WindowProps& props)
{
    return new WindowsWindow(props);
}

WindowsWindow::~WindowsWindow()
{
    Shutdown();
}
void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void WindowsWindow::SetVSync(bool enabled)
{
    // glfwSwapInterval(1) 表示启用垂直同步
    // 这样 SwapBuffers 会等待显示器刷新节奏，减少画面撕裂
    // 0 表示不等待刷新，帧率可能更高，但更容易撕裂
    if (enabled)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }
}

bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    Init(props);
}
void WindowsWindow::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    HAZEL_CORE_INFO("Creadting window {0} ({1},{2})", props.Title, props.Width, props.Height);
    if (!s_GLFWInitialized)
    {
        // 注册GLFW错误回调函数
        glfwSetErrorCallback(GLFWErrorCallback);

        // GLFW 是当前平台窗口层的底层依赖。
        // 整个进程只需要初始化一次，因此使用静态标记控制。
        //
        // glfwInit() 负责初始化 GLFW 的全局状态，
        // 包括窗口系统、输入、上下文管理等基础设施。
        int success = glfwInit();
        HAZEL_CORE_ASSERT(success, "Could not initialize GLFW!");

        s_GLFWInitialized = true;
    }

    /**
     * @note
     * OpenGL 在 Windows 上大量函数并不是静态链接可直接调用的
     * 而是需要在 context 已存在之后，通过 glfwGetProcAddress 查询地址
     * 所以正确顺序必须是：
     * 1. 创建 GLFWwindow
     * 2. glfwMakeContextCurrent
     * 3. gladLoadGLLoader
     */
    // 创建原生 GLFW 窗口。
    //
    // 参数说明：
    // - 宽度
    // - 高度
    // - 标题
    // - monitor：nullptr 表示窗口模式，不是全屏
    // - share：nullptr 表示不与其他窗口共享上下文
    // clang-format off
    m_Window = glfwCreateWindow(
        static_cast<int>(props.Width), 
        static_cast<int>(props.Height), 
        m_Data.Title.c_str(), 
        nullptr,
        nullptr
    );
    // clang-format on
    // 让这个窗口的 OpenGL context 成为当前线程的激活上下文
    glfwMakeContextCurrent(m_Window);
    // 使用 GLAD 加载当前平台/驱动实际提供的 OpenGL 函数指针
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    HAZEL_CORE_ASSERT(status, "Failed to initialize Glad!");

    // 把 Hazel 自己的窗口元数据挂到 GLFWwindow 上
    // 之后在所有 GLFW 原生回调里
    // 都可以通过 glfwGetWindowUserPointer(window) 取回 WindowData
    // 从而把原生回调桥接回 Hazel 的事件系统
    glfwSetWindowUserPointer(m_Window, &m_Data);

    // 默认开启 VSync
    SetVSync(true);

    // 设置回调
    RegisterGLFWCallbacks();
}

// clang-format off
void WindowsWindow::RegisterGLFWCallbacks()
{
    // -----------------------------
    // GLFW -> Hazel 事件桥接层
    // -----------------------------
    //
    // 这里所有回调都做同一件事：
    // 1. 取回 WindowData
    // 2. 将 GLFW 原始参数转换成 Hazel 的 Event 对象
    // 3. 调用 EventCallback，把事件继续向上交给 Application
    // 窗口
    glfwSetWindowSizeCallback(m_Window,
                              [](GLFWwindow* window, int width, int height)
                              {
                                  WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                                  data.Width = width;
                                  data.Height = height;

                                  WindowResizeEvent event(width, height);
                                  data.EventCallback(event);
                              });
    glfwSetWindowCloseCallback(m_Window,
                               [](GLFWwindow* window)
                               {
                                // 注意：
			                    // 这里不是直接销毁窗口，而是发出 WindowCloseEvent
				                // 真正的退出决策由 Application 层统一处理
                                   WindowData& data =
                                       *(WindowData*)glfwGetWindowUserPointer(window);
                                   WindowCloseEvent event;
                                   data.EventCallback(event);
                               });
    // 键盘
    glfwSetKeyCallback(m_Window,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
                           WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                           switch (action)
                           {
                               case GLFW_PRESS:
                               {
                                   KeyPressedEvent event(key, 0);
                                   data.EventCallback(event);
                                   break;
                               }
                               case GLFW_RELEASE:
                               {
                                   KeyReleasedEvent event(key);
                                   data.EventCallback(event);
                                   break;
                               }
                               case GLFW_REPEAT:
                               {
                                   KeyPressedEvent event(key, 1);
                                   data.EventCallback(event);
                                   break;
                               }
                           }
                       });
    // 文本框字符输入
    glfwSetCharCallback(m_Window,
                        [](GLFWwindow* window, unsigned int keycode)
                        {
                            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                            KeyTypedEvent event(keycode);
                            data.EventCallback(event);
                        });
    // 鼠标
    glfwSetMouseButtonCallback(m_Window,
                               [](GLFWwindow* window, int button, int action, int mods)
                               {
                                   WindowData& data =
                                       *(WindowData*)glfwGetWindowUserPointer(window);
                                   switch (action)
                                   {
                                       case GLFW_PRESS:
                                       {
                                           MouseButtonPressedEvent event(button);
                                           data.EventCallback(event);
                                           break;
                                       }
                                       case GLFW_RELEASE:
                                       {
                                           MouseButtonReleasedEvent event(button);
                                           data.EventCallback(event);
                                           break;
                                       }
                                   }
                               });
    glfwSetScrollCallback(m_Window,
                          [](GLFWwindow* window, double xOffset, double yOffset)
                          {
                              WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                              MouseScrolledEvent event((float)xOffset, (float)yOffset);
                              data.EventCallback(event);
                          });
    glfwSetCursorPosCallback(m_Window,
                             [](GLFWwindow* window, double xPos, double yPos)
                             {
                                 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                                 MouseMovedEvent event((float)xPos, (float)yPos);
                                 data.EventCallback(event);
                             });
}
// clang-format on
void WindowsWindow::OnUpdate()
{
    // glfwPollEvents():
    // 从操作系统取出窗口、键盘、鼠标等消息
    // 并触发前面注册的 GLFW 回调
    glfwPollEvents();

    // glfwSwapBuffers():
    // 交换前台/后台缓冲，把当前帧渲染结果显示到屏幕上
    //
    // 如果开启了 VSync，那么交换缓冲通常会和显示器刷新同步
    glfwSwapBuffers(m_Window);
}
} // namespace Hazel