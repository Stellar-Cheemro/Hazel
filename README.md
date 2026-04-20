# Hazel Engine v1.0.4

Hazel Engine 是一个基于 **C++20** 的早期引擎架构练习项目，当前采用 **Hazel 引擎核心库 + Sandbox 客户端示例程序** 的双项目组织方式。  
目前仍然处于学习与验证阶段，在完成 Hazel 教学内容并补齐当前资源/渲染骨架后，后续将尝试向 **Vulkan** 或 **DirectX 12** 方向继续演进。

和更早只有“窗口 + 事件 + 日志”的最小骨架相比，当前代码已经推进到一个更接近“早期 2D Renderer + 最小 Asset System 骨架”的阶段：

- `Application` 主循环与生命周期管理
- `Window` 抽象与 `WindowsWindow` 平台实现
- GLFW 回调到 Hazel 事件系统的桥接
- `Layer` / `LayerStack`
- `Input` 抽象与 `WindowsInput` 平台实现
- `Timestep` 帧间时间步长
- ImGui 集成，支持 **Docking** 与 **Multi-Viewport**
- `spdlog + fmt` 日志系统
- `GLM` 数学库接入与格式化支持
- 渲染抽象层继续推进：
  - `RendererAPI`
  - `RenderCommand`
  - `Renderer`
  - `GraphicsContext`
  - `Shader`
  - `VertexBuffer` / `IndexBuffer` / `VertexArray`
  - `OrthographicCamera`
  - `Texture` / `Texture2D`
- `Platform/OpenGL` 后端实现目录继续扩展，已经包含：
  - `OpenGLContext`
  - `OpenGLRendererAPI`
  - `OpenGLShader`
  - `OpenGLBuffer`
  - `OpenGLVertexArray`
  - `OpenGLTexture`
- 新增最小 Asset System 骨架：
  - `RefCounted` + `Ref<T>`
  - `Project`
  - `AssetHandle`
  - `AssetType`
  - `AssetMetadata`
  - `AssetRegistry`
  - `AssetManager`
  - `AssetSerializer`
  - `TextureSerializer`
  - `TextureAsset`

> 当前更准确的定位不是“只有 OpenGL 初始化验证”，也不只是“基础 Renderer 骨架验证”，而是：
>
> **一个已经具备主循环、Layer、输入、ImGui、时间步长、正交相机、基础渲染提交流程，并开始引入最小资源管理闭环的早期引擎实验项目。**

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
       -> 配置并激活 Project
       -> AssetManager::Init()
       -> PushLayer(new ExampleLayer())
       -> ExampleLayer 创建 VertexArray / Buffer / Shader / Camera
       -> ExampleLayer 通过 AssetManager 导入并加载纹理资产
  -> Application::Run()
       -> 计算 Timestep
       -> 逐层调用 Layer::OnUpdate(timestep)
       -> ImGui Begin / 各 Layer::OnImGuiRender() / ImGui End
       -> glfwPollEvents()
       -> GraphicsContext::SwapBuffers()
  -> Application 析构
       -> AssetManager::Shutdown()
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
- 资源加载入口也开始下沉到 `AssetManager`

---

## 2. 运行效果（待补图）

下面这一节预留给程序运行效果展示，你后续可以继续补截图或 GIF。  
当前建议至少展示以下几类效果：

### 2.1 效果图 1：程序启动后的主窗口、网格化方块与纹理方块渲染效果
![202604191926](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419190726271-1504573427.png)
### 2.2 效果图 2：相机移动/视图变化效果与 ImGui 调参与运行界面
![202604192019](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419201954053-916801473.gif)
### 2.3 效果图 3：棋盘纹理加载并渲染效果
![202604210019](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260421001953899-1069420446.png)

---

## 3. 当前进度概览

当前工程已经不只是“能开一个 GLFW 窗口”，也不再只是“Application 里直接写死 OpenGL 绘制”的阶段。按照当前源码，整个运行链已经更接近下面这种结构：

```text
Sandbox
  -> 定义客户端 Layer
  -> 创建顶点数据 / 索引数据 / Shader / Camera
  -> 通过 Hazel 暴露的 Renderer / RenderCommand 提交绘制
  -> 通过 AssetManager 导入并获取纹理资源

Hazel
  -> 管理应用生命周期
  -> 管理窗口与事件桥接
  -> 管理 LayerStack
  -> 管理输入查询
  -> 管理 ImGui 生命周期
  -> 提供时间步长 Timestep
  -> 提供 RendererAPI / RenderCommand / Renderer 抽象入口
  -> 提供 VertexBuffer / IndexBuffer / VertexArray / Shader / Texture 抽象资源类型
  -> 提供 Project / AssetManager / AssetRegistry / Serializer 等最小资源系统
  -> 通过 Platform/OpenGL 提供当前唯一可用的渲染后端
```

