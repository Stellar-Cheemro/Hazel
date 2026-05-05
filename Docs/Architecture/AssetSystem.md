# Hazel Asset System Architecture

## Overview

Hazel Asset 系统用于统一管理 Engine Runtime、用户项目、Editor 以及后续 Asset Database 所需的资源访问、资源标识、路径解析、运行时缓存和文件加载流程。

当前 Asset 系统围绕三类资源展开：

1. **Engine Asset**
   - 引擎内部资源。
   - 例如 Renderer2D 默认 Shader、默认 Texture、编辑器图标、引擎内置材质等。
   - 由引擎内部通过 `AssetManager::ImportEngineAsset(...)` 使用。
   - Engine Asset Root 由 `AssetManager::Init()` 内部通过 `AssetRootLocator` 自动定位。

2. **Project Asset**
   - 用户项目资源。
   - 例如用户项目中的 Shader、Texture、Scene、Mesh、Audio 等。
   - 用户侧通过 `UserAssetManager::ImportProjectAsset(...)` 使用。
   - Project Asset Root 由当前激活的 `Project` 提供。

3. **Memory Asset**
   - 运行时创建、不依赖文件系统的资源。
   - 例如动态生成的 Texture、运行时 Mesh、临时 Material 等。
   - 通过 `RegisterMemoryAsset(...)` 注册。
   - 不走路径、不走 Resolver、不走 Serializer，但进入 Registry 和 RuntimeCache。

Asset 系统由若干职责明确的组件组成：

- `AssetManager` 负责流程编排；
- `AssetRegistry` 负责 Metadata 索引；
- `AssetPath` 负责逻辑路径规范化；
- `AssetSystemFileResolver` 负责真实文件系统路径解析；
- `AssetRootLocator` 负责 Engine Asset Root 自动定位；
- `AssetRuntimeCache` 负责运行时缓存；
- `AssetSerializerRegistry` 负责 Serializer 映射；
- `AssetSerializer` 负责从文件创建 Runtime Asset；
- `UserAssetManager` 负责用户侧安全访问边界。

---

## Design Goals

当前阶段的设计目标：

1. **明确 Engine / Project / Memory 三类资源边界**
   - Engine Asset 不应被用户项目直接操作。
   - Project Asset 通过用户侧门面访问。
   - Memory Asset 不参与文件路径和 Serializer 流程。

2. **保持 Metadata 稳定**
   - `AssetMetadata` 只保存稳定身份信息。
   - 不保存绝对路径。
   - 不保存加载状态。
   - 不保存运行时对象。

3. **统一 Asset 逻辑路径规则**
   - 逻辑路径统一使用 `/`。
   - 不允许绝对路径、盘符路径、URI 路径和 `..`。
   - 路径规范化只由 `AssetPath` 完成。

4. **隔离真实文件系统路径解析**
   - Engine Asset Root 和 Project Asset Root 不进入 Metadata。
   - 真实路径只在 `AssetSystemFileResolver` 边界生成。

5. **隔离用户侧与内部实现**
   - 用户项目使用 `UserAssetManager`。
   - `AssetManager`、`AssetRegistry`、`Internal/*`、`Serialization/*` 不进入用户公开总头。

6. **支持后续扩展**
   - 后续可添加 ProjectRootLocator。
   - 后续可添加 Scene / Mesh / Audio Serializer。
   - 后续可添加 Asset Database、资源重载、Asset Browser、资源打包和虚拟文件系统。

---

## Current Scope

当前支持范围：

- Asset Domain：
  - Engine
  - Project
  - Memory

- 文件型 Asset：
  - Shader
  - Texture2D

- Runtime Asset：
  - ShaderAsset
  - TextureAsset

- 内部组件：
  - AssetRootLocator
  - AssetSystemFileResolver
  - AssetHandleAllocator
  - AssetRuntimeCache
  - AssetSerializerRegistry

- 路径系统：
  - AssetPath 负责逻辑相对路径规范化。
  - AssetSystemFileResolver 负责真实路径解析。

- Renderer 集成：
  - Renderer2D 默认 FlatColor Shader 通过 Engine Asset 加载。
  - Renderer2D 不直接调用 `Shader::Create(...)` 加载默认 Shader。

当前暂不包含：

- SceneSerializer；
- MeshSerializer；
- AudioSerializer；
- 完整 Asset Database；
- Editor Asset Browser；
- 资源热重载；
- 资源依赖图；
- 资源打包；
- 虚拟文件系统；
- 跨项目 Project Root 自动定位。

---

## Directory Layout

