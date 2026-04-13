# Hazel Engine v1.0.1

Hazel Engine 是一个基于 **C++20** 的早期引擎架构练习项目。
当前工程采用 **引擎核心库（Hazel）+ 客户端示例程序（Sandbox）** 的组织方式，
目标是在尽量小的代码体量下，把窗口、事件、Layer、输入、ImGui 和基础渲染主循环先串起来，
再逐步向渲染抽象、编辑器和场景系统演进。

> 目前的 Hazel 更适合被理解为一个“主干已经打通、基础设施持续补齐中的引擎实验项目”，
> 而不是一个功能完备的成品引擎。

---

## 1. 当前进度概览

相比更早阶段，本工程现在已经不仅仅是“窗口 + 日志 + 事件”的最小骨架，
而是继续补齐了几条非常关键的运行时链路：

- 引擎与客户端分离
- `Application` 主循环与生命周期管理
- `Window` 抽象与 `WindowsWindow` 平台实现
- 基于 GLFW 回调的 Hazel 事件系统
- `Layer` / `LayerStack`
- `Input` 抽象与 `WindowsInput` 平台实现
- ImGui 集成，支持 **Docking** 与 **Multi-Viewport**
- 基于 `spdlog + fmt` 的日志系统
- `GLM` 数学库接入与自定义 formatter
- 基于 CMake 的第三方依赖组织与输出目录管理

当前已经具备一个可持续扩展的运行骨架：

```text
Sandbox
  -> 定义客户端 Layer / Application
  -> 调用 Hazel 暴露的公共 API

Hazel
  -> 管理应用生命周期
  -> 管理 LayerStack
  -> 管理窗口与事件桥接
  -> 管理 ImGui 生命周期
  -> 管理输入查询与日志系统
```

---

## 2. 项目定位

这个项目当前阶段的核心目标，不是堆叠大量高层功能，而是先解决以下工程问题：

1. **如何划分引擎与客户端的职责边界**
2. **如何建立清晰可扩展的主循环**
3. **如何屏蔽 GLFW / OpenGL 细节**
4. **如何为 Layer、ImGui、输入系统预留统一挂载点**
5. **如何让未来的渲染抽象顺利落地**

因此当前版本更强调：

- 模块职责是否清晰
- 依赖方向是否合理
- 主循环与事件链是否顺畅
- OpenGL / GLFW / GLAD 初始化顺序是否正确
- 工程组织是否便于继续演进

---

## 3. 工程整体结构

### 3.1 根工程层
根目录负责整个工程的构建组织。

根 `CMakeLists.txt` 主要承担以下职责：

- 设置全局 C++ 标准与编译选项
- 配置 MSVC 运行时库策略
- 引入第三方库 `GLFW`、`GLAD`、`ImGui`、`GLM`
- 统一二进制输出目录
- 组织 `Hazel` 与 `Sandbox` 两个子项目

这一层不直接实现引擎逻辑，而是作为整个项目的构建与输出管理中心。

### 3.2 Hazel 引擎层
`Hazel` 当前构建为一个 **共享库（DLL）**，承载引擎核心逻辑。

当前主要包含以下模块：

- `Application`：应用主控层
- `Window`：窗口抽象接口
- `WindowsWindow`：基于 GLFW 的 Windows 平台窗口实现
- `Event` 体系：事件描述与事件分发
- `Layer` / `LayerStack`
- `Input` / `WindowsInput`
- `ImGuiLayer`
- `Log`：日志系统
- `EntryPoint`：程序入口桥接
- `SpdlogFormatters`：GLM 与 Event 的日志格式化支持

Hazel 的职责是提供统一的运行框架和基础设施，而不是直接承载客户端业务。

### 3.3 Sandbox 客户端层
`Sandbox` 是最小客户端示例程序。

它当前主要做三件事：

- 继承 `Hazel::Application`
- 在构造时压入一个 `ExampleLayer`
- 使用 `Input` 与事件系统验证引擎主链是否可用

