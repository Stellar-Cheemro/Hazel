# Hazel Engine v1.0.3

Hazel Engine 是一个基于 **C++20** 的早期引擎架构练习项目，当前采用 **Hazel 引擎核心库 + Sandbox 客户端示例程序** 的双项目组织方式。
目前只是学习阶段，在完成Hazel的已有的教学视频中的内容之后将进行集成Vulkan或者DirectX 12的尝试。

和更早只有“窗口 + 事件 + 日志”的最小骨架相比，当前代码已经推进到一个更接近“早期 2D Renderer 骨架”的阶段：

- `Application` 主循环与生命周期管理
- `Window` 抽象与 `WindowsWindow` 平台实现
- GLFW 回调到 Hazel 事件系统的桥接
- `Layer` / `LayerStack`
- `Input` 抽象与 `WindowsInput` 平台实现
- `Timestep` 帧间时间步长
- ImGui 集成，支持 **Docking** 与 **Multi-Viewport**
- `spdlog + fmt` 日志系统
- `GLM` 数学库接入与格式化支持
- 渲染抽象层的进一步推进：
  - `RendererAPI`
  - `RenderCommand`
  - `Renderer`
  - `GraphicsContext`
  - `Shader`
  - `VertexBuffer` / `IndexBuffer` / `VertexArray`
  - `OrthographicCamera`
- `Platform/OpenGL` 后端实现目录继续扩展，已经包含：
  - `OpenGLContext`
  - `OpenGLRendererAPI`
  - `OpenGLShader`
  - `OpenGLBuffer`
  - `OpenGLVertexArray`

> 当前更准确的定位不是“只有 OpenGL 初始化验证”，而是：
>
> **一个已经具备主循环、Layer、输入、ImGui、时间步长、正交相机与基础渲染提交流程的早期引擎实验项目；其中渲染抽象边界已经明显成形，但抽象尚未完全闭环。**


---

## 1. 程序启动与运行流程

当前项目仍采用“**客户端提供 Application 对象，引擎提供入口点**”的模式。

### 1.1 启动流程

```text
main
  -> Log::Init()
  -> CreateApplication()
  -> Application 构造
       -> Window::Create()
       -> WindowsWindow::Init()
       -> 创建 OpenGLContext
       -> OpenGLContext::Init()
       -> 创建并压入 ImGuiLayer
  -> Sandbox 构造
       -> PushLayer(new ExampleLayer())
       -> ExampleLayer 创建 VertexArray / Buffer / Shader / Camera
  -> Application::Run()
       -> 计算 Timestep
       -> 逐层调用 Layer::OnUpdate(timestep)
       -> ImGui Begin / 各 Layer::OnImGuiRender() / ImGui End
       -> glfwPollEvents()
       -> GraphicsContext::SwapBuffers()
  -> Application 析构
```

### 1.2 当前主循环职责

当前 `Application::Run()` 已经不再自己做具体图元绘制，而是主要承担：

1. 计算帧时间差并生成 `Timestep`
2. 驱动每个 Layer 的 `OnUpdate(timestep)`
3. 驱动 ImGui 一帧的生命周期
4. 调用窗口的 `OnUpdate()` 轮询事件与交换缓冲

这说明：

- `Application` 更像总调度器
- 具体渲染逻辑已经下沉到客户端 Layer + Renderer 抽象这一层

这是一次很关键的职责重分配。

---

## 2. 运行效果（待补图）

下面这一节预留给程序运行效果展示，你后续可以直接把截图或 GIF 补到对应位置。当前建议至少展示以下几类效果：

### 2.1 效果图 1：程序启动后的主窗口与网格化方块渲染效果

![202604191926](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419190726271-1504573427.png)

### 2.2 效果图 2：相机移动/视图变化效果与ImGui 调参与运行界面
![202604192019](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419201954053-916801473.gif)

---

## 3. 当前进度概览

