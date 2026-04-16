# Hazel Engine v1.0.2

Hazel Engine 是一个基于 **C++20** 的早期引擎架构练习项目，当前采用 **引擎核心库（Hazel）+ 客户端示例程序（Sandbox）** 的组织方式。

和更早只有“窗口 + 事件 + 日志”的最小骨架相比，当前代码已经继续向前推进到：

- `Application` 主循环与生命周期管理
- `Window` 抽象与 `WindowsWindow` 平台实现
- GLFW 回调到 Hazel 事件系统的桥接
- `Layer` / `LayerStack`
- `Input` 抽象与 `WindowsInput` 平台实现
- ImGui 集成，支持 **Docking** 与 **Multi-Viewport**
- `spdlog + fmt` 日志系统
- `GLM` 数学库接入与自定义 formatter
- **渲染模块抽象雏形**：`RendererAPI`、`GraphicsContext`、`VertexBuffer`、`IndexBuffer`、`VertexArray`
- `Platform/OpenGL` 后端实现目录已经建立

> 当前 Hazel 的真实定位，更适合描述为：
>
> **主循环、窗口、事件、输入、ImGui 与“面向多渲染 API 的资源抽象边界”已经打通，正在从单一 OpenGL 验证阶段，过渡到可扩展渲染架构阶段的引擎实验项目。**

---

## 1. 当前进度概览

当前工程已经不再只是“能开一个 GLFW 窗口”的状态，而是具备了较完整的早期运行时主干：

```text
Sandbox
  -> 定义客户端 Layer / Application
  -> 调用 Hazel 对外暴露的公共 API

Hazel
  -> 管理应用生命周期
  -> 管理窗口与事件桥接
  -> 管理 LayerStack
  -> 管理输入查询
  -> 管理 ImGui 生命周期
  -> 管理基础渲染资源对象
  -> 通过 RendererAPI 决定具体渲染后端对象创建
```

相比V1.0.1，当前v1.0.2最值得提及的地方有：

1. **渲染相关抽象层已经开始建立**，而不再只是把 OpenGL 直接写死在窗口初始化里。
2. **`Platform/OpenGL` 目录已经承担具体后端实现**，说明代码开始把“抽象接口”和“后端落地”分层。
3. **跨平台 / 多图形 API 支持已经有了架构入口**，但目前仍处于“边界抽离完成、后端数量仍然单一”的阶段。

---

## 2. 项目定位

这个项目当前阶段的目标，不是立刻做一个功能完备的游戏引擎，而是优先解决下面几类更底层、更决定后续扩展性的工程问题：

1. **如何划分引擎与客户端的职责边界**
2. **如何建立清晰可扩展的主循环**
3. **如何把平台窗口层与图形上下文层拆开**
4. **如何为不同渲染 API 的实现预留统一抽象接口**
5. **如何为未来的编辑器、场景系统和 renderer 重构打基础**

因此当前版本更强调：

- 模块边界是否清晰
- 依赖方向是否合理
- 主循环与事件传播是否顺畅
- 平台层是否尽量被限制在 `Platform/` 目录内
- 渲染资源创建是否开始摆脱“到处直接 new OpenGL 对象”的写法

---

