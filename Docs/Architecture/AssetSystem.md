# Hazel Asset System Architecture

## Overview

Hazel Asset 系统负责统一管理资源标识、路径规范化、真实路径解析、运行时缓存、Serializer 加载和用户侧资源访问。

当前 Asset 系统分为三类资源：

1. **Engine Asset**
   - 引擎内置资源，例如默认 Shader、默认 Texture、编辑器图标、内置材质等。
   - 由引擎内部通过 `AssetManager::ImportEngineAsset(...)` 注册。
   - 用户可以使用公开 Engine Asset，但不通过路径任意导入 Engine Asset。
   - 用户通过 `EngineAssets` catalog 获取公开 Engine Asset 的实际运行时产物。

2. **Project Asset**
   - 用户项目资源，例如项目贴图、项目 Shader、Scene、Mesh、Audio 等。
   - 用户通过 `UserAssetManager::ImportProjectAsset(...)` 导入。
   - 真实路径由当前 active `Project` 提供。

3. **Memory Asset**
   - 运行时创建、不依赖文件系统的资源。
   - 用户通过 `UserAssetManager::RegisterMemoryShader(...)`、`UserAssetManager::RegisterMemoryTexture2D(...)` 等接口注册实际运行时产物。
   - 不走路径、不走 Resolver、不走 Serializer，但进入 Registry 和 RuntimeCache。

## Core Concepts

### AssetHandle

```cpp
using AssetHandle = uint64_t;
```

`AssetHandle` 是 Asset 系统中的稳定资源标识。

规则：

- `0` 表示无效 handle。
- handle 由 `AssetHandleAllocator` 分配。
- handle 用于查询 Metadata 和 RuntimeCache 中的资源。
- 用户侧可以持有 handle。
- 加载函数不在函数名中区分 Engine / Project / Memory，因为 handle 已经唯一指向对应 Metadata。

### AssetMetadata

`AssetMetadata` 只保存资源稳定身份信息：

```cpp
struct AssetMetadata
{
    AssetHandle Handle = 0;
    AssetType Type = AssetType::None;
    AssetDomain Domain = AssetDomain::None;
    std::string FilePath;
};
```

`AssetMetadata` 不保存：

```text
真实文件系统绝对路径
加载状态
运行时对象
Registry key
Engine / Project 根目录
Serializer 状态
```

### AssetType

```cpp
enum class AssetType : uint8_t
{
    None = 0,

    Texture2D,
    Shader,
    Scene,
    Mesh,
    Audio
};
```

当前完整支持 Serializer 加载的文件资源类型由 `SupportedFileAssetTypes` 表达：

```text
Texture2D
Shader
```

当前可作为预留类型存在，但未完整支持加载：

```text
Scene
Mesh
Audio
```

## Main Components

```text
AssetManager
    引擎内部 Asset 系统入口。
    负责编排 Init、Import、Restore、RegisterMemory、GetAsset、Unload、Remove。

EngineAssets
    用户可见的公开 Engine Asset catalog。
    隐藏 EngineAsset 内部路径。
    返回 Shader / Texture2D 等实际运行时产物。

UserAssetManager
    用户侧 Project / Memory 资源门面。
    导入 ProjectAsset，注册 MemoryAsset，按 handle 加载实际产物。

AssetRegistry
    只负责 Metadata 索引。

AssetPath
    只负责逻辑相对路径规范化。

AssetSystemFileResolver
    只负责 AssetMetadata -> 真实文件系统路径解析和文件存在性校验。

AssetRuntimeCache
    保存 Handle -> Ref<Asset> 的已加载内部 wrapper。

AssetSerializerRegistry
    保存 AssetType -> AssetSerializer 映射。

AssetSerializer
    文件 -> Runtime Asset wrapper。

ShaderAsset / TextureAsset
    Asset 系统内部 Runtime Asset wrapper。
    用户侧不直接使用。
```

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
├─ EngineAssets.h/cpp
└─ UserAssetManager.h/cpp
```

## Component Responsibilities

### AssetManager

`AssetManager` 是引擎内部 Asset 系统入口。

职责：

- 初始化和关闭 Asset 系统。
- 重置 Registry、RuntimeCache、SerializerRegistry、Resolver、HandleAllocator。
- 自动定位或显式配置 Engine Asset Root。
- 注册和校验内置 Serializer。
- 导入 Engine Asset。
- 导入 Project Asset。
- 恢复已保存的 File Asset Metadata。
- 注册 Memory Asset 内部 wrapper。
- 加载内部 Runtime Asset wrapper。
- 卸载文件资源运行时缓存。
- 移除资源注册记录。
- 调用 `AssetSystemFileResolver` 解析真实路径。

不负责：

- 不作为用户侧 API。
- 不暴露 Registry key。
- 不定义路径规范化规则。
- 不实现具体文件资源加载。
- 不删除磁盘文件。
- 不负责公开 EngineAsset catalog。

### EngineAssets

`EngineAssets` 是公开 Engine Asset catalog。

职责：

- 在引擎启动阶段注册公开 Engine Asset。
- 隐藏 Engine Asset 内部路径。
- 保存公开 Engine Asset 的 handle。
- 通过枚举暴露当前公开的 Engine Shader / Engine Texture。
- 返回 `Shader` / `Texture2D` 等实际运行时产物。

当前阶段使用枚举作为 catalog：

```cpp
enum class EngineShader
{
    FlatColor,
    Texture,
    Count
};