```text
Hazel/src/Hazel/Asset/
├─ Internal/
│  ├─ AssetHandleAllocator.h/cpp
│  ├─ AssetRootLocator.h/cpp
│  ├─ AssetRuntimeCache.h/cpp
│  ├─ AssetSerializerRegistry.h/cpp
│  └─ AssetSystemFileResolver.h/cpp
├─ Runtime/
│  ├─ ShaderAsset.h
│  └─ TextureAsset.h
├─ Serialization/
│  ├─ AssetSerializer.h
│  ├─ ShaderSerializer.h/cpp
│  └─ TextureSerializer.h/cpp
├─ Asset.h
├─ AssetManager.h/cpp
├─ AssetMetadata.h
├─ AssetPath.h/cpp
├─ AssetRegistry.h/cpp
├─ AssetTypes.h
├─ UserAssetManager.h/cpp
```

---

## Core Concepts

### AssetHandle

```cpp
using AssetHandle = uint64_t;
```

`AssetHandle` 是 Asset 系统中的稳定资源标识。

Handle 由 `AssetHandleAllocator` 分配。

---

### AssetType

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

当前真正支持文件导入和 Serializer 加载的类型由：

```cpp
SupportedFileAssetTypes
```

表示。

当前支持：

```text
Texture2D
Shader
```

`Scene`、`Mesh`、`Audio` 当前是预留类型，在对应 Serializer 实现前不加入 `SupportedFileAssetTypes`。

---

### AssetDomain

```cpp
enum class AssetDomain
{
    None = 0,
    Engine,
    Project,
    Memory
};
```

Domain 决定 Asset 的来源和解析方式。

---

### AssetMetadata

`AssetMetadata` 保存资源稳定身份信息：

```cpp
AssetHandle Handle;
AssetType Type;
AssetDomain Domain;
std::string FilePath;
```

`FilePath` 是规范化后的 Asset 逻辑相对路径。

Memory Asset 的 `FilePath` 必须为空。

---

## Component Responsibilities

### AssetManager

`AssetManager` 是引擎内部 Asset 系统入口。

职责：

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

---

### UserAssetManager

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

---

### AssetRootLocator

`AssetRootLocator` 是 `Asset/Internal` 组件。

职责：

- 从当前工作目录向上查找 Engine Asset Root；
- 通过 `.hazel_engine_assets` 标记文件确认合法目录；
- 返回 Engine Asset Root 的真实文件路径。

它只由 `AssetManager` 内部使用。

---

### AssetSystemFileResolver

`AssetSystemFileResolver` 负责将 `AssetMetadata` 解析为真实文件路径。

```text
Engine Asset  -> EngineAssetRoot / FilePath
Project Asset -> Project::GetActive()->GetAssetAbsolutePath(FilePath)
Memory Asset  -> 不解析
```

---

### AssetPath

`AssetPath` 只负责 Asset 逻辑相对路径规范化。

主接口：

```cpp
AssetPath::TryNormalizeRelativePath(...)
```

它不负责真实文件系统路径解析。

---

### AssetRegistry

`AssetRegistry` 只负责 Metadata 索引。

索引方式：

```text
Handle -> AssetMetadata
Domain + NormalizedPath -> Handle
```

Memory Asset 只进入 Handle 索引，不进入 Path 索引。

---

### AssetRuntimeCache

`AssetRuntimeCache` 只负责已加载 Asset 缓存：

```text
Handle -> Ref<Asset>
```

它不负责 Metadata。

---

### AssetSerializerRegistry

`AssetSerializerRegistry` 负责：

```text
AssetType -> AssetSerializer
```

它根据 `SupportedFileAssetTypes` 检查当前支持的文件 Asset 是否都有对应 Serializer。

---

### AssetSerializer

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

---

### Runtime Asset

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

## Public API Boundary

### Engine Internal API

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

### User Project API

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

## Initialization Flow

`Application` 只负责启动子系统：

```cpp
AssetManager::Init();
Renderer2D::Init();
```

`AssetManager::Init()` 内部流程：

```text
ResetState()
SetEngineAssetRootAuto()
RegisterSerializers()
ValidateSerializers()
```

其中：

```text
SetEngineAssetRootAuto()
    AssetRootLocator::FindEngineAssetRoot(...)
    AssetSystemFileResolver::SetEngineAssetRoot(...)
```

显式初始化也可使用：

```cpp
AssetManager::Init(engineAssetRoot);
```

用于测试、工具或特殊启动流程。

`Application` 不直接调用 `AssetRootLocator`。

---

## Import Flow

### Engine Asset Import

```cpp
AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl");
```

流程：

```text
1. AssetPath 规范化相对路径
2. AssetTypes 根据扩展名识别 AssetType
3. AssetRegistry 检查是否已注册
4. AssetHandleAllocator 分配 Handle
5. AssetRegistry 注册 Metadata
6. 返回 Handle
```

