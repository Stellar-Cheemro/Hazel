# Asset 系统设计变更记录

本文档记录 Asset 系统设计和实现的重要变更摘要。

详细架构决策见：

```text
Docs/ADR/
```

当前最新设计见：

```text
Docs/Architecture/AssetSystem.md
```

## Unreleased

### Added

- 新增 `AssetRootLocator`。
  - 位于 `Asset/Internal`。
  - 用于自动定位 Engine Asset Root。
  - 通过 `.hazel_engine_assets` 标记文件确认合法 Engine 资源目录。
  - 仅由 `AssetManager` 内部使用。

- 新增 `AssetSystemFileResolver`。
  - 负责将 Engine / Project Asset 的逻辑路径解析为真实文件路径。
  - Engine Asset 使用 EngineAssetRoot。
  - Project Asset 使用 `Project::GetActive()`。
  - Memory Asset 不解析路径。

- 新增 `AssetRuntimeCache`。
  - 负责维护 `Handle -> Ref<Asset>` 的已加载资源缓存。

- 新增 `AssetSerializerRegistry`。
  - 负责维护 `AssetType -> AssetSerializer` 映射。
  - 根据 `SupportedFileAssetTypes` 校验当前支持的文件资源类型是否都有 Serializer。

- 新增 `AssetHandleAllocator`。
  - 负责分配 AssetHandle。
  - 支持恢复已有 Metadata 后推进下一个 Handle。

- 新增 `AssetPath`。
  - 负责 Asset 逻辑相对路径规范化。
  - 主接口为 `TryNormalizeRelativePath()`。
  - 统一路径分隔符为 `/`。
  - 禁止绝对路径、盘符路径、URI 风格路径和 `..` 路径片段。

- 新增 `Asset/Runtime` 目录。
  - `ShaderAsset`
  - `TextureAsset`

- 新增 `Asset/Serialization` 目录。
  - `AssetSerializer`
  - `ShaderSerializer`
  - `TextureSerializer`

- 新增 `UserAssetManager`。
  - 作为用户项目访问 Asset 系统的安全门面。

- 新增 `SupportedFileAssetTypes`。
  - 表示当前真正支持文件导入和 Serializer 加载的资源类型。
  - 当前包含：
    - `Texture2D`
    - `Shader`

### Changed

- 重构 `AssetManager`。
  - 从“大而全管理器”重构为引擎内部流程编排入口。
  - 原先集中在 `AssetManager.cpp` 中的路径规范化、根目录定位、Registry 索引、Serializer 选择、运行时缓存、文件加载等职责，被拆分到更明确的内部组件中。
  - 初始化流程精简为：
    - `ResetState()`
    - `SetEngineAssetRootAuto()`
    - `RegisterSerializers()`
    - `ValidateSerializers()`
  - `AssetRootLocator` 被收进 `AssetManager` 内部使用。
  - `Application` 只调用 `AssetManager::Init()`，不再关心 Engine Asset Root 定位。

- 重构 `AssetRegistry`。
  - 只负责 Metadata 索引。
  - 支持 `Handle -> Metadata` 查询。
  - 支持 `Domain + NormalizedPath -> Handle` 查询。
  - Memory Asset 不进入路径索引。

- 重构 `AssetMetadata`。
  - `FilePath` 改为 Asset 逻辑相对路径字符串。
  - 不保存真实文件系统路径。
  - 不保存加载状态。

- 重构 `AssetTypes`。
  - 使用 `SupportedFileAssetTypes` 表示当前真正支持文件导入和 Serializer 加载的 AssetType。
  - 当前支持：
    - `Texture2D`
    - `Shader`
  - `Scene`、`Mesh`、`Audio` 暂时作为预留类型。

- 重构 `ShaderSerializer`。
  - 负责从 Shader 文件创建 `ShaderAsset`。
  - 通过 `AssetManager::ResolveAssetPath()` 获取真实路径。

- 重构 `TextureSerializer`。
  - 负责从 Texture 文件创建 `TextureAsset`。
  - 通过 `AssetManager::ResolveAssetPath()` 获取真实路径。

- 调整 `Renderer2D`。
  - 默认 FlatColor Shader 改为通过 Asset 系统导入。
  - 使用 `AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl")`。
  - 使用 `AssetManager::GetAsset<ShaderAsset>()` 获取运行时 Shader。
  - 保持原 Hazel 静态渲染器风格。
  - `DrawQuad()` 负责上传 `u_Model`、`u_Color` 并提交 DrawIndexed。

- 调整公开头文件边界。
  - `Hazel.h` 应暴露：
    - `Asset`
    - `AssetTypes`
    - `AssetMetadata`
    - `UserAssetManager`
    - `ShaderAsset`
    - `TextureAsset`
  - `Hazel.h` 不应暴露：
    - `AssetManager`
    - `AssetRegistry`
    - `Asset/Internal/*`
    - `Asset/Serialization/*`

