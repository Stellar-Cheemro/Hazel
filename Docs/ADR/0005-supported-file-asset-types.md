# 0005. 使用 SupportedFileAssetTypes 表达当前支持的文件资源类型

Date: 2026-05-05

Status: Accepted

## Context

`AssetType` 枚举中可以预留未来资源类型：

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

但当前真正接入文件导入和 Serializer 加载流程的只有：

```text
Texture2D
Shader
```

如果把所有“理论上可能来自文件”的类型都纳入强校验，会导致：

- `Scene`、`Mesh`、`Audio` 尚未实现 Serializer 时产生无意义错误；
- `AssetSerializerRegistry::Validate()` 报缺失 Serializer；
- `GetAssetTypeFromPath()` 可能识别出当前无法加载的类型；
- “预留类型”和“当前支持类型”语义混淆。

因此需要区分：

- 所有 AssetType；
- 当前已经支持导入和加载的文件 AssetType。

## Decision

使用：

```cpp
SupportedFileAssetTypes
```

表达当前已经支持“文件导入 + Serializer 加载”的资源类型。

当前定义：

```cpp
inline constexpr std::array<AssetType, 2> SupportedFileAssetTypes =
{
    AssetType::Texture2D,
    AssetType::Shader
};
```

辅助函数：

```cpp
inline constexpr bool IsSupportedFileAssetType(AssetType type);
```

### AssetType 可以预留

`AssetType` 可以继续包含：

```text
Scene
Mesh
Audio
```

但在对应 Serializer 实现前，不加入 `SupportedFileAssetTypes`。

### 扩展名识别只返回当前支持类型

当前 `GetAssetTypeFromPath()` 只应识别当前支持导入和加载的扩展名。

例如当前支持：

```text
.png
.jpg
.jpeg
.shader
.glsl
```

对应：

```text
Texture2D
Shader
```

`.scene`、`.mesh`、`.audio` 可以保留注释或后续扩展，但不应在未实现 Serializer 前进入正式支持表。

### AssetSerializerRegistry 以 SupportedFileAssetTypes 为校验来源

`AssetSerializerRegistry::Register()` 和 `Validate()` 遍历：

```cpp
SupportedFileAssetTypes
```

而不是遍历所有 `AssetType`。

## Alternatives Considered

### 方案 A：命名为 FileAssetTypes

Rejected.

原因：

- 容易被理解成所有文件型 AssetType。
- `Scene`、`Mesh`、`Audio` 也可能是文件型资源，但当前尚未支持。
- 名称与实际用途不完全一致。

### 方案 B：命名为 SerializerBackedAssetTypes

Rejected.

原因：

- 语义准确，但过度绑定 Serializer 实现细节。
- 后续如果支持导入、验证或打包流程，`SupportedFileAssetTypes` 的表达更自然。

### 方案 C：删除未实现的 Scene / Mesh / Audio 枚举

Rejected.

原因：

- 这些类型是合理的未来资源类型。
- 删除没有必要。
- 预留枚举有助于规划后续架构。

### 方案 D：保留 AssetType 预留项，但使用 SupportedFileAssetTypes 表达当前支持范围

Accepted.

原因：

- 区分“未来可能支持”和“当前已经支持”。
- 避免初始化阶段无意义报错。
- 命名对后续扩展更友好。
- SerializerRegistry 校验范围明确。

## Consequences

### Positive

- 启动时不会因为预留类型缺 Serializer 而报错。
- 当前支持范围表达清晰。
- 后续新增资源类型流程明确。
- `AssetType` 可以继续作为长期枚举规划。

### Negative

- 新增资源类型时需要同时更新多个位置。
- 如果只添加扩展名映射但忘记加入 `SupportedFileAssetTypes`，资源不会进入完整支持流程。
- 如果只加入 `SupportedFileAssetTypes` 但忘记注册 Serializer，Validate 会报错。

## Extension Rule

新增 `Mesh` 支持时，必须按以下顺序更新：

1. `AssetTypes.h`
   - 确认 `AssetType::Mesh`
   - `SupportedFileAssetTypes` 加入 `AssetType::Mesh`
   - `GetAssetTypeFromExtension(".mesh") -> AssetType::Mesh`
   - `GetPrimaryAssetExtension(AssetType::Mesh) -> ".mesh"`

2. `MeshSerializer`
   - `CanLoad(AssetType::Mesh)` 返回 true
   - 实现具体加载逻辑

3. `AssetManager::RegisterSerializers()`
   - 注册 `MeshSerializer`

4. 构建和运行验证
   - `AssetSerializerRegistry::Validate()` 不应再报缺失 Mesh Serializer

## Rules

- `SupportedFileAssetTypes` 只包含当前真正支持导入和加载的文件资源类型。
- 预留 `AssetType` 不自动进入 `SupportedFileAssetTypes`。
- `AssetSerializerRegistry` 只校验 `SupportedFileAssetTypes`。
- 新增文件资源类型必须同时更新扩展名映射、支持表和 Serializer 注册。