当前工程已经不只是“能开一个 GLFW 窗口”，也不再只是“Application 里直接写死 OpenGL 绘制”的阶段。按照当前源码，整个运行链已经更接近下面这种结构：

```text
Sandbox
  -> 定义客户端 Layer
  -> 创建顶点数据 / 索引数据 / Shader / Camera
  -> 通过 Hazel 暴露的 Renderer / RenderCommand 提交绘制

Hazel
  -> 管理应用生命周期
  -> 管理窗口与事件桥接
  -> 管理 LayerStack
  -> 管理输入查询
  -> 管理 ImGui 生命周期
  -> 提供时间步长 Timestep
  -> 提供 RendererAPI / RenderCommand / Renderer 抽象入口
  -> 提供 VertexBuffer / IndexBuffer / VertexArray / Shader 抽象资源类型
  -> 通过 Platform/OpenGL 提供当前唯一可用的渲染后端
```

和旧版 README 相比，当前最值得更新的点有：

1. **`RenderCommand` 已经建立**，清屏和索引绘制不再直接散落在业务层。  
2. **`Renderer` 已经建立基础 scene 提交流程**，支持 `BeginScene()` / `Submit()` / `EndScene()`。  
3. **`OrthographicCamera` 已经接入**，Sandbox 不再只是静态三角形验证，而是具备相机视图变换。  
4. **`Timestep` 已加入主循环**，客户端 Layer 更新已经从“无时间语义”进化到“按帧间隔驱动”。  
5. **Sandbox 的演示目标已经从“单个图元验证”前进到“网格化批量提交 + 颜色调参 + 键盘相机控制”**。  

---

## 4. 项目定位

这个项目当前阶段的目标，仍然不是直接做一个功能完备的游戏引擎，而是持续解决几类更底层、更决定后续可扩展性的工程问题：

1. **如何划分引擎与客户端的职责边界**
2. **如何建立稳定清晰的应用主循环**
3. **如何拆分窗口系统、图形上下文和渲染提交职责**
4. **如何为不同渲染 API 的实现预留统一抽象接口**
5. **如何为后续的 2D/Scene/Editor 演进打基础**

因此当前版本最强调的不是“功能数量”，而是：

- 模块边界是否清晰
- 依赖方向是否合理
- 主循环与事件传播是否顺畅
- 平台层是否主要收敛在 `Platform/` 目录
- 客户端是否能通过统一抽象而不是直接散写底层 OpenGL 调用完成绘制

---

## 5. 当前工程结构（基于当前上传代码可确认范围）

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
│  │  │  │  ├─ Timestep.cpp
│  │  │  │  ├─ Timestep.h
│  │  │  │  └─ Window.h
│  │  │  ├─ Events/
│  │  │  │  ├─ ApplicationEvent.h
│  │  │  │  ├─ Event.h
│  │  │  │  ├─ KeyEvent.h
│  │  │  │  └─ MouseEvent.h
│  │  │  ├─ ImGui/
│  │  │  │  ├─ ImGuiBuild.cpp
│  │  │  │  ├─ ImGuiConfig.h
│  │  │  │  ├─ ImGuiLayer.cpp
│  │  │  │  └─ ImGuiLayer.h
│  │  │  ├─ Renderer/
│  │  │  │  ├─ Buffer.cpp
│  │  │  │  ├─ Buffer.h
│  │  │  │  ├─ GraphicsContext.cpp
│  │  │  │  ├─ GraphicsContext.h
│  │  │  │  ├─ OrthographicCamera.cpp
│  │  │  │  ├─ OrthographicCamera.h
│  │  │  │  ├─ RenderCommand.cpp
│  │  │  │  ├─ RenderCommand.h
│  │  │  │  ├─ Renderer.cpp
│  │  │  │  ├─ Renderer.h
│  │  │  │  ├─ RendererAPI.cpp
│  │  │  │  ├─ RendererAPI.h
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
│  │  │  │  ├─ OpenGLRendererAPI.cpp
│  │  │  │  ├─ OpenGLRendererAPI.h
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
└─ README.md