- 调整 Project Asset 路径职责。
  - Project 系统继续负责 ProjectDirectory 和 AssetDirectory。
  - Project Asset 的真实路径由 `Project::GetActive()` 提供。
  - Engine Asset Root 和 Project Asset Root 不混用。

- 调整文档职责划分。
  - `AssetSystem.md` 只描述当前稳定架构。
  - 重构过程、历史变化和迁移说明记录在本 ChangeLog 中。
  - 避免在 Architecture 文档中使用“当前不再”“原先集中”等变化叙述。

### Fixed

- 修复 `AssetPath.cpp` 未参与链接导致的 LNK2019。
- 修复 `AssetPath::TryNormalizeRelativePath` 声明和实现名称不一致导致的链接错误。
- 修复 `AssetFileSystemResolver` / `AssetSystemFileResolver` 命名不一致问题。
- 修复 `AssetSerializerRegistry` 持有 `Scope<AssetSerializer>` 时的不可拷贝问题。
- 修复 `Renderer2D` 未导出 DLL 符号导致 Sandbox 链接失败的问题。
- 修复 Sandbox 侧继续使用旧 `AssetManager::ImportAsset` API 的问题。
- 修复 `Renderer2D` 使用错误参数调用 `GetAsset<ShaderAsset>()` 的问题。
- 修复 `Renderer2D::DrawQuad()` 为空导致不提交绘制命令的问题。
- 修复 Shader uniform 名称与 Renderer2D 上传名称不一致的问题。
- 修复 `AssetManager::Init()` 未先于 `Renderer2D::Init()` 执行导致 Registry 未初始化的问题。
- 修复 Engine Asset Root 运行目录相对路径错误问题，引入 `AssetRootLocator` 自动定位。

### Removed

- 移除旧的 `AssetManager::ImportAsset` 使用方式。
- 移除用户侧直接访问内部 `AssetManager` 的推荐路径。
- 移除 `Application` 直接处理 Engine Asset Root 定位的设计。
- 移除 `AssetManager` 中部分低层路径、缓存、Serializer 管理细节。
- 移除 Serializer 中重复的路径职责。
- 移除 Runtime Asset 中的加载职责。

### Notes

当前 Asset 系统已完成基础职责拆分，但仍未实现完整编辑器资产数据库。

当前支持链路：

```text
Engine Shader  -> ShaderSerializer  -> ShaderAsset  -> Renderer2D
Project Shader -> ShaderSerializer  -> ShaderAsset
Engine Texture -> TextureSerializer -> TextureAsset
Project Texture -> TextureSerializer -> TextureAsset
Memory Asset -> AssetRuntimeCache
```

后续优先事项：

1. 完善 ProjectAsset 解析链路；
2. 增加 ProjectRootLocator；
3. 增加 Scene / Mesh / Audio Serializer；
4. 增加 Asset Database；
5. 增加资源重载与引用追踪；
6. 增加 Editor Asset Browser。

## 2026-05-03

### Added

- 新增 Asset 系统职责边界设计。
- 新增 Engine / Project / Memory 三类 Asset 的 Domain 语义。
- 新增 AssetPath 模块设计。
- 新增 MemoryAsset 生命周期规则。
- 新增 Registry key 内部化规则。
- 新增 AssetMetadata 字段顺序建议。

### Changed

- 明确 `AssetDomain` 只表示资源来源域，不表示资源持有者。
- 明确根目录不放在 `AssetMetadata` 中。
- 明确 Memory Asset 不走路径、不走 `AssetSerializer`。
- 明确 Memory Asset 仍然进入 `AssetRegistry` 和 RuntimeCache。
- 将路径规范化方案确定为 `AssetPath.h/.cpp`。
- 明确 `AssetPath` 使用命名空间，不使用静态类。
- 明确 `AssetPath` 使用纯字符串规则规范化路径。
- 明确 `AssetMetadata::FilePath` 使用 `std::string` 保存稳定 Asset 相对路径。
- 明确 Registry key 生成保留在 `AssetRegistry` 内部。

### Rejected

- 暂不拆分 `EngineAssetManager` / `ProjectAssetManager` / `MemoryAssetManager`。
- 暂不引入 `AssetSource` / `AssetStorage` 多态系统。
- 暂不引入 `AssetRelativePath` 值对象。
- 不将 Registry key 生成放入 `AssetPath`。
- 不让 `AssetSerializer` 自己拼接 Engine / Project 根目录。
- 不让 `AssetMetadata` 保存绝对路径。
- 不让 `AssetMetadata` 保存运行时加载状态。
- 不让 Memory Asset 拥有 `FilePath`。