## 3. 当前工程结构
```text
Hazel/
├─ Hazel/
│  ├─ src/
│  │  ├─ Hazel/
│  │  │  ├─ Core/
│  │  │  │  ├─ Application.cpp
│  │  │  │  ├─ Application.h
│  │  │  │  ├─ Core.h
│  │  │  │  ├─ EntryPoint.h
│  │  │  │  ├─ Input.h
│  │  │  │  ├─ KeyCodes.h
│  │  │  │  ├─ Layer.cpp
│  │  │  │  ├─ Layer.h
│  │  │  │  ├─ LayerStack.cpp
│  │  │  │  ├─ LayerStack.h
│  │  │  │  ├─ Log.cpp
│  │  │  │  ├─ Log.h
│  │  │  │  ├─ MouseCodes.h
│  │  │  │  └─ Window.h
│  │  │  ├─ Events/
│  │  │  │  ├─ ApplicationEvent.h
│  │  │  │  ├─ Event.h
│  │  │  │  ├─ KeyEvent.h
│  │  │  │  └─ MouseEvent.h
│  │  │  ├─ ImGui/
│  │  │  │  ├─ ImGuiBuild.cpp
│  │  │  │  ├─ ImGuiLayer.cpp
│  │  │  │  └─ ImGuiLayer.h
│  │  │  ├─ Renderer/
│  │  │  │  ├─ Buffer.cpp
│  │  │  │  ├─ Buffer.h
│  │  │  │  ├─ GraphicsContext.cpp
│  │  │  │  ├─ GraphicsContext.h
│  │  │  │  ├─ Renderer.cpp
│  │  │  │  ├─ Renderer.h
│  │  │  │  ├─ Shader.cpp
│  │  │  │  ├─ Shader.h
│  │  │  │  ├─ VertexArray.cpp
│  │  │  │  └─ VertexArray.h
│  │  │  └─ SpdlogFormatters/
│  │  │     ├─ EventFormatters.h
│  │  │     └─ GLMFormatters.h
│  │  ├─ Platform/
│  │  │  ├─ OpenGL/
│  │  │  │  ├─ OpenGLBuffer.cpp
│  │  │  │  ├─ OpenGLBuffer.h
│  │  │  │  ├─ OpenGLContext.cpp
│  │  │  │  ├─ OpenGLContext.h
│  │  │  │  ├─ OpenGLShader.cpp
│  │  │  │  ├─ OpenGLShader.h
│  │  │  │  ├─ OpenGLVertexArray.cpp
│  │  │  │  └─ OpenGLVertexArray.h
│  │  │  └─ Windows/
│  │  │     ├─ WindowsInput.cpp
│  │  │     ├─ WindowsInput.h
│  │  │     ├─ WindowsWindow.cpp
│  │  │     └─ WindowsWindow.h
│  │  ├─ Hazel.h
│  │  ├─ Hazelpch.cpp
│  │  └─ Hazelpch.h
│  ├─ vendor/
│  │  ├─ GLAD/
│  │  ├─ glfw/
│  │  ├─ glm/
│  │  ├─ imgui/
│  │  └─ spdlog/
│  └─ CMakeLists.txt
├─ Sandbox/
│  ├─ src/
│  │  └─ Sandbox.cpp
│  └─ CMakeLists.txt
├─ .gitignore
├─ .gitmodules
├─ CMakeLists.txt
├─ CMakePresets.json
├─ imgui.ini
└─ README.md
```

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

从目录层面就能看出当前版本最重要的演进：

- `Hazel/Renderer/`：开始承担**渲染抽象接口层**
- `Platform/OpenGL/`：开始承担**具体图形后端实现层**
- `Platform/Windows/`：继续承担**平台窗口 / 输入实现层**

这三个层次分开之后，代码结构已经明显比“所有渲染和平台逻辑都堆在一个类里”更健康。
### 3.3 Sandbox 客户端层
`Sandbox` 是最小客户端示例程序。

它当前主要做三件事：

- 继承 `Hazel::Application`
- 在构造时压入一个 `ExampleLayer`
- 使用 `Input` 与事件系统验证引擎主链是否可用

---

## 4. 程序启动与运行流程

当前项目仍然采用“**客户端提供应用对象，引擎提供主入口**”的启动模式。

### 4.1 启动流程

```text
main
  -> Log::Init()
  -> CreateApplication()
  -> Application 构造
       -> Window::Create()
       -> WindowsWindow::Init()
       -> 创建 GraphicsContext（当前为 OpenGLContext）
       -> GraphicsContext::Init()
       -> 创建并压入 ImGuiLayer
       -> 创建 VertexArray / VertexBuffer / IndexBuffer / Shader
  -> Application::Run()
       -> 清屏与基础绘制
       -> Layer::OnUpdate()
       -> ImGui Begin / End
       -> glfwPollEvents()
       -> GraphicsContext::SwapBuffers()
  -> Application 析构
```

### 4.2 当前价值

这种结构的价值在于：

- 客户端不需要直接初始化 GLFW 或 OpenGL 上下文
- `Application` 统一控制窗口、Layer、ImGui 与渲染主循环
- 图形上下文初始化开始从 `Window` 的窗口职责中抽离为 `GraphicsContext`
- 后续加入新的渲染 API 时，不需要把所有逻辑都塞回 `Application`

---

## 5. Core 运行时模块

### 5.1 Application

`Application` 是当前运行时的总调度器，负责：

