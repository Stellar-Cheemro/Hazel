# Asset 系统设计变更记录

## Unreleased

### Added

- 新增 `EngineAssets` 设计。
  - 作为公开 EngineAsset catalog。
  - 使用 `EngineShader` / `EngineTexture` 枚举暴露当前公开引擎资源。
  - 提供 `GetShader(...)`、`GetTexture2D(...)`、`GetShaderHandle(...)`、`GetTexture2DHandle(...)`。
  - 内部通过 `AssetManager::ImportEngineAsset(...)` 注册 EngineAsset。
  - 对外返回实际运行时产物。

- 新增用户侧实际产物加载规则：
  - `UserAssetManager::LoadShader(handle)`
  - `UserAssetManager::LoadTexture2D(handle)`

- 新增用户侧 MemoryAsset 注册规则：
  - `UserAssetManager::RegisterMemoryShader(shader)`
  - `UserAssetManager::RegisterMemoryTexture2D(texture)`

### Changed

- 保持原有 Engine / Project / Memory 三类资源域框架。
- 保持原有 AssetManager / Registry / Path / Resolver / Cache / Serializer 职责拆分框架。

- 补充 `EngineAssets` 作为公开 EngineAsset catalog。
  - 避免用户侧依赖 EngineAsset 内部路径。
  - 让用户通过稳定枚举发现公开 EngineAsset。

- 调整用户侧 API 边界。
  - 用户通过 `EngineAssets` 使用公开 EngineAsset。
  - 用户通过 `UserAssetManager` 处理 ProjectAsset / MemoryAsset。
  - 用户侧获取 `Shader` / `Texture2D`，不获取 `ShaderAsset` / `TextureAsset`。

- 调整加载 API 模型。
  - 运行时加载使用明确 handle。
  - 加载函数不在名称中表达 Domain。
  - Domain 由 handle 对应的 metadata 决定。
  - 默认 Engine -> Project -> Memory 隐式搜索不作为运行时加载行为。

- 调整 Renderer2D 集成方向。
  - Renderer2D 通过 `EngineAssets` 获取默认 Shader。
  - Renderer2D 不再依赖默认 Shader 的内部路径。

- 调整 `Hazel.h` 公开边界。
  - 公开：
    - AssetTypes
    - AssetMetadata
    - UserAssetManager
    - EngineAssets
    - Shader
    - Texture
  - 内部：
    - AssetManager
    - AssetRegistry
    - Asset/Internal/*
    - Asset/Serialization/*
    - Asset/Runtime/*

### Removed

- 移除用户侧 `ImportEngineAsset(path)` 设计。
- 移除用户侧泛型 `GetAsset<TWrapper>()` 方向。
- 移除用户侧直接依赖 `ShaderAsset` / `TextureAsset` 的方向。
- 移除长期 API 中 `LoadEngineShader()` / `LoadProjectShader()` 这类 Domain + Type 组合方向。
- 移除默认运行时 Engine -> Project -> Memory 隐式搜索方向。

### Current Chains

```text
Engine Shader:
    EngineAssets
    -> AssetManager
    -> ShaderSerializer
    -> ShaderAsset
    -> Shader

Project Shader:
    UserAssetManager::ImportProjectAsset
    -> UserAssetManager::LoadShader
    -> AssetManager
    -> ShaderSerializer
    -> ShaderAsset
    -> Shader

Engine Texture:
    EngineAssets
    -> AssetManager
    -> TextureSerializer
    -> TextureAsset
    -> Texture2D

Project Texture:
    UserAssetManager::ImportProjectAsset
    -> UserAssetManager::LoadTexture2D
    -> AssetManager
    -> TextureSerializer
    -> TextureAsset
    -> Texture2D

Memory Shader:
    UserAssetManager::RegisterMemoryShader
    -> ShaderAsset
    -> AssetManager::RegisterMemoryAsset
    -> UserAssetManager::LoadShader

Memory Texture:
    UserAssetManager::RegisterMemoryTexture2D
    -> TextureAsset
    -> AssetManager::RegisterMemoryAsset
    -> UserAssetManager::LoadTexture2D
```

### Next Implementation Steps

1. 实现 `EngineAssets.h/cpp`。
2. 重写 `UserAssetManager.h/cpp`。
3. 更新 `Application` 启动和关闭顺序。
4. 更新 `Renderer2D` 使用 `EngineAssets`。
5. 更新 `Hazel.h`。
6. 更新 CMake source list。
