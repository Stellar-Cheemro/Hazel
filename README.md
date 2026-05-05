# Hazel Engine v1.1.0

Hazel Engine 是一个基于 **C++20** 的早期引擎架构练习项目，当前采用 **Hazel 引擎核心库 + Sandbox 客户端示例程序** 的双项目组织方式。

项目仍然处于学习、验证和架构演进阶段。当前重点不是直接完成一个功能完备的游戏引擎，而是逐步建立一套边界清晰、可继续扩展的早期引擎骨架，包括：

- 应用生命周期与主循环；
- 窗口、事件、输入、Layer、ImGui；
- Renderer API 抽象与 OpenGL 后端；
- Renderer2D 基础绘制链路；
- Engine / Project / Memory Asset 三类资源管理；
- 用户侧 Asset API 与引擎内部 Asset 系统边界；
- 后续向 Asset Database、Editor Asset Browser、资源热重载、多后端渲染演进的基础。

当前更准确的定位是：

> **Hazel 已经从最早的 OpenGL 初始化验证阶段，推进到具备应用框架、Layer 系统、事件输入、ImGui、Renderer 抽象、Renderer2D、OpenGL 后端，以及职责拆分后的 Asset System 的早期引擎架构阶段。**

---

## 1. 当前阶段总览

当前工程已经具备以下主干能力：

- `Application` 主循环与生命周期管理；
- `Window` 抽象与 `WindowsWindow` 平台实现；
- GLFW 回调到 Hazel 事件系统的桥接；
- `Layer` / `LayerStack`；
- `Input` 抽象与 `WindowsInput` 平台实现；
- `Timestep` 帧间时间步长；
- ImGui 集成，支持 Docking 与 Multi-Viewport；
- `spdlog + fmt` 日志系统；
- `GLM` 数学库接入与格式化支持；
- Renderer 抽象层：
  - `RendererAPI`
  - `RenderCommand`
  - `Renderer`
  - `Renderer2D`
  - `GraphicsContext`
  - `Shader`
  - `VertexBuffer` / `IndexBuffer` / `VertexArray`
  - `OrthographicCamera`
  - `Texture` / `Texture2D`
- 当前 OpenGL 后端实现：
  - `OpenGLContext`
  - `OpenGLRendererAPI`
  - `OpenGLShader`
  - `OpenGLBuffer`
  - `OpenGLVertexArray`
  - `OpenGLTexture`
- 当前 Asset System：
  - `AssetManager`
  - `UserAssetManager`
  - `AssetRegistry`
  - `AssetMetadata`
  - `AssetPath`
  - `AssetTypes`
  - `AssetRuntimeCache`
  - `AssetSerializerRegistry`
  - `AssetSystemFileResolver`
  - `AssetRootLocator`
  - `AssetHandleAllocator`
  - `ShaderSerializer`
  - `TextureSerializer`
  - `ShaderAsset`
  - `TextureAsset`

---

## 2. 程序启动与运行流程

当前项目仍采用：

```text
客户端提供 Application 对象
引擎提供入口点
```

### 2.1 启动流程

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
       -> AssetManager::Init()
            -> ResetState()
            -> SetEngineAssetRootAuto()
                 -> AssetRootLocator::FindEngineAssetRoot(...)
                 -> AssetSystemFileResolver::SetEngineAssetRoot(...)
            -> RegisterSerializers()
            -> ValidateSerializers()
       -> Renderer2D::Init()
  -> Sandbox 构造
       -> PushLayer(new Sandbox2D())
  -> Application::Run()
       -> 计算 Timestep
       -> 逐层调用 Layer::OnUpdate(timestep)
       -> ImGui Begin / 各 Layer::OnImGuiRender() / ImGui End
       -> Window::OnUpdate()
            -> glfwPollEvents()
            -> GraphicsContext::SwapBuffers()
  -> Application 析构
       -> Renderer2D::Shutdown()
       -> AssetManager::Shutdown()
