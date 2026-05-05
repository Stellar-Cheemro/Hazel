# 0003. AssetManager 内部组件划分与初始化流程

Date: 2026-05-05

Status: Accepted

## Context

Asset 系统需要同时处理：

- Asset 系统初始化；
- Engine Asset Root 定位；
- Registry 创建和清理；
- Runtime Cache 清理；
- Serializer 注册和校验；
- Handle 分配；
- Engine / Project / Memory Asset 导入；
- Asset 加载和卸载。

如果所有职责都直接堆叠在 `AssetManager.cpp` 中，会导致：

- `AssetManager.cpp` 持续膨胀；
- 初始化流程和低层实现细节混杂；
- 路径定位、Resolver、Registry、Cache、Serializer 的职责边界不清；
- `Application` 或用户项目可能接触不该暴露的内部细节；
- 后续扩展 ProjectRootLocator、Asset Database、资源热重载时难以维护。

## Decision

`AssetManager` 保留为引擎内部 Asset 系统入口，但它主要承担流程编排职责。

低层职责拆分到 `Asset/Internal` 组件中：

```text
Asset/Internal/
├─ AssetHandleAllocator
├─ AssetRootLocator
├─ AssetRuntimeCache
├─ AssetSerializerRegistry
└─ AssetSystemFileResolver
```

### AssetManager 初始化入口

保留两个初始化入口：

```cpp
static void Init();
static void Init(const std::filesystem::path& engineAssetRoot);
```

语义：

- `Init()`：自动定位 Engine Asset Root。
- `Init(engineAssetRoot)`：显式指定 Engine Asset Root，用于测试、工具或特殊启动流程。

### AssetManager 初始化流程

`AssetManager::Init()` 只做线性流程编排：

```text
ResetState()
SetEngineAssetRootAuto()
RegisterSerializers()
ValidateSerializers()
```

`AssetManager::Init(engineAssetRoot)` 只做：

```text
ResetState()
SetEngineAssetRootExplicit(engineAssetRoot)
RegisterSerializers()
ValidateSerializers()
```

流程函数不重复报错；具体失败原因由下层函数输出。

### AssetRootLocator 归属

`AssetRootLocator` 是 Asset 系统内部组件。

`Application` 不直接调用：

```cpp
AssetRootLocator::FindEngineAssetRoot(...)
```

`Application` 只调用：

```cpp
AssetManager::Init();
```

### Internal 组件不导出公共 ABI

`Asset/Internal` 下组件不作为用户 API 暴露。

这些类默认不加 `HAZEL_API`，除非后续确实需要跨 DLL 边界直接使用。

## Component Responsibilities

### AssetHandleAllocator

职责：

- 分配新的 AssetHandle；
- 在恢复已有 Metadata 后推进下一个 Handle。

### AssetRootLocator

职责：

- 自动查找 Engine Asset Root；
- 通过 `.hazel_engine_assets` 标记文件确认合法资源目录；
- 返回真实目录路径。

不负责：

- 保存 Engine Asset Root；
- 拼接资源路径；
- 处理 Project Asset Root。

### AssetRuntimeCache

职责：

- 保存已加载资源：

```text
Handle -> Ref<Asset>
```

不负责：

- Metadata；
- 路径解析；
- Serializer 选择。

### AssetSerializerRegistry

职责：

- 保存 `AssetType -> AssetSerializer` 映射；
- 注册内置 Serializer；
- 校验 `SupportedFileAssetTypes` 是否有对应 Serializer。

不负责：

- 加载文件；
- 缓存 Asset；
- 路径解析。

### AssetSystemFileResolver

职责：

- 保存 Engine Asset Root；
- 根据 AssetMetadata 解析真实文件路径；
- 对 Engine / Project / Memory Domain 做路径分流。

不负责：

- Asset 加载；
- Serializer 选择；
- Registry 索引；
- RuntimeCache。

## Alternatives Considered

### 方案 A：Application 直接调用 AssetRootLocator

Rejected.

原因：

- `Application` 会依赖 `Asset/Internal`。
- Core/Application 层会知道 Engine Asset Root 定位策略。
- 后续定位策略变化会影响 Application。
- Internal 细节泄露。

### 方案 B：CMake 注入 Engine Asset Root 绝对路径

Rejected for default runtime.

原因：

- 换机器后路径可能失效。
- 二进制中会包含源码绝对路径。
- 不利于安装包和可迁移运行。
- 适合测试或特殊工具，不适合作为默认方案。

### 方案 C：构建后复制 Assets 到输出目录

Rejected as default.

原因：

- 多配置、多 target 会产生多份资源。
- 输出目录容易变脏。
- 大资源复制成本高。
- 修改资源后的同步策略容易出错。

### 方案 D：AssetManager 直接实现所有内部逻辑

Rejected.

原因：

- 职责过多。
- 文件持续膨胀。
- 不利于测试和维护。
- 不利于后续扩展。

### 方案 E：AssetManager 编排流程，Internal 组件实现细节

Accepted.

原因：

- Application 调用简单。
- AssetRootLocator 不泄露。
- AssetManager 保持统一入口。
- 内部组件职责清晰。
- 后续扩展空间更好。

## Consequences

### Positive

- `Application` 不需要知道 Asset Root 查找逻辑。
- Asset 系统初始化顺序清晰。
- 内部组件边界明确。
- 便于后续增加 ProjectRootLocator、Asset Database、异步加载等功能。
- 减少 `AssetManager.cpp` 的职责膨胀。

### Negative

- 内部类数量增加。
- 需要维护 CMake 源文件列表。
- 初始化失败日志需要遵守分层规则，避免重复报错。

## Rules

- `Application` 不直接 include `Asset/Internal/*`。
- `AssetRootLocator` 只由 `AssetManager` 使用。
- `AssetManager::Init()` 只做流程编排，不重复输出下层错误。
- Engine Asset Root 查找失败由定位函数或配置函数输出清晰日志。
- Internal 组件不进入 `Hazel.h`。