---

## 4. 程序启动与运行流程

当前项目采用“**客户端提供应用对象，引擎提供主入口**”的启动模式。

### 4.1 启动流程

```text
main
  -> Log::Init()
  -> CreateApplication()
  -> Application 构造
       -> Window::Create()
       -> WindowsWindow::Init()
       -> 创建 ImGuiLayer 并压入 LayerStack
  -> Application::Run()
       -> OpenGL 清屏
       -> Layer::OnUpdate()
       -> ImGui Begin/End
       -> glfwPollEvents + glfwSwapBuffers
  -> Application 析构
```

### 4.2 当前价值

这种结构的好处是：

- 所有客户端都走同一套启动与退出流程
- 客户端不需要自己写窗口初始化或原生消息循环
- 后续若加入渲染器、资源系统、编辑器初始化，可以统一放在引擎入口或 `Application` 中管理

---

## 5. Application：应用主控层

`Application` 是当前运行时的总调度器，也是现阶段架构中的中心对象。

### 5.1 当前职责

- 创建并持有主窗口
- 注册窗口事件回调
- 管理主循环
- 管理 `LayerStack`
- 驱动 `ImGuiLayer`
- 决定程序何时退出

### 5.2 当前运行逻辑

当前主循环大致如下：

1. 调用基础 OpenGL 清屏
2. 遍历 `LayerStack` 执行 `OnUpdate()`
3. 让 `ImGuiLayer` 开始一帧 ImGui
4. 遍历 `LayerStack` 执行 `OnImGuiRender()`
5. 让 `ImGuiLayer` 提交 ImGui DrawData
6. 调用窗口 `OnUpdate()`，完成事件轮询与缓冲交换

### 5.3 所有权注意事项

当前 `LayerStack` **拥有所有 layer/overlay 的生命周期**，会在析构时统一 `delete`。
因此 `Application` 中缓存的 `m_ImGuiLayer` 只是一个 **non-owning 指针**，
方便每帧调用 `Begin()` / `End()`，但不负责释放。

- 若把 `ImGuiLayer` 同时交给 `std::unique_ptr` 和 `LayerStack` 管理
- 应用退出时会发生 **双重析构**
- 最终表现为崩溃、heap corruption 或 heap overflow

---

## 6. Window：窗口抽象层

当前工程并没有让 `Application` 直接依赖 GLFW，而是先定义了一个抽象接口 `Window`。

### 6.1 当前暴露能力

- 窗口更新
- 获取窗口尺寸
- 设置事件回调
- 控制垂直同步
- 查询垂直同步状态
- 获取原生窗口句柄
- 通过工厂函数创建平台窗口实现

### 6.2 设计意义

这层抽象的价值在于：

- 上层依赖“窗口能力”，而不是具体平台 API
- `Application` 不需要知道 GLFW 窗口类型和回调细节
- 后续若切换平台或引入 `GraphicsContext`，有明确边界可扩展

---

## 7. WindowsWindow：GLFW + OpenGL + GLAD 初始化链路

`WindowsWindow` 打通了：

- GLFW 窗口系统
- OpenGL 上下文
- GLAD 函数加载
- Hazel 事件系统

### 7.1 当前职责

- 初始化 GLFW
- 创建 GLFWwindow
- 绑定当前 OpenGL 上下文
- 使用 GLAD 加载 OpenGL 函数地址
- 注册 GLFW 回调
- 把平台输入转成 Hazel 事件
- 轮询消息并交换缓冲

### 7.2 初始化顺序为什么不能错

当前实现中最关键的顺序是：

```text
glfwCreateWindow
  -> glfwMakeContextCurrent
  -> gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)
```

原因如下：

#### GLFW 负责“创建窗口 + 创建 OpenGL 上下文”
`glfwCreateWindow()` 不只是生成一个 OS 窗口，它还会附带创建 OpenGL context。