enum class EngineTexture
{
    Checkerboard,
    ChernoLogo,
    Count
};
```

典型使用：

```cpp
Ref<Shader> shader =
    EngineAssets::GetShader(EngineShader::FlatColor);

Ref<Texture2D> texture =
    EngineAssets::GetTexture2D(EngineTexture::Checkerboard);
```

### UserAssetManager

`UserAssetManager` 是用户侧 Project / Memory 资源门面，并提供基于明确 handle 的实际产物加载接口。

职责：

```text
ProjectAsset:
    ImportProjectAsset(path)

MemoryAsset:
    RegisterMemoryShader(shader)
    RegisterMemoryTexture2D(texture)

Load:
    LoadShader(handle)
    LoadTexture2D(handle)

Lifecycle:
    TryUnloadProjectAsset(handle)
    TryRemoveProjectAsset(handle)
    TryRemoveMemoryAsset(handle)
```

不负责：

- 不暴露 EngineAsset 路径导入接口。
- 不暴露 `GetAsset<TWrapper>()`。
- 不暴露 `ShaderAsset` / `TextureAsset`。
- 不暴露真实文件系统路径。
- 不暴露可修改 Metadata。

### AssetRegistry

`AssetRegistry` 只负责 Metadata 索引。

```text
Handle -> AssetMetadata
Domain + NormalizedPath -> AssetHandle
```

### AssetPath

`AssetPath` 只负责 Asset 逻辑相对路径规范化。

主接口：

```cpp
AssetPath::TryNormalizeRelativePath(...)
```

### AssetSystemFileResolver

`AssetSystemFileResolver` 负责把文件资源 Metadata 解析为真实文件系统路径。

```text
Engine Asset:
    EngineAssetRoot / Metadata.FilePath

Project Asset:
    Project::GetActive()->GetAssetAbsolutePath(Metadata.FilePath)

Memory Asset:
    不解析
```

### Runtime Asset Wrapper

当前内部 wrapper：

```text
ShaderAsset
TextureAsset
```

映射：

```text
ShaderAsset:
    Ref<Shader>

TextureAsset:
    Ref<Texture2D>
```

用户侧 API 拆 wrapper 并返回实际运行时产物。

## Public API Boundary

### Engine Internal API

```cpp
AssetManager::Init();
AssetManager::ImportEngineAsset(...);
AssetManager::ImportProjectAsset(...);
AssetManager::RegisterMemoryAsset(...);
AssetManager::GetAsset<TAssetWrapper>(...);
AssetManager::ResolveAssetPath(...);
AssetManager::TryUnloadAsset(...);
AssetManager::TryRemoveAsset(...);
```

### User Project API

```cpp
EngineAssets::GetShader(...);
EngineAssets::GetTexture2D(...);
EngineAssets::GetShaderHandle(...);
EngineAssets::GetTexture2DHandle(...);

UserAssetManager::ImportProjectAsset(...);
UserAssetManager::LoadShader(...);
UserAssetManager::LoadTexture2D(...);
UserAssetManager::RegisterMemoryShader(...);
UserAssetManager::RegisterMemoryTexture2D(...);
UserAssetManager::IsAssetHandleValid(...);
UserAssetManager::IsAssetLoaded(...);
UserAssetManager::TryUnloadProjectAsset(...);
UserAssetManager::TryRemoveProjectAsset(...);
UserAssetManager::TryRemoveMemoryAsset(...);
```

`Hazel.h` 应暴露：

```cpp
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Asset/UserAssetManager.h>
#include <Hazel/Asset/EngineAssets.h>

