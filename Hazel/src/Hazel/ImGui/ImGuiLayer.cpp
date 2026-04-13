// clang-format off
#include "ImGuiLayer.h"

#include "Hazel/Application.h"

// Dear ImGui 核心
#include <imgui.h>
// 平台 / 渲染后端
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// GLFW 原生窗口类型
#include <GLFW/glfw3.h>
// clang-format on  
namespace Hazel
{
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
{
}

void ImGuiLayer::OnAttach()
{
    IMGUI_CHECKVERSION();

	// 创建 Dear ImGui 全局上下文
	// 一个 ImGui Context 可以理解为“整套 UI 状态机”的根对象
	// 包括：
	// - 字体
	// - 样式
	// - IO 状态
	// - 各窗口状态
	//
	// 没有 Context 的情况下，几乎所有 ImGui API 都不能安全使用
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    static_cast<void>(io);

	// -----------------------------
	// 配置 ImGui 功能开关
	// -----------------------------
    // 允许键盘导航。
	// 开启后，ImGui 可以更完整地接管键盘输入，例如焦点切换、快捷键等。
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
	// 启用 Docking
	// 这使得 ImGui 窗口可以像编辑器面板一样停靠、拆分、重新布局
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   
    // 启用 Multi-Viewport（多视口 / 平台窗口）
	// 开启后，Dear ImGui 会在底层创建额外的平台窗口
    // ImGui 窗口不仅能停靠在主窗口内
	// 还可以被拖出主窗口，变成真正独立的 OS 窗口
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

	// 如果你后续不想让平台窗口出现在任务栏或希望它们更强地合并主窗口，可以继续打开这些选项：
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    ImGui::StyleColorsDark();

    // 当启用多视口后，取消圆角、提高背景不透明度
    // 把主窗口风格调整得更接近独立平台窗口
    // 提高视觉自然度
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

	// 自定义主题函数，暂未实现
	// SetDarkThemeColors();

    // 取出 Hazel 自己主窗口对应的原生 GLFWwindow
	// ImGui 的 GLFW 后端初始化需要这个句柄来进行：
	// - 绑定输入
	// - 建立后端与平台窗口的桥接
    Application& app = Application::GetApplication();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

	// 初始化 ImGui 的 GLFW 平台后端
	// 第二个参数传 true，表示让 ImGui 自动安装 GLFW 回调并尝试链式转发
	// 这样 ImGui 能直接获得键盘、鼠标、滚轮等输入事件
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    // 初始化 ImGui 的 OpenGL3 渲染后端
	// "#version 410" 是传给内部 shader 的 GLSL 版本字符串
	// 它应与你当前 OpenGL 目标版本匹配
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
	// 关闭顺序：
	// 1. 先关闭渲染后端
	// 2. 再关闭平台后端
	// 3. 最后销毁 ImGui Context
	//
	// 这样可以确保后端在 Shutdown 时仍然能访问到有效的 ImGui 状态。
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnImGuiRender()
{
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

void ImGuiLayer::Begin()
{
	// 一帧 ImGui 的开始顺序：
	// 1. 渲染后端 NewFrame
	// 2. 平台后端 NewFrame
	// 3. ImGui::NewFrame()
	//
	// 这样平台输入状态、渲染状态和 ImGui 本身状态会一起进入“新的一帧”。
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::GetApplication();
    io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

    // 渲染
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

} // namespace Hazel