#### `glfwMakeContextCurrent()` 负责把这个 context 绑定到当前线程
OpenGL 的绝大部分函数都依赖“当前线程已经有当前上下文”。
如果上下文没绑定，就没有可供 GLAD 查询的有效函数入口。

#### GLAD 只负责“加载函数指针”，不负责创建 context
`gladLoadGLLoader()` 的本质，是通过 `glfwGetProcAddress` 把当前上下文可用的 OpenGL 函数地址解析出来。
所以如果在 `glfwMakeContextCurrent()` 之前就调用 GLAD，
通常会导致函数指针加载失败，后续任何 OpenGL 调用都不可靠。

### 7.3 回调桥接逻辑

`WindowsWindow` 当前使用 `glfwSetWindowUserPointer(m_Window, &m_Data)`
把 Hazel 自己的窗口状态挂到原生 `GLFWwindow` 上。

这样在 GLFW 的匿名回调里就能这样做：

```text
GLFW callback
  -> 取回 WindowData
  -> 构造 Hazel Event
  -> 调用 EventCallback
```

这一步的意义是：

- 平台回调只存在于平台层
- 上层永远面对 Hazel 的事件对象，而不是 GLFW 原始参数
- Application、Layer、ImGui 等系统都能消费统一事件语义

### 7.4 OnUpdate 做了什么

当前 `WindowsWindow::OnUpdate()` 主要做两件事：

1. `glfwPollEvents()`：驱动 GLFW 处理消息并触发回调
2. `glfwSwapBuffers(m_Window)`：把本帧 OpenGL 渲染结果显示到屏幕上

这意味着当前主循环里的 OpenGL 绘制，最终都要依赖这里完成呈现。

---

## 8. 事件系统

事件系统是当前工程中最完整、最有“引擎骨架感”的模块之一。

### 8.1 当前组成

目前事件系统已经包含：

- 事件基类 `Event`
- 事件类型枚举 `EventType`
- 事件分类位标志 `EventCategory`
- 事件分发器 `EventDispatcher`
- 一组具体事件类型

当前已实现的具体事件包括：

#### 窗口事件
- `WindowCloseEvent`
- `WindowResizeEvent`

#### 应用事件
- `AppTickEvent`
- `AppUpdateEvent`
- `AppRenderEvent`

#### 键盘事件
- `KeyPressedEvent`
- `KeyReleasedEvent`
- `KeyTypedEvent`

#### 鼠标事件
- `MouseMovedEvent`
- `MouseScrolledEvent`
- `MouseButtonPressedEvent`
- `MouseButtonReleasedEvent`

### 8.2 事件流转路径

```text
GLFW Callback
  -> WindowsWindow 捕获平台事件
  -> 构造 Hazel Event 对象
  -> 调用注册的 EventCallback
  -> Application::OnEvent(Event&)
  -> 先处理 Application 级事件
  -> 再由 LayerStack 从栈顶向下逆序传播
```

### 8.3 当前设计意义

- 统一事件语义
- 解耦平台层与应用层
- 为 Layer / Overlay 的事件消费顺序提供基础
- 便于日志记录和调试

---

## 9. Layer / LayerStack

相比更早阶段，当前工程已经加入 `Layer` 和 `LayerStack`。

### 9.1 当前组织方式

`LayerStack` 使用一个 `std::vector<Layer*>` 存储所有层对象，
并用 `m_LayerInsertIndex` 记录普通 Layer 与 Overlay 的分界线：

- `[0, m_LayerInsertIndex)`：普通层
- `[m_LayerInsertIndex, end())`：Overlay

### 9.2 为什么用索引而不是 iterator

当前代码已经从“缓存 `vector` iterator”改成了“缓存索引”的方式。
这是因为：

- `std::vector` 扩容后，iterator 很容易失效
- 但“插入边界是第几个位置”这个语义，用索引表达更稳定

这对于需要频繁插入普通 Layer / Overlay 的结构更安全。

### 9.3 当前注意事项