```

---

## 6. 工程整体结构

### 6.1 1 Hazel 引擎层

`Hazel` 当前构建为一个 **共享库（DLL）**，承载引擎核心逻辑。

当前主要模块包括：

- `Application`：应用主控层
- `Window`：窗口抽象接口
- `WindowsWindow`：基于 GLFW 的 Windows 平台窗口实现
- `Event` 体系：事件描述与分发
- `Layer` / `LayerStack`
- `Input` / `WindowsInput`
- `ImGuiLayer`
- `Log`
- `Timestep`
- `GraphicsContext`
- `RendererAPI`
- `RenderCommand`
- `Renderer`
- `Shader`
- `VertexBuffer` / `IndexBuffer` / `VertexArray`
- `OrthographicCamera`
- `EntryPoint`

从目录分层上看，当前版本最重要的演进是：

- `Hazel/Renderer/`：承担**渲染抽象接口层**
- `Platform/OpenGL/`：承担**当前唯一图形后端实现层**
- `Platform/Windows/`：承担**平台窗口 / 输入实现层**

这意味着代码已经不再是“所有平台与渲染逻辑混在一起”的早期试验形态。

### 6.2 2 Sandbox 客户端层

`Sandbox` 当前已经不只是最小“窗口启动样例”，而是一个带基础相机与渲染验证逻辑的客户端示例：

- 创建三角形和方形的顶点/索引资源
- 创建 shader
- 创建正交相机
- 使用键盘控制相机平移/旋转
- 使用 ImGui 调整方块颜色
- 通过 `Renderer` 批量提交一个 20x20 的方块网格

也就是说，现在的 Sandbox 更像是：

> **Hazel 当前渲染抽象接口是否真的能驱动一小段可交互 2D 场景的验证程序。**


## 7. Core 运行时模块

### 7.1 1 Application

`Application` 是当前运行时的总调度器，负责：

- 创建并持有主窗口
- 注册窗口事件回调
- 管理主循环
- 管理 `LayerStack`
- 驱动 `ImGuiLayer`
- 向 Layer 传递 `Timestep`

相比旧版，当前最明显的变化是：

- **渲染资源不再由 `Application` 构造**
- **`Application` 更专注于框架调度本身**

### 7.2 2 Window / WindowsWindow

`Window` 仍然是平台窗口抽象接口，对外暴露：

- `OnUpdate()`
- `GetWidth()` / `GetHeight()`
- `SetEventCallback()`
- `SetVSync()` / `IsVSync()`
- `GetNativeWindow()`
- `Window::Create()`

`WindowsWindow` 当前仍然是唯一平台实现，负责：

- 初始化 GLFW
- 创建 `GLFWwindow`
- 创建图形上下文对象
- 注册 GLFW 回调
- 将原生消息桥接为 Hazel 事件
- 轮询事件并交换缓冲

当前 `Window::Create()` 仍然直接返回 `WindowsWindow`，因此**实际平台支持依旧是 Windows-only**。

### 7.3 3 事件系统

当前事件系统已较完整，包含：

- `Event`
- `EventType`
- `EventCategory`
- `EventDispatcher`
- 窗口、应用、键盘、鼠标等具体事件类型

当前传播链为：

```text
GLFW callback
  -> WindowsWindow
  -> Hazel Event
  -> Application::OnEvent()
  -> LayerStack 从顶向下逆序传播