当前值得更新的点有：

1. **`Texture` / `Texture2D` 抽象与 `OpenGLTexture2D` 实现已经接入。**
2. **最小 Asset System 已经建立，不再只是裸路径直接读取纹理。**
3. **`Project` 已用于统一资源根目录。**
4. **`AssetManager` / `TextureSerializer` / `TextureAsset` 已经跑通最小纹理闭环。**
5. **Sandbox 的演示目标已经从“网格化批量提交 + 颜色调参 + 键盘相机控制”继续推进到“纹理资源经 Asset 系统导入并参与渲染验证”。**

---

## 4. 项目定位

这个项目当前阶段的目标，仍然不是直接做一个功能完备的游戏引擎，而是持续解决几类更底层、更决定后续可扩展性的工程问题：

1. **如何划分引擎与客户端的职责边界**
2. **如何建立稳定清晰的应用主循环**
3. **如何拆分窗口系统、图形上下文和渲染提交职责**
4. **如何为不同渲染 API 的实现预留统一抽象接口**
5. **如何让资源从“磁盘路径字符串”逐步升级为“被统一身份化和管理的资产对象”**

因此当前版本最强调的不是“功能数量”，而是：

- 模块边界是否清晰
- 依赖方向是否合理
- 主循环与事件传播是否顺畅
- 平台层是否主要收敛在 `Platform/` 目录
- 客户端是否开始通过统一抽象，而不是直接散写底层 OpenGL 调用与磁盘路径访问来完成绘制

---

## 5. 当前工程结构

