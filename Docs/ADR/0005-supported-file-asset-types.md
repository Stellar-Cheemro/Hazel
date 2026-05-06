# 0005. 使用 SupportedFileAssetTypes 表达当前具备 Serializer 支持的文件资源类型

Date: 2026-05-05

Status: Accepted

## Context

`AssetType` 枚举中可以预留未来资源类型：

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

当前代码中，扩展名识别函数可以识别：

```text
.png / .jpg / .jpeg -> Texture2D
.shader / .glsl    -> Shader
.scene             -> Scene
.mesh              -> Mesh
.audio             -> Audio
```

但当前真正接入 Serializer 注册和加载流程的只有：

```text
Texture2D
Shader
```

如果把所有 `AssetType` 都纳入 Serializer 强校验，会导致：

- `Scene`、`Mesh`、`Audio` 尚未实现 Serializer 时产生无意义错误；
- `AssetSerializerRegistry::Validate()` 报缺失 Serializer；
- “已识别类型”和“当前可加载类型”语义混淆。

因此需要区分：

- 已存在 / 已识别的 AssetType；
- 当前已经有 Serializer 支持的文件 AssetType；
- 当前用户侧可以直接获取的实际运行时产物类型。

## Decision

使用：

```cpp
SupportedFileAssetTypes
```

表达当前已经有 Serializer 支持的文件资源类型。

当前定义：

```cpp
inline constexpr std::array<AssetType, 2> SupportedFileAssetTypes =
{
    AssetType::Texture2D,
    AssetType::Shader
};
```

当前辅助函数命名为：

```cpp
inline constexpr bool IsFileAssetType(AssetType type);
```

在现有代码语义中，它判断 `type` 是否在 `SupportedFileAssetTypes` 中。

### AssetType 可以预留

`AssetType` 可以继续包含：

```text
Scene
Mesh
Audio
```

这些类型可以由扩展名识别函数返回，用于保留未来能力和维持 Metadata 类型表达。

### 扩展名识别与 Serializer 支持是两个层次

当前代码允许：

```cpp
GetAssetTypeFromExtension(".scene") -> AssetType::Scene
GetAssetTypeFromExtension(".mesh")  -> AssetType::Mesh
GetAssetTypeFromExtension(".audio") -> AssetType::Audio
```

但这些类型当前不在 `SupportedFileAssetTypes` 中。

因此：

```text
扩展名可识别 != 当前可完整加载
```

当前可完整加载的文件类型仍然只有：

```text
Texture2D
Shader
```

### AssetSerializerRegistry 以 SupportedFileAssetTypes 为校验来源

`AssetSerializerRegistry::Register()` 和 `Validate()` 遍历：

```cpp
SupportedFileAssetTypes
```

而不是遍历所有 `AssetType`。

这意味着：

- Texture2D 必须有 Serializer。
- Shader 必须有 Serializer。
- Scene / Mesh / Audio 在加入 `SupportedFileAssetTypes` 前，不参与 SerializerRegistry 强校验。

### 用户侧产物 API 与 AssetType 对应

当前用户侧实际产物 API：

```cpp
UserAssetManager::LoadShader(handle);
UserAssetManager::LoadTexture2D(handle);
```

分别要求：

```text
LoadShader(handle)    -> AssetType::Shader
LoadTexture2D(handle) -> AssetType::Texture2D
```

用户侧不通过：

```cpp
UserAssetManager::GetAsset<ShaderAsset>(handle);
UserAssetManager::GetAsset<TextureAsset>(handle);
```

获取内部 wrapper。

## Alternatives Considered

### 方案 A：命名为 FileAssetTypes

Rejected.

原因：

- 容易被理解成所有文件型 AssetType。
- `Scene`、`Mesh`、`Audio` 也可能是文件型资源，但当前尚未支持加载。
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

### 方案 D：保留 AssetType 预留项，但使用 SupportedFileAssetTypes 表达当前 Serializer 支持范围

Accepted.

原因：

- 区分“可识别 / 未来可能支持”和“当前已经支持加载”。
- 避免初始化阶段无意义报错。
- 命名对后续扩展更友好。
- SerializerRegistry 校验范围明确。
- 与当前代码行为一致。

## Consequences

### Positive

- 启动时不会因为预留类型缺 Serializer 而报错。
- 当前加载支持范围表达清晰。
- 后续新增资源类型流程明确。
- `AssetType` 可以继续作为长期枚举规划。
- 用户侧产物加载 API 可以在调用内部 wrapper 前先做 `AssetType` 检查。

### Negative

- 可能出现“扩展名已识别，但加载时没有 Serializer”的情况。
- 新增资源类型时需要同步更新多个位置。
- 如果只添加扩展名映射但忘记加入 `SupportedFileAssetTypes`，资源不会进入完整加载支持流程。
- 如果只加入 `SupportedFileAssetTypes` 但忘记注册 Serializer，Validate 会报错。
- 新增用户侧产物类型时，需要增加对应 Load API。

## Extension Rule

新增 `Mesh` 完整加载支持时，必须按以下顺序更新：

1. `AssetTypes.h`
   - 确认 `AssetType::Mesh`
   - `SupportedFileAssetTypes` 加入 `AssetType::Mesh`
   - 保持 `GetAssetTypeFromExtension(".mesh") -> AssetType::Mesh`
   - 如需要，增加 `GetPrimaryAssetExtension(AssetType::Mesh) -> ".mesh"`

2. `MeshSerializer`
   - `CanLoad(AssetType::Mesh)` 返回 true
   - 实现具体加载逻辑

3. `AssetManager::RegisterSerializers()`
   - 注册 `MeshSerializer`

4. Runtime wrapper
   - 增加 `MeshAsset` 或对应内部 wrapper

5. 用户侧产物 API
   - 增加 `UserAssetManager::LoadMesh(handle)` 或更合适的实际产物获取 API

6. 构建和运行验证
   - `AssetSerializerRegistry::Validate()` 不应再报缺失 Mesh Serializer
   - `AssetManager::GetAsset(meshHandle)` 应能成功加载 Mesh Runtime Asset
   - 用户侧产物 API 应能返回实际 Mesh 产物

## Rules

- `SupportedFileAssetTypes` 只包含当前真正支持 Serializer 加载的文件资源类型。
- 预留 `AssetType` 不自动进入 `SupportedFileAssetTypes`。
- `AssetSerializerRegistry` 只校验 `SupportedFileAssetTypes`。
- `GetAssetTypeFromExtension()` 可以返回预留类型。
- 扩展名可识别不等于当前可完整加载。
- 用户侧 Load API 必须检查 AssetType。
- 用户侧不直接获取 Runtime wrapper。
- 新增完整文件资源类型支持时，必须同时更新扩展名映射、支持表、Serializer、Runtime wrapper 和用户侧产物 API。
