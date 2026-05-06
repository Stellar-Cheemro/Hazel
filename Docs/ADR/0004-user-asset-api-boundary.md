# 0004. 用户侧 Asset API 与公开头文件边界

Date: 2026-05-05

Status: Accepted

## Context

Asset 系统同时服务引擎内部和用户项目。

引擎内部需要访问完整能力：

- 导入 Engine Asset；
- 导入 Project Asset；
- 恢复 File Asset Metadata；
- 注册 Memory Asset；
- 解析真实路径；
- 使用 Registry、Cache、Serializer 等内部组件。

用户项目只应访问安全能力：

- 导入 Project Asset；
- 注册 Memory Asset；
- 获取 Asset；
- 查询只读 Metadata；
- 查询 Handle / Loaded 状态；
- 对 Project / Memory Asset 执行允许的生命周期操作。

如果用户项目直接包含 `AssetManager.h` 或 `Asset/Internal/*`，会产生以下问题：

- 用户项目可能导入或移除 Engine Asset；
- 用户项目可能依赖内部实现细节；
- 后续重构 Internal 组件会破坏用户代码；
- `Hazel.h` 会暴露过多引擎内部 API；
- Asset 系统权限边界不清。

## Decision

引入并保留 `UserAssetManager` 作为用户侧 Asset API。

### Engine Internal Boundary

引擎内部模块可以使用：

```cpp
AssetManager
AssetRegistry
Asset/Internal/*
Asset/Serialization/*
```

典型内部使用者：

- Renderer；
- Editor；
- Engine Runtime；
- Asset 系统自身。

### User Boundary

用户项目应使用：

```cpp
UserAssetManager
```

用户侧允许：

```cpp
UserAssetManager::ImportProjectAsset(...);
UserAssetManager::RegisterMemoryAsset(...);
UserAssetManager::GetAsset<T>(...);
UserAssetManager::GetMetadata(...);
UserAssetManager::IsAssetHandleValid(...);
UserAssetManager::IsAssetLoaded(...);
UserAssetManager::TryUnloadProjectAsset(...);
UserAssetManager::TryRemoveProjectAsset(...);
UserAssetManager::TryRemoveMemoryAsset(...);
```

用户侧不允许：

```cpp
AssetManager::ImportEngineAsset(...);
AssetManager::ResolveAssetPath(...);
AssetRegistry::TryRegisterAsset(...);
AssetRootLocator::FindEngineAssetRoot(...);
AssetSerializerRegistry::Register(...);
```

### Hazel.h Public Includes

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

```cpp
#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/AssetRegistry.h>
#include <Hazel/Asset/Internal/...>
#include <Hazel/Asset/Serialization/...>
```

### Runtime Asset Types

`ShaderAsset` 和 `TextureAsset` 可以暴露给用户项目。

原因是用户项目可能需要写：

```cpp
Ref<ShaderAsset> shader =
    UserAssetManager::GetAsset<ShaderAsset>(handle);

Ref<TextureAsset> texture =
    UserAssetManager::GetAsset<TextureAsset>(handle);
```

Runtime Asset 类型只包装运行时对象，不负责加载、注册和真实路径解析，因此可以作为用户可见类型。

## Alternatives Considered

### 方案 A：用户项目直接使用 AssetManager

Rejected.

原因：

- 用户项目会获得 Engine Asset 操作能力。
- 内部 API 难以演进。
- 用户侧和引擎内部边界不清。

### 方案 B：把 AssetManager 拆成 EngineAssetManager / ProjectAssetManager / MemoryAssetManager

Rejected.

原因：

- 入口数量增加。
- 当前 Domain 已足以表达资源来源。
- Registry、Cache、Serializer 协调复杂度上升。

### 方案 C：只靠 include 习惯约束用户不要使用内部 API

Rejected.

原因：

- 没有明确 API 边界。
- `Hazel.h` 一旦暴露内部头，用户很容易依赖。
- 后续重构成本高。

### 方案 D：提供 UserAssetManager 门面

Accepted.

原因：

- 用户侧入口清晰。
- 内部实现可继续演进。
- 可对 Project / Memory 操作做权限限制。
- 保留引擎内部完整能力。

## Consequences

### Positive

- 用户项目 API 更安全。
- Engine Asset 不会被用户项目直接操作。
- 内部组件可以自由重构。
- `Hazel.h` 更干净。
- Renderer / Editor 仍可使用内部 AssetManager。

### Negative

- 需要维护 `AssetManager` 和 `UserAssetManager` 两套接口。
- 需要保证用户文档和示例使用 `UserAssetManager`。
- Sandbox 旧代码需要从 `AssetManager` 迁移到 `UserAssetManager`。

## Rules

- 用户项目不直接 include `AssetManager.h`。
- 用户项目不直接 include `Asset/Internal/*`。
- 用户项目不直接 include `Asset/Serialization/*`。
- `Hazel.h` 不暴露内部 Asset 组件。
- Engine 内部资源加载由 `AssetManager` 完成。
- 用户项目资源加载由 `UserAssetManager` 完成。