```

### 2.2 当前主循环职责

`Application::Run()` 主要承担：

1. 计算帧时间差并生成 `Timestep`；
2. 驱动每个 Layer 的 `OnUpdate(timestep)`；
3. 驱动 ImGui 一帧的生命周期；
4. 调用窗口的 `OnUpdate()` 轮询事件并交换缓冲。

也就是说：

- `Application` 负责应用级调度；
- 具体渲染逻辑由 Layer、Renderer、Renderer2D 承担；
- Asset 系统由 `AssetManager::Init()` 在引擎内部初始化；
- 用户侧资源访问通过 `UserAssetManager` 暴露。

---

## 3. 运行效果

### 3.1 程序启动后的主窗口、网格化方块与纹理方块渲染效果

![202604191926](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419190726271-1504573427.png)

### 3.2 相机移动 / 视图变化效果与 ImGui 调参与运行界面

![202604192019](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260419201954053-916801473.gif)

### 3.3 棋盘纹理加载并渲染效果

![202604210019](https://img2024.cnblogs.com/blog/3737081/202604/3737081-20260421001953899-1069420446.png)

> 注：截图仍可继续更新。当前代码已经进一步推进到 Renderer2D 默认 Shader 通过 Engine Asset 加载，Asset System 也已完成基础职责拆分。

---

## 4. 当前项目定位

Hazel 当前不是完整游戏引擎，而是一个持续推进的早期引擎架构实验项目。

当前阶段更关注：

1. 如何划分引擎内部 API 与用户侧 API；
2. 如何建立稳定清晰的应用主循环；
3. 如何拆分窗口系统、图形上下文和渲染提交职责；
4. 如何为不同渲染 API 的实现预留统一抽象接口；
5. 如何让资源从“磁盘路径字符串”升级为“带稳定身份和生命周期管理的 Asset”；
6. 如何把 Asset 系统内部职责从单个大 Manager 拆分为可维护的内部组件；
7. 如何让默认引擎资源也纳入统一 Asset 生命周期管理。

因此当前版本最强调的不是功能数量，而是：

- 模块边界是否清晰；
- 依赖方向是否合理；
- 主循环与事件传播是否顺畅；
- 平台层是否主要收敛在 `Platform/` 目录；
- Renderer 抽象是否逐步减少后端泄漏；
- Asset 系统是否能清楚区分 Engine / Project / Memory 三类资源；
- 用户侧是否通过安全门面访问 Asset 系统，而不是直接依赖内部实现。

---

## 5. 当前工程结构

```text
Hazel/
├─ Hazel/
│  ├─ src/
│  │  ├─ Hazel/
│  │  │  ├─ Asset/
│  │  │  │  ├─ Internal/
│  │  │  │  │  ├─ AssetHandleAllocator.h/cpp
│  │  │  │  │  ├─ AssetRootLocator.h/cpp
│  │  │  │  │  ├─ AssetRuntimeCache.h/cpp
│  │  │  │  │  ├─ AssetSerializerRegistry.h/cpp
│  │  │  │  │  └─ AssetSystemFileResolver.h/cpp
│  │  │  │  ├─ Runtime/
│  │  │  │  │  ├─ ShaderAsset.h
│  │  │  │  │  └─ TextureAsset.h
│  │  │  │  ├─ Serialization/
│  │  │  │  │  ├─ AssetSerializer.h
│  │  │  │  │  ├─ ShaderSerializer.h/cpp
│  │  │  │  │  └─ TextureSerializer.h/cpp
│  │  │  │  ├─ Asset.h
│  │  │  │  ├─ AssetManager.h/cpp
│  │  │  │  ├─ AssetMetadata.h
│  │  │  │  ├─ AssetPath.h/cpp
│  │  │  │  ├─ AssetRegistry.h/cpp
│  │  │  │  ├─ AssetTypes.h
│  │  │  │  └─ UserAssetManager.h/cpp
│  │  │  ├─ Core/
│  │  │  ├─ Events/
│  │  │  ├─ ImGui/
│  │  │  ├─ Project/
│  │  │  ├─ Renderer/
│  │  │  ├─ SpdlogFormatters/
│  │  │  └─ Assets/
│  │  │     ├─ .hazel_engine_assets
│  │  │     ├─ Shaders/
│  │  │     │  ├─ FlatColor.glsl
│  │  │     │  └─ Texture.glsl
│  │  │     └─ Textures/
│  │  │        ├─ Checkerboard.png
│  │  │        └─ ChernoLogo.png
│  │  ├─ Platform/
│  │  │  ├─ OpenGL/
│  │  │  └─ Windows/
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
│  │  ├─ Shaders/
│  │  │  ├─ FlatColor.glsl
│  │  │  └─ Texture.glsl
│  │  └─ Textures/
│  │     ├─ Checkerboard.png
│  │     └─ ChernoLogo.png
│  ├─ src/
│  │  └─ Sandbox2D.cpp
│  └─ CMakeLists.txt
├─ Docs/
│  ├─ Architecture/
│  │  └─ AssetSystem.md
│  ├─ ADR/
│  │  ├─ 0001-asset-domain-boundary.md
│  │  ├─ 0002-asset-path-policy.md
│  │  ├─ 0003-asset-manager-internal-components.md
│  │  ├─ 0004-user-asset-api-boundary.md
│  │  └─ 0005-supported-file-asset-types.md
│  └─ ChangeLog/
│     └─ asset-system-changelog.md
├─ .gitignore
├─ .gitmodules
├─ CMakeLists.txt
├─ CMakePresets.json
└─ README.md
```

---

## 6. Core 运行时模块

### 6.1 Application

`Application` 是当前运行时的总调度器，负责：

- 创建并持有主窗口；
- 注册窗口事件回调；
- 管理主循环；
- 管理 `LayerStack`；
- 驱动 `ImGuiLayer`；
- 初始化 Asset 系统与 Renderer2D；
- 向 Layer 传递 `Timestep`。

### 6.2 Window / WindowsWindow

`Window` 是平台窗口抽象接口。

`WindowsWindow` 是当前唯一平台实现，负责：

- 初始化 GLFW；
- 创建 `GLFWwindow`；
- 创建图形上下文对象；
- 注册 GLFW 回调；
- 将原生消息桥接为 Hazel 事件；
- 轮询事件并交换缓冲。

### 6.3 事件系统

当前事件传播链：

```text
GLFW callback
  -> WindowsWindow
  -> Hazel Event
  -> Application::OnEvent()
  -> LayerStack 从顶向下逆序传播