- 创建并持有主窗口
- 注册窗口事件回调
- 管理主循环
- 管理 `LayerStack`
- 驱动 `ImGuiLayer`
- 初始化当前用于验证渲染链路的基础几何与 shader

当前主循环已经可以完成：

1. 清空颜色缓冲
2. 绑定 shader / vertex array
3. 调用 `glDrawElements` 画出基础三角形
4. 更新所有 Layer
5. 驱动 ImGui 一帧
6. 轮询窗口事件并交换缓冲

这说明“窗口 -> 上下文 -> 资源 -> 绘制 -> ImGui -> Present”的整条链路已经跑通。

### 5.2 Window

`Window` 是平台窗口抽象接口，当前对外暴露：

- `OnUpdate()`
- `GetWidth()` / `GetHeight()`
- `SetEventCallback()`
- `SetVSync()` / `IsVSync()`
- `GetNativeWindow()`
- `Window::Create()` 工厂函数

这使得上层依赖的是“窗口能力”，而不是直接依赖 GLFW。

### 5.3 WindowsWindow

`WindowsWindow` 当前仍然是唯一平台实现，负责：

- 初始化 GLFW
- 创建 `GLFWwindow`
- 创建图形上下文对象
- 注册 GLFW 回调
- 将原生输入/窗口消息桥接为 Hazel 事件
- 轮询消息并交换缓冲

当前 `Window::Create()` 仍然直接返回 `WindowsWindow`，所以**平台维度上的实际支持仍然是 Windows-only**。

### 5.4 事件系统

当前事件系统已经比较完整，包含：

- `Event`
- `EventType`
- `EventCategory`
- `EventDispatcher`
- 窗口、应用、键盘、鼠标等具体事件类型

当前事件传播链：

```text
GLFW callback
  -> WindowsWindow
  -> Hazel Event
  -> Application::OnEvent()
  -> LayerStack 从顶向下逆序传播
```

Overlay / UI 优先消费事件，这对编辑器型项目是合理的基础。

### 5.5 Layer / LayerStack

`LayerStack` 已经具备普通层与 Overlay 的分区插入能力：

- 普通 Layer 插在 `m_LayerInsertIndex` 之前
- Overlay 插在容器尾部

同时当前 `LayerStack` 统一持有 `Layer*` 生命周期，这意味着：

- `Application` 中缓存的 `ImGuiLayer*` 是 non-owning 指针
- 不能再额外用另一个 owning 智能指针重复管理同一层对象

### 5.6 Input

当前输入系统采用“**事件 + 轮询**”并存的方式：

- 事件系统回答“发生了什么”
- `Input` 接口回答“当前状态是什么”

`Input` 是平台无关接口，`WindowsInput` 则通过 GLFW 轮询主窗口输入状态。

### 5.7 ImGui

当前项目已集成 ImGui，并开启：

- Keyboard Navigation
- Docking
- Multi-Viewport

这说明工程已经具备编辑器型 UI 骨架，而不是单纯的“游戏窗口 + console log”。

---

## 6.  **渲染模块抽象与跨平台支持**

这一部分是当前 README 最需要更新的核心内容。

过去的描述更接近“OpenGL 初始化验证阶段”，而从当前源码来看，项目已经开始搭建**面向多渲染 API 的抽象边界**。

不过这里必须诚实说明：

> 当前的真实状态是：
>
> **渲染抽象层已经开始形成，OpenGL 仍是唯一落地后端；跨平台/跨 API 的架构入口已经建立，但尚未实现完整切换能力。**

### 6.1 已经建立的抽象层

#### 1）`RendererAPI`

当前有一个渲染 API 枚举：

```cpp
enum class RendererAPI
{
    None,
    OpenGl, DirectX, Metal, Vulkan
};
```

这意味着项目已经不再把“底层只能是 OpenGL”写死在所有上层接口中，而是开始明确区分：

- 抽象层面对的是“当前选中的渲染 API”
- 具体对象创建由后端实现决定

虽然现在真正启用的仍然只有 `OpenGl`，但**API 选择点已经被放到了统一入口**。

#### 2）`GraphicsContext`

`GraphicsContext` 是图形上下文抽象接口：

- `Init()`
- `SwapBuffers()`

当前 `WindowsWindow` 不再把 OpenGL 上下文细节全部内联到窗口类中，而是持有一个 `GraphicsContext*`。

当前实际实例是：

- `OpenGLContext`