```

Overlay / UI 先消费事件，这对编辑器型架构是合理基础。

### 7.4 4 Layer / LayerStack

`LayerStack` 仍然提供普通层与 Overlay 的分区插入。

当前 `ImGuiLayer` 的生命周期由 `LayerStack` 统一管理，因此：

- `Application` 中保存的 `m_ImGuiLayer` 是 non-owning 指针
- 不能再用其他 owning 指针重复接管该对象

### 7.5 5 Input

当前输入系统仍采用“**事件 + 轮询**”并存的方式：

- 事件系统回答“发生了什么”
- `Input` 查询接口回答“当前状态是什么”

`WindowsInput` 通过 GLFW 直接查询当前主窗口的键鼠状态。

### 7.6 6 Timestep

这是当前 README 必须补上的新模块。

`Timestep` 是一个非常轻量的时间步长封装，提供：

- 秒
- 毫秒
- 微秒
- 到 `float` 的隐式转换

当前 `Application::Run()` 每帧通过 `glfwGetTime()` 计算帧间时间差，并传给各个 Layer。这样客户端更新逻辑就不再依赖“默认每帧固定速度”的假设。

### 7.7 7 ImGui

当前项目已集成 ImGui，并开启：

- Keyboard Navigation
- Docking
- Multi-Viewport

当前仍采用：

- `imgui_impl_glfw`
- `imgui_impl_opengl3`

因此 ImGui 这一层当前仍然绑定在 **GLFW + OpenGL3** 后端组合上。

---

## 8. 当前渲染架构进度（本次 README 更新核心）

这一部分是旧 README 最需要修正的内容。

之前的描述更接近“正在建立 Buffer / VertexArray 抽象，仍主要处于 OpenGL 资源验证阶段”；而从当前源码来看，项目已经继续推进到了：

- **有 RendererAPI**
- **有 RenderCommand**
- **有 Renderer scene 提交流程**
- **有 OrthographicCamera**
- **客户端已经通过 Submit 路径进行基础场景绘制**

但同时，也必须实事求是：

> 当前并不是一个抽象已经完全闭环的 renderer。
>
> **它更准确地说是：已经具备“命令层 + scene 提交层 + 相机层”的早期 Renderer 骨架，但 shader uniform、公开 API 边界和具体后端泄漏问题仍然存在。**

### 8.1 1 已经建立的抽象层

#### 1）`RendererAPI`

当前 `RendererAPI` 已经不只是一个枚举，而是一个抽象接口类，定义了：

- `Clear()`
- `SetClearColor()`
- `DrawIndexed()`

并通过静态 `s_API` 指定当前渲染后端。

当前 API 枚举包含：

- `None`
- `OpenGl`
- `DirectX`
- `Metal`
- `Vulkan`

虽然当前真正可运行的仍然只有 OpenGL，但**后端选择点已经有了正式入口**。

#### 2）`RenderCommand`

这是当前比旧版前进最明显的一步之一。

`RenderCommand` 作为命令转发层，当前负责：

- `SetClearColor()`
- `Clear()`
- `DrawIndexed()`

内部通过 `RendererAPI* s_RendererAPI = new OpenGLRendererAPI;` 绑定当前后端实现。

这意味着业务层已经不需要直接写：

```cpp
glClearColor(...);
glClear(...);
glDrawElements(...);
```

而是可以写成：

```cpp
Hazel::RenderCommand::SetClearColor(...);
Hazel::RenderCommand::Clear();
Hazel::RenderCommand::DrawIndexed(...);
```

这一步非常关键，因为它把最基础的绘制命令从业务层剥离了出来。

#### 3）`Renderer`

`Renderer` 当前已经不是单纯保存“当前 API 是什么”的占位类，而是开始承担 scene 级提交流程：

- `BeginScene(OrthographicCamera&)`
- `Submit(shader, vertexArray, modelMatrix)`
- `EndScene()`

当前它会缓存 `ViewProjectionMatrix`，并在 `Submit()` 时：

1. 绑定 shader
2. 上传 `u_ViewProjection`
3. 上传 `u_Model`
4. 绑定 vertex array
5. 调用 `RenderCommand::DrawIndexed()`

这代表项目已经开始形成一个非常基础但清晰的“scene -> submit -> command”流程。

#### 4）`OrthographicCamera`

这是旧 README 没覆盖、但当前代码已经落地的新核心模块。

`OrthographicCamera` 当前支持：

- 设置投影范围
- 设置相机位置
- 设置旋转角度
- 自动重算 View / ViewProjection 矩阵

它让 Sandbox 当前的渲染验证从“静态模型绘制”进入了“可移动视角下的 2D 场景绘制”。

#### 5）渲染资源抽象仍然成立

以下对象仍然是抽象基类，并通过工厂函数按当前 API 返回具体实现：

- `Shader`
- `VertexBuffer`
- `IndexBuffer`
- `VertexArray`

这说明上层依旧可以面向抽象资源类型编程，而不是直接 `new OpenGL...`。

#### 6）`Platform/OpenGL` 后端已经进一步成型

当前 OpenGL 后端目录已经包括：

- `OpenGLContext`
- `OpenGLRendererAPI`
- `OpenGLShader`
- `OpenGLBuffer`
- `OpenGLVertexArray`

与旧版相比，这说明 OpenGL 相关实现的集中程度进一步提升。

---

### 8.2 2 当前已经做到的抽离

当前已经完成或基本完成的抽离包括：

1. **窗口对象与图形上下文分离**
2. **清屏 / DrawIndexed 已从业务层抽离到 `RenderCommand`**
3. **场景级提交入口已由 `Renderer` 承担**
4. **正交相机已从客户端逻辑中抽象成独立类型**
5. **Shader / Buffer / VertexArray 已通过工厂方法按 API 创建**
6. **OpenGL 具体实现主要被收拢到 `Platform/OpenGL`**

这比旧版 README 所描述的“只有资源抽象雏形”又向前走了一步。

---

### 8.3 3 当前还没有完全做到的抽离

这一部分也必须写清楚，否则 README 会显得比代码更“超前”。

#### 1）`Renderer::Submit()` 仍然显式依赖 `OpenGLShader`

当前 `Renderer.cpp` 中为了上传 uniform，会在 OpenGL 分支下把抽象 `Shader` 强转成 `OpenGLShader`，再调用：

- `UploadUniformMat4("u_ViewProjection", ...)`
- `UploadUniformMat4("u_Model", ...)`

这说明：

- `Shader` 虽然已经有抽象基类和工厂函数
- 但 uniform 上传接口还没有被真正提升到 API 无关抽象层

#### 2）Sandbox 仍然显式依赖 `OpenGLShader`

在 `Sandbox.cpp` 中，为了设置方块颜色，也仍然有：

```cpp
std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->UploadUniformFloat4(...)
```

这意味着客户端代码仍然知道“当前 shader 的真实后端类型是 OpenGLShader”。

这会削弱 `Shader` 抽象的封装性。

#### 3）`Hazel.h` 仍然直接暴露 `Platform/OpenGL/OpenGLShader.h`

这也是一个非常明显的边界泄漏信号。

如果公共总头文件直接包含具体后端实现，那么：

- 客户端天然更容易直接依赖 OpenGL 细节
- 抽象层的意义会被削弱

#### 4）`RenderCommand` 当前实例绑定仍写死为 OpenGL

虽然命令抽象已经建立，但：

```cpp
RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
```

仍然是固定绑定，而不是通过完整平台/配置系统动态选择。

#### 5）ImGui backend 仍写死为 GLFW + OpenGL3

如果未来切换到 Vulkan / DirectX / Metal，对应 ImGui backend 也必须一起替换。

#### 6）平台仍然是 Windows-only

当前仍可看到：

- `Core.h` 明确限制 `Hazel only supports Windows!`
- `Window::Create()` 返回 `WindowsWindow`
- `Hazel` CMake 直接链接 `opengl32`

所以当前准确说法应当是：

> **项目已经围绕多渲染 API / 可扩展渲染架构建立了明确入口，但当前可运行形态仍然是 Windows + OpenGL 定向实现。**

---

## 9. 当前渲染能力：已经能做什么

基于当前 Sandbox，项目已经可以完成下面这些事情：

- 创建 OpenGL context
- 通过 GLAD 加载 OpenGL 函数
- 创建顶点缓冲、索引缓冲、顶点数组
- 创建并编译 shader
- 使用 `RenderCommand` 做清屏与索引绘制
- 使用 `Renderer` 进行 scene 提交
- 使用 `OrthographicCamera` 控制视图
- 提交一个 20x20 的方块网格
- 绘制一个附加三角形
- 通过 ImGui 调整颜色参数
- 通过键盘控制相机平移和旋转

因此当前最准确的阶段描述可以更新为：

> **基础 2D 渲染骨架已经成形：命令提交、scene 提交、正交相机和客户端可交互验证链路均已跑通；但材质系统、统一 uniform 抽象、真正的后端无关 renderer 和更高层场景系统仍未建立。**

---

## 10. Sandbox 当前演示内容

当前 `Sandbox` 的 `ExampleLayer` 已经具备以下演示逻辑：

### 10.1 1 几何与资源

- 一个彩色三角形
- 一个由两个三角形组成的方形
- 使用 `VertexArray + VertexBuffer + IndexBuffer + Shader` 管理资源

### 10.2 2 相机控制

当前键位：

- `Left / Right`：左右平移
- `Up / Down`：上下平移
- `Q / E`：相机旋转

### 10.3 3 场景提交

每帧会：

1. 设置清屏颜色并清屏
2. 更新相机位置与旋转
3. `BeginScene(camera)`
4. 循环提交 20x20 的缩放方块模型矩阵
5. 再提交一个三角形
6. `EndScene()`

### 10.4 4 ImGui 调试界面

当前提供一个 `Settings` 面板，用 `ColorEdit4` 实时调整方块颜色。

这说明当前 Sandbox 已经不是“窗口是否打开”的测试，而是**相机 + renderer 提交 + ImGui 联动**的综合验证场景。

---

## 11. 构建系统与当前平台约束

从当前上传的 CMake 配置可见，工程当前主要面向：

- **C++20**
- **CMake**
- **Windows / MSVC**
- **GLFW**
- **GLAD**
- **Dear ImGui**
- **GLM**
- **spdlog**

当前明确的平台约束包括：

1. `Core.h` 直接限制为 Windows
2. `Hazel` 目标直接链接 `opengl32`
3. 输入实现当前只有 `WindowsInput`
4. 窗口实现当前只有 `WindowsWindow`
5. ImGui 后端当前固定使用 GLFW + OpenGL3

所以即使架构层已经朝“可扩展 renderer”方向前进，**当前工程的实际运行平台仍然是 Windows + OpenGL**。

---

## 12. 当前架构的主要优点

### 12.1 1 主干已经更完整

当前项目已经具备：

- 统一入口
- 统一主循环
- 窗口抽象
- 平台实现层
- 事件系统
- Layer 结构
- 输入系统
- 时间步长
- ImGui 集成
- 日志系统
- 正交相机
- 命令层与 scene 提交层
- 基础渲染资源抽象

### 12.2 2 渲染链条比旧版更闭合

现在已经不是只有资源对象抽象，而是形成了更完整的链路：

```text
Camera
  -> Renderer::BeginScene()
  -> Renderer::Submit()
  -> RenderCommand::DrawIndexed()
  -> RendererAPI
  -> OpenGLRendererAPI