```

### 6.4 Layer / LayerStack

`LayerStack` 提供普通 Layer 与 Overlay 的分区插入。

### 6.5 Input

当前输入系统采用“事件 + 轮询”并存的方式：

- 事件系统回答“发生了什么”；
- `Input` 查询接口回答“当前状态是什么”。

### 6.6 Timestep

`Timestep` 是轻量时间步长封装，提供：

- 秒；
- 毫秒；
- 微秒；
- 到 `float` 的隐式转换。

### 6.7 ImGui

当前项目已集成 ImGui，并开启：

- Keyboard Navigation；
- Docking；
- Multi-Viewport。

---

## 7. Renderer 当前进度

当前渲染架构已经具备：

```text
RendererAPI
  -> RenderCommand
  -> Renderer / Renderer2D
  -> OpenGLRendererAPI
```

### 7.1 RendererAPI

`RendererAPI` 是渲染后端抽象接口，当前提供：

- `Clear()`
- `SetClearColor()`
- `DrawIndexed()`

### 7.2 RenderCommand

`RenderCommand` 是命令转发层，负责把高层渲染命令分发给当前 Renderer API 实现。

### 7.3 Renderer

`Renderer` 当前承担 scene 级提交基础流程：

- `BeginScene(...)`
- `Submit(...)`
- `EndScene()`

### 7.4 Renderer2D

`Renderer2D` 是当前 2D 渲染入口，已经接入 Asset 系统加载默认 Shader。

当前默认 FlatColor Shader 通过：

```cpp
AssetHandle handle =
    AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl");

Ref<ShaderAsset> shaderAsset =
    AssetManager::GetAsset<ShaderAsset>(handle);