这一步非常关键，因为它把：

- “创建原生窗口”
- “初始化图形上下文”
- “交换显示缓冲”

从职责上拆成了两个层次。

这意味着未来如果换成：

- Vulkan context
- DirectX swap chain/context
- Metal layer/context

窗口类的改动面会更小，边界也更清晰。

#### 3）渲染资源抽象：`VertexBuffer` / `IndexBuffer` / `VertexArray`

当前以下对象已经是抽象基类：

- `VertexBuffer`
- `IndexBuffer`
- `VertexArray`

它们都通过 `Create(...)` 工厂函数根据 `Renderer::GetAPI()` 返回具体实现。

例如：

```cpp
VertexBuffer* VertexBuffer::Create(...)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::OpenGl:
            return new OpenGLVertexBuffer(...);
    }
}
```

这说明上层代码已经可以写成：

```cpp
auto vb = VertexBuffer::Create(...);
auto va = VertexArray::Create();
```

而不必直接写：

```cpp
new OpenGLVertexBuffer(...)
new OpenGLVertexArray(...)
```

这正是**跨 API 支持最基础、也最必要的一步**：

- 上层面向抽象资源类型编程
- 底层再根据当前 API 选择具体实现

#### 4）`Platform/OpenGL` 后端实现目录

当前 OpenGL 相关实现已经集中在：

- `OpenGLContext`
- `OpenGLVertexBuffer`
- `OpenGLIndexBuffer`
- `OpenGLVertexArray`

这说明项目已经有意识地把 OpenGL 细节收拢到后端目录中，而不是让 `Hazel/Renderer` 到处直接混入 OpenGL 代码。

这对未来接入 Vulkan / DirectX / Metal 的意义非常直接：

- 新增后端时，优先在 `Platform/<API>/` 下扩展
- 抽象接口层尽量少改
- 上层 Application / Layer 使用方式尽量不变

---

### 6.2 这套抽象为什么对“不同渲染 API + 跨平台支持”很关键

“跨平台”其实至少包含两个维度：

#### 维度一：操作系统平台
例如：

- Windows
- Linux
- macOS

#### 维度二：图形 API 平台
例如：

- OpenGL
- Vulkan
- DirectX
- Metal

在一个引擎里，这两类差异如果没有提前抽象，后面会非常难拆。

当前项目已经开始把这两类问题分层处理：

##### 平台窗口层
由 `Window` / `WindowsWindow` 处理：

- 原生窗口创建
- 系统消息
- 原生输入回调
- VSync / native handle

##### 图形上下文层
由 `GraphicsContext` / `OpenGLContext` 处理：

- context 初始化
- 当前线程绑定
- 函数加载
- buffer swap / present

##### 渲染资源层
由 `VertexBuffer` / `IndexBuffer` / `VertexArray` 处理：

- 顶点数据上传
- 索引数据管理
- VAO/输入布局绑定

这种拆分的好处是：

- **切换 OS 平台** 时，重点替换 `Platform/<OS>/` 的窗口与输入实现
- **切换渲染 API** 时，重点替换 `Platform/<API>/` 的 context 与渲染资源实现
- 上层 `Application`、`Layer`、客户端业务代码理论上可以尽量保持稳定

也就是说，当前代码已经从“功能验证思维”向“边界设计思维”转了一步。

---

### 6.3 当前已经做到的“抽离”与“还没有做到的抽离”

这一部分需要特别实事求是。

#### 已经做到的部分

1. **窗口对象与图形上下文开始分离**
2. **资源对象创建开始走抽象工厂**
3. **OpenGL 实现开始集中到 `Platform/OpenGL`**
4. **`RendererAPI` 已经成为后端分发入口**
5. **上层创建 `VertexBuffer` / `VertexArray` 时不再直接绑定具体 OpenGL 类型**

#### 还没有完全做到的部分

1. **`Application::Run()` 里仍然直接调用了 `glClearColor`、`glClear`、`glDrawElements`**
   - 这说明真正的绘制命令提交还没有被抽象到 `RenderCommand` 或更高层 `Renderer` 中。

2. **`Shader` 目前仍然直接依赖 OpenGL**
   - 当前 `Shader.cpp` 直接使用 `glCreateShader`、`glCompileShader`、`glLinkProgram`。
   - 也就是说，`Shader` 还没有像 `VertexBuffer` / `VertexArray` 一样完成真正的后端抽象。

