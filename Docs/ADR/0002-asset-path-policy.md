# 0002. 使用 AssetPath 统一 Asset 相对路径规范

Date: 2026-05-03

Status: Accepted

## Context

Asset 系统中路径规范化如果分散在多个地方，会导致规则不一致：

- 有的地方允许绝对路径。
- 有的地方允许 `..`。
- 有的地方使用平台相关分隔符。
- 有的地方使用 `/`。
- Engine Asset 和 Project Asset 路径规则不一致。
- Metadata、Registry、序列化之间路径表达不一致。

当前 `AssetMetadata::FilePath` 的语义是“Asset 系统内部稳定相对路径标识”，不是操作系统路径对象。

因此需要统一 Asset 系统中的相对路径规范。

## Decision

新增 AssetPath 模块：

```text
Hazel/Asset/AssetPath.h
Hazel/Asset/AssetPath.cpp
```

AssetPath 只提供一个主接口：

```cpp
namespace Hazel::AssetPath
{
    bool TryNormalizeRelativePath(
        std::string_view inputPath,
        std::string& outNormalizedPath);
}
```

AssetPath 使用纯字符串规则进行规范化，不使用 `std::filesystem::path` 参与规范化流程。

路径规则：

1. 路径不能为空。
2. 路径不能是绝对路径。
3. 路径不能包含盘符或 URI 风格的冒号。
4. 路径中的 `\` 会统一转换为 `/`。
5. `.` 路径片段会被忽略。
6. 不允许出现 `..` 路径片段。
7. 输出统一使用 `/`。
8. 不允许 Windows 文件名非法字符：`* ? " < > |`。
9. 不允许 ASCII 控制字符。
10. 不允许路径片段以空格或 `.` 结尾。
11. 不允许 Windows 设备保留名，例如 `CON`、`PRN`、`AUX`、`NUL`、`COM1`、`LPT1`。
12. 支持 UTF-8 中文路径，但不做 UTF-8 合法性校验。

AssetPath 不负责：

- Registry key 生成。
- Engine / Project 根目录拼接。
- 文件是否存在检查。
- Asset 注册。
- Asset 加载。
- 真实文件系统路径解析。
- 决定某个 Engine Asset 是否对用户公开。

`AssetMetadata::FilePath` 使用 `std::string`，保存 AssetPath 输出的稳定相对路径字符串。

真实文件系统边界由 `AssetSystemFileResolver` 负责。

公开 Engine Asset 的用户侧发现和访问由 `EngineAssets` 负责；`AssetPath` 只处理内部保存的逻辑相对路径，不作为用户侧 Engine Asset 路径 API。

## Alternatives Considered

### 方案 A：继续让各模块各自调用 `lexically_normal()`

Rejected.

原因：

- 路径规则会继续分散。
- 后续不同模块可能出现不同校验规则。
- 无法保证写入 `AssetMetadata::FilePath` 的路径一定符合 Asset 系统规则。

### 方案 B：新增 NormalizeTool

Rejected.

原因：

- `Normalize` 是动作，不是领域。
- 路径规范化、字符串规范化、Shader 源码规范化等规则完全不同。
- 容易变成杂物工具类。

### 方案 C：AssetPath 使用 header-only inline 多函数

Rejected.

原因：

- `TryNormalizeRelativePath()` 逻辑不够短。
- header-only 会增加编译依赖传播。
- 多函数接口容易让调用方错误组合。

### 方案 D：AssetPath 使用 std::filesystem::path 做规范化

Rejected.

原因：

- 会造成 `string -> filesystem::path -> string` 的冗余转换。
- Asset 路径是引擎内部逻辑路径，不应依赖操作系统路径语义。
- 不利于保持 Metadata、Registry、序列化之间的稳定字符串表达。

### 方案 E：AssetPath.h/.cpp + 单一 TryNormalizeRelativePath() 接口

Accepted.

原因：

- 接口收敛。
- 职责清楚。
- 编译依赖较小。
- 输出结果可直接写入 `AssetMetadata::FilePath`。
- Registry 可直接基于稳定字符串生成内部 key。
- 真实文件系统边界再由 `AssetSystemFileResolver` 临时转换为 `std::filesystem::path`。

### 方案 F：引入 AssetRelativePath 值对象

Rejected for now.

原因：

- 长期更类型安全。
- 但当前阶段会扩大改动面。
- 当前职责重构还不需要这么重的路径类型系统。

## Consequences

正面影响：

- Asset 相对路径规范有唯一来源。
- `AssetManager` 和 `AssetRegistry` 不再各自实现不同路径规范化规则。
- `AssetMetadata::FilePath` 语义明确为稳定逻辑路径字符串。
- Registry key 仍然保持为 `AssetRegistry` 内部实现细节。
- 真实文件系统路径解析只发生在 `AssetSystemFileResolver` 边界。
- Engine Asset 内部路径可以继续稳定记录在 Metadata 中，同时不暴露为用户侧任意导入 API。

代价：

- 新增 `AssetPath.h/.cpp` 两个文件。
- 当前仍然依赖约束保证外部不绕过对应入口。
- `std::string` 仍然不是强类型的已规范化路径。

## Rules

- `AssetPath` 只负责相对路径规范化。
- `AssetPath` 不生成 Registry key。
- `AssetPath` 不解析绝对文件系统路径。
- `AssetPath` 不检查文件是否存在。
- `AssetPath` 不负责 Asset 注册。
- `AssetPath` 不负责 Asset 加载。
- `AssetPath` 不决定 Engine Asset 是否公开。
- `AssetPath::TryNormalizeRelativePath()` 输出的字符串统一使用 `/`。
- `AssetMetadata::FilePath` 保存 `AssetPath::TryNormalizeRelativePath()` 输出的字符串。
- 后续路径输入入口应优先经过 `AssetPath::TryNormalizeRelativePath()`。
- Registry key 生成必须保留在 `AssetRegistry` 内部。
- 真实文件系统路径解析必须保留在 `AssetSystemFileResolver` 边界。
