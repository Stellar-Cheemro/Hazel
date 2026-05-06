# 0001. 使用 AssetDomain 区分 Engine / Project / Memory Asset

Date: 2026-05-03

Status: Accepted

## Context

Asset 系统需要支持三类资源：

1. 引擎内部资源，例如默认 Shader、默认 Texture、编辑器图标。
2. 项目资源，例如项目导入的贴图、Shader、场景、网格、音频。
3. 运行时内存资源，例如动态生成的 Mesh、临时 Texture、运行时 Material。

需要明确：

- 根目录由谁持有？
- Domain 是持有者还是分类标签？
- MemoryAsset 是否进入 Registry？
- Serializer 是否需要关心 Engine / Project / Memory？
- 用户项目如何使用公开 Engine Asset？
- 用户项目是否应该接触 Asset 系统内部 Runtime wrapper？

## Decision

采用以下设计：

1. `AssetDomain` 只表达资源来源域，不表达持有者。
2. `AssetMetadata` 只保存稳定身份信息：
   - Handle
   - Type
   - Domain
   - FilePath
3. Engine Asset Root 由 `AssetManager::Init()` 内部通过 `AssetRootLocator` 自动定位，并保存在 `AssetSystemFileResolver` 中。
4. Project Asset Root 由 `Project::GetActive()` 提供，由 `AssetSystemFileResolver` 在真实文件系统边界访问。
5. MemoryAsset 不走文件路径，不走 Resolver，不走 Serializer，但进入 Registry 和 RuntimeCache。
6. `AssetRegistry` 使用 `Domain + NormalizedRelativePath` 作为文件资源路径索引。
7. 真实文件系统路径解析统一由 `AssetSystemFileResolver` 完成。
8. `AssetManager` 是引擎内部 Asset 系统入口，负责编排流程。
9. `UserAssetManager` 是用户项目访问 Project / Memory 资源，以及通过明确 `AssetHandle` 获取实际运行时产物的公开入口。
10. `EngineAssets` 是用户项目使用公开 Engine Asset 的公开目录入口。
11. 用户项目不通过路径任意导入 Engine Asset，也不直接操作 Engine Asset 生命周期。
12. `ShaderAsset`、`TextureAsset` 等 Runtime wrapper 属于 Asset 系统内部；用户侧 API 返回 `Shader`、`Texture2D` 等实际运行时产物。

## Alternatives Considered

### 方案 A：把根目录放进 AssetMetadata

Rejected.

原因：

- Project 移动目录后 Metadata 会失效。
- Engine 发布路径变化后 Metadata 会失效。
- MemoryAsset 没有根目录。
- Metadata 会混入运行时环境信息。

### 方案 B：拆成 EngineAssetManager / ProjectAssetManager / MemoryAssetManager

Rejected.

原因：

- 当前资源来源只有三类，拆分管理器会增加入口数量。
- Serializer、Registry、RuntimeCache 的协调会更复杂。
- 当前阶段 `AssetDomain` 已足以表达资源来源。

### 方案 C：新增 AssetSource / AssetStorage 多态系统

Rejected for now.

原因：

- 未来可能有用，但当前阶段过度设计。
- 现在只需要通过 Domain 做简单分流。

### 方案 D：AssetManager 编排生命周期，AssetSystemFileResolver 解析真实路径

Accepted.

原因：

- 符合当前代码结构。
- `AssetManager` 负责 Import / Restore / RegisterMemory / Get / Unload / Remove 等流程编排。
- `AssetSystemFileResolver` 负责 Engine / Project 真实路径解析和文件系统边界检查。
- Serializer 可以继续只关心最终文件路径。
- Registry 可以继续只关心索引。

### 方案 E：用户项目通过 `UserAssetManager::ImportEngineAsset(path)` 使用 Engine Asset

Rejected.

原因：

- 会暴露 Engine Asset 的内部路径结构。
- 用户无法从路径 API 中判断哪些 Engine Asset 是公开稳定资源。
- Engine Asset 的注册和生命周期应由引擎侧控制。
- 用户应通过 `EngineAssets` catalog 使用公开 Engine Asset。

### 方案 F：用户侧直接获取 `ShaderAsset` / `TextureAsset` 等 Runtime wrapper

Rejected.

原因：

- Runtime wrapper 是 Asset 系统内部实现细节。
- 用户真正需要的是 `Shader`、`Texture2D` 等实际运行时产物。
- wrapper 结构变化不应影响用户项目代码。

## Consequences

正面影响：

- Engine / Project / Memory Asset 边界清晰。
- Serializer 不需要知道资源来自哪个 Domain。
- AssetMetadata 保持稳定、可序列化。
- Project 目录迁移不会破坏 Metadata。
- MemoryAsset 可以通过 Handle 统一管理。
- 用户项目可以使用公开 Engine Asset。
- 用户项目不会依赖 Engine Asset 的内部路径。
- 用户项目不会依赖 Asset 系统内部 Runtime wrapper。

代价：

- `AssetManager` 需要协调更多内部组件。
- `AssetSystemFileResolver` 需要依赖 `Project::GetActive()` 解析 Project Asset。
- Registry 内部需要使用 Domain + Path 作为路径索引。
- 需要维护 `EngineAssets` 公开资源目录。
- 新增公开 Engine Asset 时需要同步更新公开目录。

## Rules

- 不允许 AssetMetadata 保存绝对路径。
- 不允许 AssetMetadata 保存根目录。
- 不允许 AssetMetadata 保存加载状态。
- 不允许 Serializer 自己拼接资源根目录。
- 不允许 MemoryAsset 拥有 FilePath。
- 不允许 Registry key 暴露到 AssetRegistry 外部。
- 不允许用户项目直接使用 AssetManager 操作资源。
- 用户项目必须通过 UserAssetManager 访问 Project / Memory 资源。
- 用户项目必须通过 EngineAssets 使用公开 Engine Asset。
- 用户项目不通过路径任意导入 Engine Asset。
- 用户项目不直接使用 ShaderAsset / TextureAsset 等 Runtime wrapper。