```text
Hazel/
├─ Hazel/
│  ├─ src/
│  │  ├─ Hazel/
│  │  │  ├─ Asset/
│  │  │  │  ├─ Asset.h
│  │  │  │  ├─ AssetExtensions.h
│  │  │  │  ├─ AssetManager.cpp
│  │  │  │  ├─ AssetManager.h
│  │  │  │  ├─ AssetMetadata.h
│  │  │  │  ├─ AssetRegistry.cpp
│  │  │  │  ├─ AssetRegistry.h
│  │  │  │  ├─ AssetSerializer.h
│  │  │  │  ├─ AssetTypes.h
│  │  │  │  ├─ TextureAsset.h
│  │  │  │  ├─ TextureSerializer.cpp
│  │  │  │  └─ TextureSerializer.h
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
│  │  │  │  ├─ Ref.h
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
│  │  │  ├─ Project/
│  │  │  │  ├─ Project.cpp
│  │  │  │  └─ Project.h
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
│  │  │  │  ├─ Texture.cpp
│  │  │  │  ├─ Texture.h
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
│  │  │  │  ├─ OpenGLTexture.cpp
│  │  │  │  ├─ OpenGLTexture.h
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
│  │  ├─ spdlog/
│  │  └─ stb_image/
│  └─ CMakeLists.txt
├─ Sandbox/
│  ├─ assets/
│  │  └─ textures/
│  │     └─ Checkerboard.png
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

### 6.1 Hazel 引擎层

`Hazel` 当前构建为一个 **共享库（DLL）**，承载引擎核心逻辑。

当前主要模块包括：

- `Application`
- `Window`
- `WindowsWindow`
- `Event` 体系
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
- `Texture` / `Texture2D`
- `OrthographicCamera`
- `Project`
- `Asset` / `AssetRegistry` / `AssetManager`
- `AssetSerializer` / `TextureSerializer`
- `EntryPoint`

从目录分层上看，当前版本最重要的演进是：

- `Hazel/Renderer/`：承担**渲染抽象接口层**
- `Hazel/Asset/`：承担**最小资源管理层**
- `Hazel/Project/`：承担**项目根与资源根目录管理**
- `Platform/OpenGL/`：承担**当前唯一图形后端实现层**
- `Platform/Windows/`：承担**平台窗口 / 输入实现层**

### 6.2 Sandbox 客户端层

`Sandbox` 当前已经不只是最小窗口启动样例，而是一个带基础相机、网格绘制与纹理资源验证逻辑的客户端示例：

- 创建方形顶点/索引资源
- 创建 shader
- 创建正交相机
- 使用键盘控制相机平移/旋转
- 使用 ImGui 调整方块颜色
- 通过 `Renderer` 提交一个 20x20 的方块网格
- 通过 `AssetManager` 导入并获取 `Checkerboard.png`
- 使用纹理 shader 对纹理方块进行采样与绘制

也就是说，现在的 Sandbox 更像是：

> **Hazel 当前渲染抽象与最小 Asset System 是否真的能驱动一小段可交互 2D 场景的联合验证程序。**

---

## 7. Core 运行时模块

### 7.1 Application

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

### 7.2 Window / WindowsWindow

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

### 7.3 事件系统

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

### 7.4 Layer / LayerStack

`LayerStack` 仍然提供普通层与 Overlay 的分区插入。

### 7.5 Input

当前输入系统仍采用“**事件 + 轮询**”并存的方式：

- 事件系统回答“发生了什么”
- `Input` 查询接口回答“当前状态是什么”

### 7.6 Timestep

`Timestep` 是一个非常轻量的时间步长封装，提供：

- 秒
- 毫秒
- 微秒
- 到 `float` 的隐式转换

### 7.7 ImGui

当前项目已集成 ImGui，并开启：

- Keyboard Navigation
- Docking
- Multi-Viewport

---

## 8. 当前渲染与资源架构进度

之前的描述更接近“Renderer 骨架推进中”；而从当前源码来看，项目已经继续推进到了：

- **有 RendererAPI**
- **有 RenderCommand**
- **有 Renderer scene 提交流程**
- **有 OrthographicCamera**
- **有 Texture / Texture2D 抽象**
- **有 OpenGLTexture2D 实现**
- **有最小 Asset System**
- **客户端已经通过 AssetManager + TextureSerializer 路径获取纹理并参与渲染**

但同时，也必须实事求是：

> 当前并不是一个抽象已经完全闭环的 renderer 或完整资产管线。
>
> **它更准确地说是：已经具备“命令层 + scene 提交层 + 相机层 + 最小纹理资产加载闭环”的早期引擎骨架，但 shader uniform、资源导入持久化、真正的后端无关资产数据层和编辑器资产工作流仍未建立。**

### 8.1 已经建立的抽象层

#### 1）`RendererAPI`

当前 `RendererAPI` 已经不只是一个枚举，而是一个抽象接口类，定义了：

- `Clear()`
- `SetClearColor()`
- `DrawIndexed()`

#### 2）`RenderCommand`

`RenderCommand` 作为命令转发层，当前负责：

- `SetClearColor()`
- `Clear()`
- `DrawIndexed()`

#### 3）`Renderer`

`Renderer` 当前已经开始承担 scene 级提交流程：

- `BeginScene(OrthographicCamera&)`
- `Submit(shader, vertexArray, modelMatrix)`
- `EndScene()`

#### 4）`OrthographicCamera`

当前支持：

- 设置投影范围
- 设置相机位置
- 设置旋转角度
- 自动重算 View / ViewProjection 矩阵

#### 5）`Texture` / `Texture2D`

当前纹理资源抽象已经接入，客户端不再需要直接构造 `OpenGLTexture2D`。

#### 6）最小 Asset System

当前已经建立：

- `Project`
- `AssetHandle`
- `AssetType`
- `AssetMetadata`
- `AssetRegistry`
- `AssetManager`
- `AssetSerializer`
- `TextureSerializer`
- `TextureAsset`

并已跑通最小闭环：

```text
ImportAsset(relativePath)
  -> AssetRegistry 注册 metadata
  -> AssetManager 查询与分发
  -> TextureSerializer 加载
  -> TextureAsset 包装 Texture2D
  -> Sandbox 获取纹理并参与渲染