当前 `LayerStack::~LayerStack()` 会统一 `delete` 所有 layer。
因此：

- 外部缓存裸指针时，只能把它当作 non-owning 句柄使用
- `PopLayer/PopOverlay()` 当前只移除，不立即 delete
- 若后续希望在 Pop 时立即销毁，需要统一所有权策略

---

## 10. Input 系统

当前工程已经有了 `Input` 抽象与 `WindowsInput` 平台实现。

### 10.1 当前风格

输入系统当前采用“**事件 + 轮询并存**”的模式：

- 事件系统负责描述“发生了什么输入事件”
- `Input` 查询接口负责读取“当前状态是什么”

例如：

- `KeyPressedEvent` 适合事件分发
- `Input::IsKeyPressed(...)` 适合在 `OnUpdate()` 中持续查询按键状态

### 10.2 WindowsInput 的实现基础

当前 `WindowsInput` 完全基于 GLFW 的轮询接口：

- `glfwGetKey`
- `glfwGetMouseButton`
- `glfwGetCursorPos`

### 10.3 当前限制

当前所有输入查询都默认从主窗口读取。
因此在多窗口编辑器场景下，后续还需要进一步处理：

- 焦点窗口是谁
- 输入是否应被 ImGui 拦截
- 多视口状态下如何决定输入归属

---

## 11. ImGui 集成

当前工程已经集成 ImGui，并启用了：

- Keyboard Navigation
- Docking
- Multi-Viewport

### 11.1 当前职责

`ImGuiLayer` 当前负责：

- 创建 / 销毁 ImGui Context
- 初始化 `imgui_impl_glfw` backend
- 初始化 `imgui_impl_opengl3` backend
- 每帧执行 Begin / End
- 展示 DemoWindow 作为集成验证

### 11.2 Docking / Viewport 目前意味着什么

这说明项目已经从“单窗口 OpenGL 主循环”向“编辑器风格 UI 骨架”前进一步。
当前虽然还没有自己的面板系统，但底层所需的关键能力已经接通：

- 可停靠窗口
- 可拖出主窗口的子视口
- 多平台窗口渲染链路

### 11.3 Multi-Viewport 下最关键的 OpenGL 注意事项

在 `ImGuiLayer::End()` 中，当前已经做了：

```text
backup_current_context = glfwGetCurrentContext()
ImGui::UpdatePlatformWindows()
ImGui::RenderPlatformWindowsDefault()
glfwMakeContextCurrent(backup_current_context)
```

这一步非常重要。

原因是：

- 开启多视口后，ImGui 可能在渲染额外平台窗口时切换 OpenGL context
- 如果不在结束后恢复主窗口上下文
- 下一帧主窗口的 OpenGL 调用或 `glfwSwapBuffers` 可能会作用在错误的 context 上

对 **GLFW + OpenGL** 组合来说，这一步通常是必须的。

### 11.4 当前日志系统还没有直接输出到 ImGui 子视口

虽然工程已经有 ImGui Demo 窗口和 docking/viewports，
但当前 `Log::Init()` 仍然只挂了控制台 sink，日志依旧输出到 VS Code 终端或外部控制台。

如果后续希望在 ImGui 内做真正的 Log 面板，需要继续补：

- 自定义 spdlog sink
- 日志缓冲区
- ImGui 日志面板绘制逻辑

---

## 12. 日志系统与 formatter

当前日志系统基于 **spdlog**，并在这版里补齐了对自定义类型的直接输出支持。

### 12.1 当前结构

- `Log::Init()`：初始化 Core / Client logger
- `HAZEL_CORE_*`：引擎日志宏
- `HAZEL_CLIENT_*`：客户端日志宏
- `EventFormatters.h`：支持直接输出 `Hazel::Event`
- `GLMFormatters.h`：支持直接输出 `glm::vec*` / `glm::mat4`

### 12.2 当前价值

你现在已经可以直接这样写：