3. **`Platform/OpenGL/OpenGLShader.*` 已经存在，但 shader 抽象拆分还没有完全落地**
   - 当前更像是“目录和意图已经有了，真正迁移尚未完成”。

4. **ImGui 后端仍然写死为 GLFW + OpenGL3**
   - 目前使用的是 `imgui_impl_glfw` 与 `imgui_impl_opengl3`。
   - 如果未来换成 Vulkan / DirectX / Metal，对应的 ImGui backend 也需要一起更换。

5. **平台支持仍然是 Windows-only**
   - `Core.h` 中仍直接限制为 `Hazel only supports Windows!`
   - `Window::Create()` 当前也只返回 `WindowsWindow`
   - `Hazel/CMakeLists.txt` 里仍直接链接 `opengl32`

因此，当前更准确的表述不是：

> “项目已经支持多渲染 API 和跨平台。”

而应该是：

> **项目已经开始围绕多渲染 API / 跨平台目标进行抽象设计，并且在 Context、Buffer、VertexArray 层已经有了可继续扩展的接口边界；但绘制命令、Shader、ImGui backend 和平台工厂层仍然主要是 OpenGL + Windows 定向实现。**

---

### 6.4 当前阶段最合理的架构理解

如果只看现在的代码，最合理的判断是：

#### 已经完成

- 从“只有窗口和事件”进化到“具备基本渲染资源抽象的引擎骨架”
- 从“OpenGL 初始化散落在窗口逻辑里”进化到“Context 层开始独立”
- 从“上层直接 new OpenGL 资源”进化到“通过抽象类型 + 工厂函数创建”

#### 正在进行中

- 将剩余 OpenGL 直接调用继续往后端层下沉
- 将 `Shader` 从当前 OpenGL 直写状态改造成真正的抽象接口
- 将 `Renderer` 从“保存当前 API 状态”继续扩展成真正的渲染调度入口

#### 尚未完成

- 真正意义上的 Vulkan / DirectX / Metal 后端接入
- 真正意义上的 Linux / macOS 窗口与输入实现
- 真正 API 无关的 `RenderCommand` / `Renderer` 绘制命令系统
- 场景级 renderer、相机、材质、统一 shader 管理

---

## 7. 当前渲染状态

当前项目已经可以完成最基本的图形绘制链验证：

- 创建 OpenGL context
- 通过 GLAD 加载函数地址
- 创建顶点缓冲、索引缓冲与顶点数组
- 创建并编译简单 shader
- 绘制基础三角形
- 提交 ImGui DrawData

因此当前最准确的阶段描述应该是：

> **基础渲染资源抽象已经起步，OpenGL 后端验证链路已经跑通，但高层 renderer 仍未建立。**

当前还没有正式形成的内容包括：

- `RenderCommand`
- 真正意义上的 `Renderer` 绘制接口
- API 无关的 `Shader` 工厂体系
- 统一材质/管线抽象
- 场景级渲染流程

---

## 8. 构建系统与当前平台约束

从当前可见的 CMake 配置来看，工程当前主要面向：

- **C++20**
- **CMake**
- **MSVC / Windows**
- **GLFW**
- **GLAD**
- **Dear ImGui**
- **GLM**
- **spdlog**

同时当前有几个很明确的平台约束：

1. `Core.h` 直接限制为 Windows 平台
2. `Hazel` 链接了 `opengl32`
3. `Window::Create()` 返回 `WindowsWindow`
4. 输入实现当前只有 `WindowsInput`

所以即便架构层已经开始考虑跨平台，**当前工程仍然是一个 Windows + OpenGL 的实际运行项目**。

---

## 9. 客户端接入方式

`Sandbox` 当前仍然保持轻量客户端示例的定位。

客户端接入方式：

1. 继承 `Hazel::Application`
2. 在构造函数中压入自定义 `Layer`
3. 实现 `Hazel::CreateApplication()`

示例：