```

### 8.2 当前已经做到的抽离

当前已经完成或基本完成的抽离包括：

1. **窗口对象与图形上下文分离**
2. **清屏 / DrawIndexed 已从业务层抽离到 `RenderCommand`**
3. **场景级提交入口已由 `Renderer` 承担**
4. **正交相机已从客户端逻辑中抽象成独立类型**
5. **Shader / Buffer / VertexArray / Texture 已通过工厂方法按 API 创建**
6. **OpenGL 具体实现主要被收拢到 `Platform/OpenGL`**
7. **纹理资源访问开始从“裸路径字符串”抽离到 `Project + AssetManager + Serializer` 这条链上**

### 8.3 当前还没有完全做到的抽离

#### 1）`Renderer::Submit()` 仍然显式依赖 `OpenGLShader`

当前 `Renderer.cpp` 中为了上传 uniform，会在 OpenGL 分支下把抽象 `Shader` 强转成 `OpenGLShader`。

#### 2）Sandbox 仍然显式依赖 `OpenGLShader`

当前 `Sandbox.cpp` 中，为了上传 uniform，客户端仍然知道 shader 的真实后端类型。

#### 3）Asset 系统当前仍与 Renderer 抽象层有耦合

当前 `TextureAsset` 内部持有的是 `Ref<Texture2D>`，因此当前资源系统更接近：

```text
Asset System -> Renderer Abstraction
```

而不是更彻底的：

```text
Asset System -> CPU Asset Data -> Renderer Resource
```

#### 4）Asset Registry 尚未持久化

当前 `AssetHandle` 主要还是运行时注册结果，还没有形成：

- UUID 持久化
- registry 文件
- 资源移动/重命名修复
- redirector

#### 5）还没有完整导入 / cook / package 流程

当前更接近开发期资源直读验证，而不是正式内容管线。

---

## 9. 当前渲染与资源能力：已经能做什么

基于当前 Sandbox，项目已经可以完成下面这些事情：

- 创建 OpenGL context
- 通过 GLAD 加载 OpenGL 函数
- 创建顶点缓冲、索引缓冲、顶点数组
- 创建并编译 shader
- 使用 `RenderCommand` 做清屏与索引绘制
- 使用 `Renderer` 进行 scene 提交
- 使用 `OrthographicCamera` 控制视图
- 提交一个 20x20 的方块网格
- 通过 ImGui 调整颜色参数
- 通过键盘控制相机平移和旋转
- 通过 `AssetManager` 导入 `Checkerboard.png`
- 通过 `TextureSerializer` 和 `TextureAsset` 获取纹理资源
- 将纹理资源绑定到 shader sampler 并完成纹理方块渲染验证

因此当前最准确的阶段描述可以更新为：

> **基础 2D 渲染骨架已经成形，最小纹理资源管理链路也已跑通：命令提交、scene 提交、正交相机、纹理抽象与纹理资产加载验证链路均已建立；但材质系统、统一 uniform 抽象、完整资产导入持久化、真正的后端无关资产数据层和更高层场景系统仍未建立。**

---

## 10. Sandbox 当前演示内容

当前 `Sandbox` 的 `ExampleLayer` 已经具备以下演示逻辑：

### 10.1 几何与资源

- 一个由两个三角形组成的方形
- 使用 `VertexArray + VertexBuffer + IndexBuffer + Shader` 管理资源
- 使用 `Texture2D` 纹理资源进行采样绘制

### 10.2 相机控制

当前键位：

- `Left / Right`：左右平移
- `Up / Down`：上下平移
- `Q / E`：相机旋转

### 10.3 场景提交

每帧会：

1. 设置清屏颜色并清屏
2. 更新相机位置与旋转
3. `BeginScene(camera)`
4. 循环提交 20x20 的缩放方块模型矩阵
5. 再提交一个中心纹理方块
6. `EndScene()`

### 10.4 纹理资源验证

当前纹理方块已经不再直接通过裸路径创建，而是：

1. 通过 `AssetManager::ImportAsset("textures/Checkerboard.png")`
2. 通过 `GetAsset<TextureAsset>(handle)` 获取纹理资产
3. 从 `TextureAsset` 中取出 `Texture2D`
4. 绑定到 `u_Texture` 对应采样单元
5. 通过纹理 shader 完成采样渲染

### 10.5 ImGui 调试界面

当前提供一个 `Settings` 面板，用 `ColorEdit4` 实时调整方块颜色。

---

## 11. 构建系统与当前平台约束

从当前顶层 `CMakeLists.txt` 可见，工程当前主要面向：

- **C++20**
- **CMake**
- **Windows / MSVC**
- **GLFW**
- **GLAD**
- **Dear ImGui**
- **GLM**

并且仍然采用：

- 顶层统一输出目录 `out/bin` / `out/lib`
- `Hazel` + `Sandbox` 双子项目组织
- Visual Studio 默认启动项目为 `Sandbox`

当前明确的平台约束包括：

1. 实际运行平台仍然是 Windows
2. 当前可运行图形后端仍然是 OpenGL
3. 输入实现当前只有 `WindowsInput`
4. 窗口实现当前只有 `WindowsWindow`
5. ImGui backend 当前固定使用 GLFW + OpenGL3

因此即使架构层已经朝“可扩展 renderer + 可扩展 asset system”方向前进，**当前工程的实际运行形态仍然是 Windows + OpenGL 定向实现。**

---

## 12. 当前架构的主要优点

### 12.1 主干已经更完整

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
- 最小资源系统骨架

### 12.2 渲染链条比旧版更闭合

现在已经形成了更完整的链路：

```text
Camera
  -> Renderer::BeginScene()
  -> Renderer::Submit()
  -> RenderCommand::DrawIndexed()
  -> RendererAPI
  -> OpenGLRendererAPI