```cpp
HAZEL_CORE_INFO(event);
HAZEL_CLIENT_INFO(transform);
```

而不用手动 `ToString()` 或 `glm::to_string()`。

这在调试输入事件、矩阵变换和主循环状态时非常方便。

---

## 13. 当前渲染相关状态

当前工程已经能够完成最基础的 OpenGL 清屏和 ImGui 绘制提交，这说明以下链路已经打通：

- GLFW 窗口创建成功
- OpenGL 上下文建立成功
- GLAD 函数加载成功
- 主循环能够驱动 OpenGL 调用
- ImGui OpenGL3 backend 能够提交 DrawData

但需要明确的是：

> 当前仍处于“OpenGL 调用链验证已经打通，但渲染抽象尚未建立”的阶段。

当前还没有正式形成：

- `GraphicsContext`
- `Renderer`
- `RenderCommand`
- `RendererAPI`

因此这部分更适合被视为：

> **主循环 + OpenGL + ImGui 集成验证阶段**

而不是完整渲染系统。

---

## 14. 目录结构说明

当前工程目录可概括为：

```text
HazelEngine/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ Hazel/
│  ├─ CMakeLists.txt
│  ├─ Hazel.h
│  ├─ src/
│  │  ├─ Hazelpch.h
│  │  ├─ Hazelpch.cpp
│  │  ├─ Hazel/
│  │  │  ├─ Application.h / Application.cpp
│  │  │  ├─ Window.h
│  │  │  ├─ Layer.h / Layer.cpp
│  │  │  ├─ LayerStack.h / LayerStack.cpp
│  │  │  ├─ Log.h / Log.cpp
│  │  │  ├─ input.h / input.cpp
│  │  │  ├─ KeyCodes.h / MouseCodes.h
│  │  │  ├─ ImGui/
│  │  │  │  ├─ ImGuiLayer.h / ImGuiLayer.cpp
│  │  │  │  └─ ImGuiBuild.cpp
│  │  │  ├─ Events/
│  │  │  └─ SpdlogFormatters/
│  │  └─ Platform/
│  │     └─ Windows/
│  │        ├─ WindowsWindow.h / WindowsWindow.cpp
│  │        └─ WindowsInput.h / WindowsInput.cpp
│  └─ vendor/
│     ├─ glfw/
│     ├─ GLAD/
│     ├─ glm/
│     ├─ imgui/
│     └─ spdlog/
└─ Sandbox/
   ├─ CMakeLists.txt
   └─ src/
      └─ Sandbox.cpp
```

---

## 15. 构建系统说明

### 15.1 当前工具链

项目当前使用：

- **C++20**
- **CMake 3.28+**
- **MSVC / Visual Studio / VS Code + CMake Tools**
- **GLFW**
- **GLAD**
- **Dear ImGui**
- **GLM**
- **spdlog**

### 15.2 输出目录组织

当前根工程统一输出到：

- `out/bin`：动态库与可执行程序
- `out/lib`：库文件
- `out/pdb`：调试符号（MSVC）

这种方式便于集中查看构建产物。

### 15.3 当前第三方接入说明

- `GLFW`：窗口、输入回调、上下文管理
- `GLAD`：OpenGL 函数指针加载
- `Dear ImGui`：调试 UI、Docking、多视口
- `GLM`：数学类型与变换函数
- `spdlog`：日志输出

---

## 16. 构建与运行

### 16.1 环境要求

建议使用以下环境：

- Windows
- Visual Studio / MSVC
- CMake 3.28 或以上

### 16.2 配置工程

```bash
cmake --preset msvc-debug
```

### 16.3 编译 Sandbox

```bash
cmake --build out/build/msvc-debug --config Debug --target Sandbox
```

### 16.4 运行结果

当前程序运行后会：