### Project Asset Import

```cpp
UserAssetManager::ImportProjectAsset("Textures/Checkerboard.png");
```

流程与 Engine Asset 类似，但 Domain 为：

```text
Project
```

Project Asset 的真实路径解析依赖：

```cpp
Project::GetActive()
```

### Memory Asset Registration

```cpp
UserAssetManager::RegisterMemoryAsset(asset);
```

流程：

```text
1. 检查 asset 是否有效
2. 分配 Handle
3. 创建 Memory Metadata
4. 写入 AssetRuntimeCache
5. 写入 AssetRegistry
6. 返回 Handle
```

---

## Load Flow

```cpp
AssetManager::GetAsset(handle)
```

流程：

```text
1. 检查 Registry 是否存在
2. 检查 Handle 是否有效
3. 查询 AssetRuntimeCache
4. 如果已加载，直接返回
5. 如果未加载，读取 Metadata
6. Memory Asset 直接从 RuntimeCache 返回
7. File Asset 通过 Resolver 解析真实路径
8. 根据 AssetType 找 Serializer
9. Serializer 创建 Runtime Asset
10. 写入 AssetRuntimeCache
11. 返回 Ref<Asset>
```

---

## Domain-specific Resolution

### Engine Asset

```text
EngineAssetRoot / Metadata.FilePath
```

EngineAssetRoot 由 `AssetRootLocator` 自动定位。

当前目录：

```text
Hazel/src/Hazel/Assets/
```

需要包含标记文件：

```text
.hazel_engine_assets
```

### Project Asset

```text
Project::GetActive()->GetAssetAbsolutePath(Metadata.FilePath)
```

Project Asset 导入前必须保证当前已经存在 Active Project。

### Memory Asset

Memory Asset 不解析路径。

Memory Asset 必须满足：

```text
Domain = Memory
FilePath = ""
```

---

## Renderer2D Integration

`Renderer2D` 使用 Asset 系统加载默认 Shader：

```cpp
AssetHandle handle =
    AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl");

Ref<ShaderAsset> shaderAsset =
    AssetManager::GetAsset<ShaderAsset>(handle);
```

`Renderer2D` 不直接使用：

```cpp
Shader::Create(...)
```

这样默认 Shader 也纳入 Asset 生命周期管理。

`Renderer2D::DrawQuad()` 负责每帧提交绘制命令：

```cpp
Renderer2D::BeginScene(camera);
Renderer2D::DrawQuad(position, size, color);
Renderer2D::EndScene();
```

当前 FlatColor Shader 使用：

```glsl
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform vec4 u_Color;
```

因此 Renderer2D 上传的模型矩阵 uniform 名称应为：

```text
u_Model
```

---

## Current Engine Asset Layout

当前 Engine Asset Root：

```text
Hazel/src/Hazel/Assets/
```

内容：

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

---

## Current Project Asset Layout

当前 Sandbox Project Asset Root：

```text
Sandbox/assets/
```

内容：

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

## Logging Policy

Asset 系统错误日志统一使用：

```text
Fail:<operation>. <reason>. <context>
```

示例：

```text
Fail:import file asset. Invalid relative path. Path: ...
Fail:get asset. Invalid handle. Handle: ...
Fail:resolve engine asset path. Engine asset root is empty. Handle: ...
```

原则：

- 上层流程函数不重复报错；
- 真正失败的位置负责输出日志；
- 避免同一个错误重复输出多次。

---

## Rules

- 不允许 `AssetMetadata` 保存绝对路径。
- 不允许 `AssetMetadata` 保存运行时加载状态。
- 不允许 `MemoryAsset` 拥有 `FilePath`。
- 不允许 `AssetSerializer` 自己拼接 Engine / Project 根目录。
- 不允许 `AssetRegistry` 负责加载资源。
- 不允许 Registry key 暴露到 `AssetRegistry` 外部。
- 不允许用户项目直接使用 `AssetManager`。
- 不允许 `Application` 直接调用 `AssetRootLocator`。
- 不允许各模块各自写不同的路径规范化规则。
- 真实文件系统路径解析只应集中在 `AssetSystemFileResolver` 边界。

---

## Future Work

1. 增加 ProjectRootLocator；
2. 增加 SceneSerializer；
3. 增加 MeshSerializer；
4. 增加 AudioSerializer；
5. 增加 Asset Database；
6. 增加资源重载；
7. 增加资源引用追踪；
8. 增加 Editor Asset Browser；
9. 支持资源打包；
10. 支持虚拟文件系统。