```

### 12.3 资源链条也开始形成

当前已经开始形成：

```text
Project
  -> AssetManager
  -> AssetRegistry
  -> TextureSerializer
  -> TextureAsset
  -> Texture2D
  -> Render path
```

### 12.4 客户端与引擎职责更清晰

当前 `ExampleLayer` 不再直接依赖裸资源路径创建纹理，资源系统开始承担统一入口职责。

---

## 13. 当前仍存在的不足

### 13.1 `Shader` 抽象仍未真正闭环

目前 shader 的 bind/unbind 已抽象，但 uniform 上传仍依赖 `OpenGLShader`。

### 13.2 公共 API 仍泄漏具体后端类型

客户端仍然能够直接接触具体 OpenGL shader 类型。

### 13.3 `Renderer` 仍然是非常轻量的早期实现

目前它主要做：

- 缓存 VP 矩阵
- 上传变换 uniform
- 调用 DrawIndexed

还没有：

- 材质系统
- 纹理批处理
- 渲染状态管理
- RenderPass / Command Queue
- 更高层 scene renderer

### 13.4 Asset 系统仍然是最小骨架

目前主要跑通了 `Texture2D` 这一类资产，还没有：

- registry 持久化
- UUID
- 编辑器内容浏览器
- 重命名/移动修复
- 资源导入配置
- cook / package

### 13.5 当前资源系统仍偏开发期直读

当前通过项目根目录 + 相对资源路径直接读取磁盘资源，尚未形成运行时打包资源格式。

---

## 14. 建议的后续演进方向

### 14.1 先补齐 Shader 抽象

建议把下面这些能力提升到抽象 `Shader` 接口：

- `SetMat4()`
- `SetFloat4()`
- `SetInt()`

### 14.2 把 `OpenGLShader` 从公共总头文件移出

`Hazel.h` 更适合只暴露抽象层，不直接暴露 `Platform/OpenGL/*`。

### 14.3 继续扩展 `Renderer`

下一步可以考虑：

- `Renderer2D`
- 批量提交 / batching
- 更正式的纹理支持
- 更清晰的场景数据缓存结构

### 14.4 继续扩展 Asset System

下一步更合理的方向包括：

- 扩展更多 `AssetType`
- 引入更稳定的 `AssetHandle` 体系（如 UUID）
- 让 `AssetRegistry` 支持持久化
- 引入更多 serializer（如 Scene / Shader / Mesh）
- 逐步把资源系统从“Renderer 抽象绑定”推进到更清晰的数据层 / GPU 资源层分离

### 14.5 增加相机控制器或窗口自适应

当前相机操作逻辑直接写在 `ExampleLayer` 内，可以后续提炼成：

- `OrthographicCameraController`
- 窗口尺寸变化时自动更新投影

### 14.6 为多后端和跨平台继续清理边界

未来如果继续做：

- `Platform/Linux`
- `Platform/macOS`
- `Platform/Vulkan`
- `Platform/DirectX`
- `Platform/Metal`

当前目录分层已经能承载扩展，但需要把后端泄漏点继续清掉。

---

## 15. 总结

当前 Hazel Engine 已经明显从“基础窗口/事件样例工程”推进到了一个更像样的早期渲染器与资源系统骨架：

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
- 有 `Shader` / `Buffer` / `VertexArray` / `Texture` 资源抽象
- 有 `Project`
- 有 `AssetHandle`
- 有 `AssetMetadata`
- 有 `AssetRegistry`
- 有 `AssetManager`
- 有 `AssetSerializer`
- 更重要的是：**客户端已经通过这套抽象完成了一个可交互基础 2D 场景 + 最小纹理资产加载闭环的联合验证**

因此，当前项目最准确的描述，不应再停留在：

> “OpenGL + GLFW + ImGui 已经跑起来了。”

而应更新为：

> **Hazel 当前已经从单纯的 OpenGL 初始化验证阶段，推进到了具备命令层、scene 提交层、正交相机、纹理抽象以及最小 Asset System 的早期引擎架构阶段；不过 Shader uniform 抽象、公共 API 边界、资产持久化、多后端切换与跨平台支持仍处于后续演进范围内。**