#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/Texture.h>
```

`Hazel.h` 不应暴露：

```text
AssetManager
AssetRegistry
Asset/Internal/*
Asset/Serialization/*
Asset/Runtime/ShaderAsset.h
Asset/Runtime/TextureAsset.h
```

## Initialization Flow

启动顺序：

```cpp
AssetManager::Init();
EngineAssets::Init();
Renderer2D::Init();
```

关闭顺序：

```cpp
Renderer2D::Shutdown();
EngineAssets::Shutdown();
AssetManager::Shutdown();
```

## Import Flow

### Engine Asset Import

Engine Asset 导入是内部行为。

典型入口：

```cpp
EngineAssets::Init()
    -> AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl")
```

流程：

```text
1. 检查 Registry 是否初始化。
2. 检查 Domain 是否为 Engine。
3. 使用 AssetPath 规范化相对路径。
4. 检查 Domain + Path 是否已注册。
5. 根据扩展名判断 AssetType。
6. 分配 handle。
7. 构造 AssetMetadata。
8. 通过 AssetSystemFileResolver 解析真实路径。
9. 校验文件存在。
10. 注册 Metadata。
11. 将 handle 保存到 EngineAssets catalog。
```

### Project Asset Import

```cpp
AssetHandle handle =
    UserAssetManager::ImportProjectAsset("Textures/Wood.png");
```

### Memory Asset Registration

```cpp
UserAssetManager::RegisterMemoryShader(shader);
UserAssetManager::RegisterMemoryTexture2D(texture);
```

## Load Flow

### User-facing Load

```cpp
UserAssetManager::LoadShader(handle);
UserAssetManager::LoadTexture2D(handle);
```

流程：

```text
1. 根据 handle 读取 Metadata。
2. 检查 AssetType 是否匹配请求的产物类型。
3. 调用 AssetManager::GetAsset<ShaderAsset>() 或 GetAsset<TextureAsset>()。
4. 从 wrapper 取出实际产物。
5. 返回 Ref<Shader> 或 Ref<Texture2D>。
```

加载函数不区分 Engine / Project / Memory，因为 `AssetHandle` 已经唯一指向 Metadata，Metadata 内部包含 Domain。

## User Access Patterns

### Engine Asset

```cpp
Ref<Shader> shader =
    EngineAssets::GetShader(EngineShader::FlatColor);
```

### Project Asset

```cpp
AssetHandle handle =
    UserAssetManager::ImportProjectAsset("Textures/Wood.png");

Ref<Texture2D> texture =
    UserAssetManager::LoadTexture2D(handle);
```

### Memory Asset

```cpp
AssetHandle handle =
    UserAssetManager::RegisterMemoryTexture2D(texture);

Ref<Texture2D> loaded =
    UserAssetManager::LoadTexture2D(handle);
```

## Search and Discovery Policy

运行时加载必须明确。

Hazel 不使用以下行为作为默认运行时加载策略：

```text
Load("Checkerboard") -> 搜索 Engine -> Project -> Memory
```

发现能力应属于 catalog / editor / asset database 层。

## Renderer2D Integration

Renderer2D 通过 `EngineAssets` 获取默认 Shader：

```cpp
s_Data->FlatColorShader =
    EngineAssets::GetShader(EngineShader::FlatColor);

s_Data->TextureShader =
    EngineAssets::GetShader(EngineShader::Texture);
```

## Rules

- 不允许 `AssetMetadata` 保存绝对路径。
- 不允许 `AssetMetadata` 保存运行时加载状态。
- 不允许 `MemoryAsset` 拥有 `FilePath`。
- 不允许 Serializer 自己拼接 Engine / Project 根目录。
- 不允许用户项目直接使用 `AssetManager`。
- 不允许用户项目直接使用 `ShaderAsset` / `TextureAsset`。
- 不允许用户项目通过路径任意导入 EngineAsset。
- EngineAsset 通过 `EngineAssets` catalog 公开。
- ProjectAsset 通过 `UserAssetManager` 导入。
- 加载实际产物时使用明确 handle，不通过隐式搜索。