```

`Renderer2D` 不再直接通过：

```cpp
Shader::Create(...)
```

加载默认 Shader。

### 7.5 当前 Shader Uniform

当前 FlatColor Shader 使用：

```glsl
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform vec4 u_Color;
```

因此 Renderer2D 上传模型矩阵时应使用：

```text
u_Model
```

---

## 8. Asset System 当前进度

Asset 系统是当前项目这轮重构的重点。

当前 Asset 系统围绕三类资源展开：

| 类型 | 说明 | 文件路径 | Serializer | 典型入口 |
|---|---|---:|---:|---|
| Engine Asset | 引擎内部资源，例如默认 Shader、默认 Texture、编辑器图标 | 有 | 是 | `AssetManager::ImportEngineAsset(...)` |
| Project Asset | 用户项目资源，例如用户 Shader、Texture、Scene、Mesh、Audio | 有 | 是 | `UserAssetManager::ImportProjectAsset(...)` |
| Memory Asset | 运行时创建、不依赖文件系统的资源 | 无 | 否 | `UserAssetManager::RegisterMemoryAsset(...)` |

### 8.1 AssetManager

`AssetManager` 是引擎内部 Asset 系统入口，负责流程编排：

- 初始化和关闭 Asset 系统；
- 自动定位 Engine Asset Root；
- 注册内置 Serializer；
- 导入 Engine Asset；
- 导入 Project Asset；
- 注册 Memory Asset；
- 获取 Asset；
- 卸载运行时缓存；
- 移除 Asset；
- 调用 Resolver 解析真实路径。

`AssetManager` 不直接承担：

- 路径字符串规范化细节；
- Registry key 维护；
- Serializer 映射细节；
- RuntimeCache 容器细节；
- Engine Asset Root 搜索细节；
- 具体文件资源加载细节。

### 8.2 UserAssetManager

`UserAssetManager` 是用户侧 Asset API。

用户项目应该使用：

```cpp
UserAssetManager::ImportProjectAsset(...);
UserAssetManager::RegisterMemoryAsset(...);
UserAssetManager::GetAsset<T>(...);
```

用户项目不应该直接使用：

```text
AssetManager
AssetRegistry
AssetRootLocator
AssetSystemFileResolver
AssetSerializerRegistry
AssetRuntimeCache
AssetSerializer
```

### 8.3 AssetMetadata

`AssetMetadata` 只保存稳定身份信息：

```cpp
AssetHandle Handle;
AssetType Type;
AssetDomain Domain;
std::string FilePath;
```

规则：

- 不保存绝对路径；
- 不保存根目录；
- 不保存加载状态；
- 不保存 `Ref<Asset>`；
- `FilePath` 保存规范化后的 Asset 逻辑相对路径；
- Memory Asset 的 `FilePath` 必须为空。

### 8.4 AssetPath

`AssetPath` 只负责 Asset 逻辑相对路径规范化。

主接口：

```cpp
AssetPath::TryNormalizeRelativePath(...)
```

它不负责：

- Registry key 生成；
- Engine / Project 根目录拼接；
- 文件是否存在检查；
- Asset 注册；
- Asset 加载；
- 真实文件系统路径解析。

### 8.5 AssetRegistry

`AssetRegistry` 只负责 Metadata 索引：

```text
Handle -> AssetMetadata
Domain + NormalizedPath -> Handle
```

Memory Asset 只进入 Handle 索引，不进入 Path 索引。

Registry key 是 `AssetRegistry` 的内部实现细节，不暴露给外部模块。

### 8.6 AssetSystemFileResolver

`AssetSystemFileResolver` 负责将 `AssetMetadata` 解析为真实文件路径：

```text
Engine Asset  -> EngineAssetRoot / FilePath
Project Asset -> Project::GetActive()->GetAssetAbsolutePath(FilePath)
Memory Asset  -> 不解析
```

真实文件系统路径解析集中在 `AssetSystemFileResolver` 边界。

### 8.7 AssetRootLocator

`AssetRootLocator` 是 `Asset/Internal` 组件。

职责：

- 从当前工作目录向上查找 Engine Asset Root；
- 通过 `.hazel_engine_assets` 标记文件确认合法目录；
- 返回 Engine Asset Root 的真实文件路径。

它只由 `AssetManager` 内部使用。

### 8.8 AssetRuntimeCache

`AssetRuntimeCache` 只负责已加载 Asset 缓存：

```text
Handle -> Ref<Asset>
```

它不负责 Metadata、路径解析或 Serializer 选择。

### 8.9 AssetSerializerRegistry

`AssetSerializerRegistry` 负责：

```text
AssetType -> AssetSerializer
```

它根据 `SupportedFileAssetTypes` 检查当前支持的文件 Asset 是否都有对应 Serializer。

### 8.10 AssetSerializer

`AssetSerializer` 负责把文件资源加载为 Runtime Asset。

当前内置 Serializer：

```text
ShaderSerializer
TextureSerializer
```

Serializer 通过：

```cpp
AssetManager::ResolveAssetPath(metadata)
```

获取已解析的真实路径字符串。

### 8.11 Runtime Asset

Runtime Asset 是文件资源加载完成后的运行时包装对象。

当前包括：

```text
ShaderAsset
TextureAsset
```

`ShaderAsset` 包装：

```cpp
Ref<Shader>
```

`TextureAsset` 包装：

```cpp
Ref<Texture2D>
```

Runtime Asset 不负责路径、不负责加载、不负责注册。

---

## 9. Public API Boundary

### 9.1 Engine Internal API

引擎内部模块可以使用：

```cpp
AssetManager::Init();
AssetManager::ImportEngineAsset(...);
AssetManager::ImportProjectAsset(...);
AssetManager::RegisterMemoryAsset(...);
AssetManager::GetAsset<T>(...);
```

典型使用者：

- Renderer；
- Editor；
- Engine Runtime；
- Asset 系统内部组件。

### 9.2 User Project API

用户项目应使用：

```cpp
UserAssetManager::ImportProjectAsset(...);
UserAssetManager::RegisterMemoryAsset(...);
UserAssetManager::GetAsset<T>(...);
```

`Hazel.h` 应暴露：

```cpp
#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Asset/UserAssetManager.h>
#include <Hazel/Asset/Runtime/ShaderAsset.h>
#include <Hazel/Asset/Runtime/TextureAsset.h>
```

`Hazel.h` 不应暴露：

```text
AssetManager
AssetRegistry
Asset/Internal/*
Asset/Serialization/*
```

---

## 10. 当前支持的 Asset 类型

`AssetType` 可以预留未来资源类型：

```cpp
enum class AssetType
{
    None = 0,
    Texture2D,
    Shader,
    Scene,
    Mesh,
    Audio
};
```

但当前真正支持“文件导入 + Serializer 加载”的类型是：

```text
Texture2D
Shader
```

这由：

```cpp
SupportedFileAssetTypes
```

表达。

当前暂未实现：

- SceneSerializer；
- MeshSerializer；
- AudioSerializer。

---

## 11. Engine Asset 与 Project Asset 布局

### 11.1 Engine Asset Root

当前 Engine Asset Root：

```text
Hazel/src/Hazel/Assets/
```

该目录需要包含标记文件：

```text
.hazel_engine_assets
```

示例结构：

```text
Assets/
├─ .hazel_engine_assets
├─ Shaders/
│  ├─ FlatColor.glsl
│  └─ Texture.glsl
└─ Textures/
   ├─ Checkerboard.png
   └─ ChernoLogo.png
```

### 11.2 Sandbox Project Asset Root

当前 Sandbox Project Asset Root：

```text
Sandbox/assets/
```

示例结构：

```text
assets/
├─ Shaders/
│  ├─ FlatColor.glsl
│  └─ Texture.glsl
└─ Textures/
   ├─ Checkerboard.png
   └─ ChernoLogo.png
```

Project Asset Root 由：

```cpp
ProjectConfig::ProjectDirectory
ProjectConfig::AssetDirectory
```

共同决定。

---

## 12. Sandbox 当前演示内容

当前 Sandbox 用于验证 Hazel 的基础 2D 渲染和资源系统链路。

### 12.1 当前演示重点

- 创建基础 2D 场景；
- 使用 Renderer2D 提交方块绘制；
- 使用默认 Engine Shader；
- 使用相机控制视图；
- 使用 ImGui 调整颜色或调试参数；
- 验证 Engine Asset 可以通过 Asset 系统加载；
- 验证 Project Asset 和 Memory Asset 后续可通过用户侧 API 接入。

### 12.2 相机控制

当前常见控制方式：

- `Left / Right`：左右平移；
- `Up / Down`：上下平移；
- `Q / E`：相机旋转。

### 12.3 渲染链路

典型每帧流程：

```text
Renderer2D::BeginScene(camera)
Renderer2D::DrawQuad(...)
Renderer2D::EndScene()
```

`Renderer2D::DrawQuad()` 负责上传：

```text
u_Model
u_Color
```

并提交 `DrawIndexed`。

---

## 13. 构建系统与当前平台约束

当前工程主要面向：

- C++20；
- CMake；
- Windows / MSVC；
- GLFW；
- GLAD；
- Dear ImGui；
- GLM；
- OpenGL。

当前组织方式：

- 顶层统一输出目录 `out/bin` / `out/lib`；
- `Hazel` + `Sandbox` 双子项目；
- Visual Studio 默认启动项目为 `Sandbox`。

当前平台约束：

1. 实际运行平台仍然是 Windows；
2. 当前可运行图形后端仍然是 OpenGL；
3. 输入实现当前只有 `WindowsInput`；
4. 窗口实现当前只有 `WindowsWindow`；
5. ImGui backend 当前固定使用 GLFW + OpenGL3。

因此，即使架构层已经朝“可扩展 Renderer + 可扩展 Asset System”方向推进，当前工程的实际运行形态仍然是：

```text
Windows + OpenGL
```

---

## 14. 文档索引

当前 Asset 系统相关文档：

```text
Docs/Architecture/AssetSystem.md
Docs/ADR/0001-asset-domain-boundary.md
Docs/ADR/0002-asset-path-policy.md
Docs/ADR/0003-asset-manager-internal-components.md
Docs/ADR/0004-user-asset-api-boundary.md
Docs/ADR/0005-supported-file-asset-types.md
Docs/ChangeLog/asset-system-changelog.md
```

建议阅读顺序：

1. `Docs/Architecture/AssetSystem.md`
2. `Docs/ADR/0001-asset-domain-boundary.md`
3. `Docs/ADR/0002-asset-path-policy.md`
4. `Docs/ADR/0003-asset-manager-internal-components.md`
5. `Docs/ADR/0004-user-asset-api-boundary.md`
6. `Docs/ADR/0005-supported-file-asset-types.md`
7. `Docs/ChangeLog/asset-system-changelog.md`

---

## 15. 当前架构的主要优点

### 15.1 主干已经更完整

当前项目已经具备：

- 统一入口；
- 统一主循环；
- 窗口抽象；
- 平台实现层；
- 事件系统；
- Layer 结构；
- 输入系统；
- 时间步长；
- ImGui 集成；
- 日志系统；
- 正交相机；
- RendererAPI；
- RenderCommand；
- Renderer / Renderer2D；
- 基础渲染资源抽象；
- Engine / Project / Memory Asset 管理基础。

### 15.2 渲染链条更闭合

当前渲染链路已经形成：

```text
Camera
  -> Renderer2D::BeginScene()
  -> Renderer2D::DrawQuad()
  -> Renderer2D::EndScene()
  -> RenderCommand::DrawIndexed()
  -> RendererAPI
  -> OpenGLRendererAPI
```

### 15.3 资源链条职责更清晰

当前 Asset 系统已经形成：

```text
AssetPath
  -> AssetRegistry
  -> AssetSystemFileResolver
  -> AssetSerializerRegistry
  -> AssetSerializer
  -> Runtime Asset
  -> AssetRuntimeCache
```

### 15.4 用户侧与引擎内部边界更清晰

用户项目通过：

```cpp
UserAssetManager
```

访问允许的 Project / Memory Asset 能力。

引擎内部通过：

```cpp
AssetManager
```

访问完整 Asset 系统能力。

---

## 16. 当前仍存在的不足

### 16.1 Renderer 仍处于早期阶段

当前还没有完整的：

- 材质系统；
- 纹理批处理；
- 渲染状态管理；
- RenderPass；
- Command Queue；
- 更完整的 Scene Renderer。

### 16.2 Shader 抽象仍需要继续完善

当前仍需要继续收敛：

- uniform 设置接口；
- shader 反射；
- 材质参数绑定；
- 后端差异封装。

### 16.3 Asset 系统仍未进入完整编辑器资产数据库阶段

当前还没有：

- registry 文件持久化；
- UUID 持久化；
- 资源移动 / 重命名修复；
- redirector；
- Editor Asset Browser；
- 资源依赖图；
- 资源导入配置；
- cook / package 流程。

### 16.4 当前资源系统仍偏开发期直读

当前 Engine / Project 资源仍然通过逻辑相对路径解析到真实磁盘文件。

后续需要继续演进到：

```text
Asset Database
Asset Importer
Asset Browser
Cook / Package
Virtual File System
```

---

## 17. 建议的后续演进方向

### 17.1 继续完善 Renderer2D

可以继续加入：

- batching；
- texture slots；
- statistics；
- camera controller；
- window resize 自适应；
- 更完整的 2D API。

### 17.2 继续完善 Shader 抽象

建议逐步补齐：

- `SetMat4()`
- `SetFloat4()`
- `SetInt()`
- uniform cache
- backend-independent shader interface

### 17.3 继续扩展 Asset System

后续合理方向：

- 完善 ProjectAsset 解析链路；
- 增加 ProjectRootLocator；
- 增加 SceneSerializer；
- 增加 MeshSerializer；
- 增加 AudioSerializer；
- 增加 Asset Database；
- 增加资源热重载；
- 增加资源引用追踪；
- 增加 Editor Asset Browser。

### 17.4 清理公共 API 边界

继续确保：

- `Hazel.h` 暴露用户侧安全 API；
- `Asset/Internal/*` 不泄露到用户项目；
- `Asset/Serialization/*` 不泄露到用户项目；
- `Platform/OpenGL/*` 不作为通用用户 API 暴露。

### 17.5 为多后端和跨平台继续清理边界

未来如继续支持：

- `Platform/Linux`
- `Platform/macOS`
- `Platform/Vulkan`
- `Platform/DirectX`
- `Platform/Metal`

需要继续减少 OpenGL 具体类型在公共层和客户端层的泄漏。

---

## 18. 总结

当前 Hazel Engine 已经明显从“基础窗口 / 事件样例工程”推进到了更完整的早期引擎架构阶段。

当前已经具备：

- 应用入口与主循环；
- 窗口抽象与 Windows 实现；
- OpenGL 图形上下文；
- 事件系统；
- Layer / Overlay；
- 输入系统；
- 时间步长；
- ImGui；
- 日志；
- 正交相机；
- RendererAPI；
- RenderCommand；
- Renderer / Renderer2D；
- Shader / Buffer / VertexArray / Texture 资源抽象；
- Engine / Project / Memory Asset 三类资源模型；
- AssetPath；
- AssetRegistry；
- AssetManager；
- UserAssetManager；
- AssetRootLocator；
- AssetSystemFileResolver；
- AssetRuntimeCache；
- AssetSerializerRegistry；
- ShaderSerializer / TextureSerializer；
- ShaderAsset / TextureAsset；
- Renderer2D 通过 Engine Asset 加载默认 Shader。

因此当前项目最准确的描述是：

> **Hazel 当前已经具备应用框架、基础渲染抽象、Renderer2D 和拆分后的 Asset System。它仍然是早期实验项目，但已经开始围绕清晰的模块边界、资源生命周期、用户侧 API 边界和后续编辑器资产工作流进行演进。**