```

这说明抽象已经从“资源层”推进到了“绘制提交流程层”。

### 12.3 3 客户端与引擎职责更清晰

当前 `Application` 不再直接负责示例几何的创建和绘制，客户端 Layer 开始自己组织资源和场景逻辑，这一点非常重要。

### 12.4 4 已具备继续演进成 2D Renderer 的基础

有了：

- camera
- submit
- model matrix
- color uniform
- scene begin/end

后续接入更正式的 2D renderer、批处理、纹理、摄像机控制器会顺畅很多。

---

## 13. 当前仍存在的不足

### 13.1 1 `Shader` 抽象仍未真正闭环

目前 shader 的 bind/unbind 已抽象，但 uniform 上传仍严重依赖 `OpenGLShader`。

### 13.2 2 公共 API 仍泄漏具体后端类型

`Hazel.h` 暴露 `OpenGLShader.h`，说明公共接口边界还不够干净。

### 13.3 3 `Renderer` 仍然是非常轻量的早期实现

目前它主要做：

- 缓存 VP 矩阵
- 上传变换 uniform
- 调用 DrawIndexed

还没有：

- 材质系统
- 纹理系统
- 渲染状态管理
- 批处理
- RenderPass / Command Queue
- 更高层 scene renderer

### 13.4 4 窗口尺寸变化后的渲染适配还未完善

当前事件系统中已有 `WindowResizeEvent`，但渲染侧还没有看到完整的 viewport/projection 自适应链路。

### 13.5 5 平台与图形后端支持都还是单一实现

- 平台：Windows-only
- 图形后端：OpenGL-only

---

## 14. 建议的后续演进方向

结合当前代码状态，下一阶段更合理的推进顺序通常是：

### 14.1 1 先补齐 Shader 抽象

建议把下面这些能力提升到抽象 `Shader` 接口：

- `SetMat4()`
- `SetFloat4()`
- `SetInt()`
- 其他常用 uniform 设置接口

这样：

- `Renderer` 不必再强转为 `OpenGLShader`
- `Sandbox` 也不必知道后端具体类型

### 14.2 2 把 `OpenGLShader` 从公共总头文件移出

`Hazel.h` 更适合只暴露抽象层，不直接暴露 `Platform/OpenGL/*`。

### 14.3 3 继续扩展 `Renderer`

下一步可以考虑：

- `Renderer2D`
- 批量提交 / batching
- 纹理支持
- 更清晰的场景数据缓存结构

### 14.4 4 增加相机控制器或窗口自适应

当前相机操作逻辑直接写在 `ExampleLayer` 内，可以后续提炼成：

- `OrthographicCameraController`
- 窗口尺寸变化时自动更新投影

### 14.5 5 为多后端和跨平台继续清理边界

未来如果继续做：

- `Platform/Linux`
- `Platform/macOS`
- `Platform/Vulkan`
- `Platform/DirectX`
- `Platform/Metal`

当前目录分层已经能承载扩展，但需要把后端泄漏点继续清掉。

---

## 15. 总结

当前 Hazel Engine 已经明显从“基础窗口/事件样例工程”推进到了一个更像样的早期渲染器骨架：

- 有统一入口
- 有统一主循环
- 有窗口抽象层
- 有平台实现层
- 有事件系统
- 有 Layer / Overlay 结构
- 有输入系统
- 有时间步长
- 有 ImGui 集成
- 有日志系统
- 有正交相机
- 有 `RendererAPI`
- 有 `RenderCommand`
- 有 `Renderer`
- 有 `Shader` / `Buffer` / `VertexArray` 资源抽象
- 更重要的是：**客户端已经通过这套抽象完成了一个可交互的基础 2D 场景验证**

因此，当前项目最准确的描述，不应再停留在：

> “OpenGL + GLFW + ImGui 已经跑起来了。”

而应更新为：

> **Hazel 当前已经从单纯的 OpenGL 初始化验证阶段，推进到了具备命令层、scene 提交层、正交相机和客户端交互验证链路的早期渲染架构阶段；不过 Shader uniform 抽象、公共 API 边界、多后端切换与跨平台支持仍处于后续演进范围内。**