```cpp
#include <Hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example") {}

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

这种组织方式保持了客户端与引擎核心的边界清晰：

- 客户端负责组合 Layer 和业务逻辑
- 引擎负责入口、主循环、事件、窗口、ImGui 和底层资源创建

---

## 10. 当前架构的主要优点

### 10.1 主干已经明显完整起来

项目已经不再是孤立实验代码，而是具备了：

- 统一入口
- 统一主循环
- 窗口抽象
- 平台实现层
- 事件系统
- Layer 结构
- 输入系统
- ImGui 集成
- 日志系统
- 初步渲染资源抽象

### 10.2 渲染模块开始具备可扩展边界

尤其是：

- `RendererAPI`
- `GraphicsContext`
- `VertexBuffer` / `IndexBuffer` / `VertexArray`
- `Platform/OpenGL`

这些组件的出现，意味着后续再做 Vulkan / DirectX / Metal 支持时，不必从零重构全部代码。

### 10.3 平台细节开始收敛

- 窗口与输入主要放在 `Platform/Windows`
- OpenGL 资源与 context 主要放在 `Platform/OpenGL`

这是跨平台工程能否继续长大的关键前提。

### 10.4 已具备编辑器型 UI 的基础条件

ImGui 的 Docking 和 Multi-Viewport 已经接通，说明后续做面板系统、调试工具和编辑器骨架会更顺畅。

---

## 11. 当前仍存在的不足

### 11.1 高层渲染抽象还不完整

目前有资源抽象，但还没有完整的：

- `RenderCommand`
- `Renderer` 调度层
- API 无关 shader 系统
- 更高层 draw call 封装

### 11.2 `Shader` 仍然是当前抽象链中的薄弱环节

它目前仍直接使用 OpenGL API，说明抽象还没有完全闭环。

### 11.3 主循环里仍存在直接 OpenGL 调用

这会让上层运行时与底层 API 继续耦合，后续应逐步下沉到 renderer 层。

### 11.4 平台支持仍只有 Windows

虽然已经有跨平台设计意识，但真正的平台实现覆盖还没展开。

### 11.5 ImGui backend 仍然绑定 GLFW + OpenGL3

这部分以后切换渲染 API 时也要一起抽象或替换。

---

## 12. 建议的后续演进方向

结合当前真实代码状态，下一阶段更合理的推进顺序通常是：

### 12.1 先补齐 `RenderCommand`

把：

- `glClearColor`
- `glClear`
- `glDrawElements`

从 `Application::Run()` 中移出去，形成 API 无关的命令接口。

### 12.2 把 `Shader` 真正抽象化

建议参考 `VertexBuffer` / `VertexArray` 的模式，改成：

- 抽象 `Shader`
- `Shader::Create(...)`
- `OpenGLShader` 负责具体实现

这样渲染资源抽象才算基本闭环。

### 12.3 完善 `Renderer`

当前 `Renderer` 还主要承担“记录当前 API 类型”的角色，后续可以继续演进为：

- renderer 初始化入口
- 提交 draw call 的统一入口
- 场景提交与批处理的挂载点

### 12.4 继续分离平台窗口与图形后端

未来如果继续做跨平台：

- `Platform/Windows`
- `Platform/Linux`
- `Platform/macOS`

未来如果继续做多图形后端：

- `Platform/OpenGL`
- `Platform/Vulkan`
- `Platform/DirectX`
- `Platform/Metal`

当前这套目录边界已经可以承载这种扩展，只是实现还没补上。

### 12.5 处理 ImGui 与输入路由问题

在 Docking + Multi-Viewport 下，需要进一步梳理：

- 输入是否先被 ImGui 消费
- 多视口平台窗口如何归属输入焦点
- 后续多窗口编辑器时输入和事件如何路由

---

## 13. 总结

当前 Hazel Engine 已经从“基础窗口/事件样例”继续发展为一套更接近真正引擎骨架的工程：

- 有统一入口
- 有统一主循环
- 有窗口抽象层
- 有平台实现层
- 有事件系统
- 有 Layer / Overlay 结构
- 有输入系统
- 有 ImGui 集成
- 有日志系统
- 更重要的是：**已经开始建立渲染抽象层与后端实现层之间的边界**

因此，当前项目最准确的描述不应该再停留在：

> “OpenGL + GLFW + ImGui 已经跑起来了。”

而应该更新为：

> **Hazel 目前已经从单纯的 OpenGL 初始化验证阶段，推进到了“为多渲染 API 与跨平台支持预留接口边界”的早期渲染架构阶段；其中 `GraphicsContext`、`RendererAPI`、`Buffer` 与 `VertexArray` 的抽象已经落地，但 `Shader`、绘制命令系统与真正的平台/后端扩展仍在后续演进范围内。**