- 初始化日志系统
- 创建 GLFW 窗口与 OpenGL context
- 使用 GLAD 加载 OpenGL 函数
- 进入主循环
- 执行基础清屏
- 接收并分发窗口/键盘/鼠标事件
- 运行 ImGui Demo 窗口
- 支持 Docking 与 Multi-Viewport 验证
- 在 Sandbox 中验证输入查询链路

---

## 17. 客户端接入方式

当前客户端接入方式仍然保持轻量：

1. 继承 `Hazel::Application`
2. 在构造函数里压入自己的 Layer
3. 实现 `Hazel::CreateApplication()`

示例：

```cpp
#include "Hazel.h"

class ExampleLayer : public Hazel::Layer
{
public:
    void OnUpdate() override
    {
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_TAB))
            HAZEL_CLIENT_TRACE("Tab key is pressed!");
    }
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
```

---

## 18. 当前架构的优点

### 18.1 主干链路已经明显比早期更完整
当前不仅有主循环和事件系统，还补齐了：

- Layer 运行结构
- ImGui 骨架
- 输入查询接口
- 常用数学类型和日志格式化能力

### 18.2 GLFW / OpenGL / GLAD 链路已经清晰可控
这对后续抽离 `GraphicsContext` 和渲染抽象非常关键。

### 18.3 平台细节被较好地限制在平台层与 ImGui backend 层
大部分上层逻辑不需要直接处理 GLFW 原生对象或 OpenGL 初始化顺序。

### 18.4 已具备“编辑器雏形”所需的 UI 基础能力
虽然还没有真正的面板系统，但 Docking 与 Viewports 已经把底层准备好了。

---

## 19. 当前存在的不足

当前工程已经具备较好的主干，但仍处在较早期阶段。

### 19.1 渲染抽象尚未建立
仍然是基础 OpenGL 清屏与 ImGui 提交，尚无 Renderer 级抽象。

### 19.2 日志系统仍然只输出到控制台
尚未接入 ImGui 日志面板 sink。

### 19.3 输入系统仍然与主窗口绑定较强
未来进入多窗口编辑器阶段，需要更细的焦点与输入路由设计。

### 19.4 LayerStack 的所有权约定仍然偏原始
当前基于裸指针 + owning container，后续可考虑更现代的生命周期管理方式。

### 19.5 更高层系统尚未建立
场景、实体、资源、时间系统、渲染抽象、编辑器面板系统都还没正式开始构建。

---

## 20. 建议的后续演进方向

结合当前实际进度，更合理的下一步通常包括：

1. **抽离 `GraphicsContext`**
   把窗口管理与图形上下文初始化拆开。

2. **建立 `Renderer` / `RenderCommand` 抽象**
   逐步把 `Application::Run()` 中的直接 OpenGL 调用移出去。

3. **补齐 `Timestep`**
   让 Layer 更新不再完全依赖帧率。

4. **补 ImGui 日志面板与调试面板体系**
   让日志、统计信息、Hierarchy/Inspector 拥有自己的面板。

5. **完善输入系统与事件吞噬策略**
   尤其是在 Docking + Viewports 场景下，理清 ImGui 与游戏层的输入优先级。

6. **引入更高层运行时系统**
   包括 Scene、Entity、资源系统和编辑器基础设施。

---

## 21. 总结

当前 Hazel Engine 已经不只是“窗口跑起来了”的阶段，而是形成了一套比较完整的早期引擎骨架：

- 有统一入口
- 有统一主循环
- 有窗口抽象层
- 有平台实现层
- 有统一事件系统
- 有 Layer / Overlay 结构
- 有输入系统
- 有 ImGui 集成
- 有日志系统与 formatter 支持
- 有清晰的构建组织方式

虽然它距离完整引擎还有很长的路要走，但从工程设计的角度看，当前阶段已经完成了最难也最关键的一步：

> **先把核心主链打通，并把容易踩坑的 GLFW / GLAD / OpenGL / ImGui 集成顺序理顺。**

在这个基础上继续长渲染系统、编辑器与场景系统，会比在零散实验代码上堆功能稳得多。
