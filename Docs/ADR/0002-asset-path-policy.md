# 0002. 使用 AssetPath 统一 Asset 相对路径规范

Date: 2026-05-03

Status: Accepted

## Context

当前 Asset 系统中路径规范化逻辑分散在多个地方：

- `AssetManager::ImportAsset()` 直接调用 `relativePath.lexically_normal()`。
- `AssetRegistry` 内部有 `NormalizePath()`。
- `AssetMetadata::GetFileSystemPath()` 实际也只是做 `FilePath.lexically_normal()`，并不是真正文件系统路径解析。

随着设计明确，`AssetMetadata::FilePath` 的语义已经不是“操作系统路径对象”，而是“Asset 系统内部稳定相对路径标识”。

如果继续让各模块各自处理路径，后续容易出现规则不一致，例如：

- 有的地方允许绝对路径。
- 有的地方允许 `..`。
- 有的地方使用平台相关分隔符。
- 有的地方使用 `/`。
- Engine Asset 和 Project Asset 路径规则不一致。
- Metadata、Registry、序列化之间路径表达不一致。

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

`AssetMetadata::FilePath` 使用 `std::string`，保存 AssetPath 输出的稳定相对路径字符串。

真实文件系统边界由 `AssetSystemFileResolver` 负责。

当 Engine / Project 文件资源需要解析到真实文件系统路径时，`AssetSystemFileResolver` 会在边界处将 `AssetMetadata::FilePath` 临时转换为 `std::filesystem::path` 并拼接对应根目录。

## Relationship With AssetSystemFileResolver

`AssetPath` 和 `AssetSystemFileResolver` 的边界如下：

```text
AssetPath:
    原始输入路径字符串
    -> 规范化 Asset 逻辑相对路径字符串

AssetSystemFileResolver:
    AssetMetadata + Domain Root
    -> 真实文件系统路径
```

`AssetPath` 不知道 Engine Asset Root，也不访问 `Project::GetActive()`。

`AssetSystemFileResolver` 不重新定义 Asset 逻辑路径规范，它只接收已经写入 `AssetMetadata::FilePath` 的规范化相对路径，并在真实文件系统边界转换为 `std::filesystem::path`。

## Alternatives Considered

### 方案 A：继续让各模块各自调用 `lexically_normal()`

Rejected.

原因：

- 路径规则会继续分散。
- 后续不同模块可能出现不同校验规则。
- 不利于长期维护。
- 无法清晰保证写入 `AssetMetadata::FilePath` 的路径一定符合 Asset 系统规则。

### 方案 B：新增 `NormalizeTool`

Rejected.

原因：

- `Normalize` 是动作，不是领域。
- 路径规范化、字符串规范化、Shader 源码规范化等规则完全不同。
- 容易变成杂物工具类。
- 会让低层工具文件逐渐依赖越来越多的高层模块。

### 方案 C：`AssetPath` 使用 header-only inline 多函数

Rejected.

原因：

- `TryNormalizeRelativePath()` 逻辑不够短，`inline` 不保证实际内联优化。
- header-only 会增加编译依赖传播。
- 多函数接口容易让调用方错误组合，例如绕过规范化流程直接转稳定字符串。
- 当前阶段不需要为了减少一个 `.cpp` 文件牺牲接口边界。

### 方案 D：`AssetPath` 使用 `std::filesystem::path` 做规范化

Rejected.

原因：

- 会造成 `string -> filesystem::path -> string` 的冗余转换。
- Asset 路径是引擎内部逻辑路径，不应依赖操作系统路径语义。
- 不利于保持 Metadata、Registry、序列化之间的稳定字符串表达。

### 方案 E：`AssetPath.h/.cpp` + 单一 `TryNormalizeRelativePath()` 接口，使用纯字符串规则输出 `std::string`

Accepted.

原因：

- 接口收敛。
- 职责清楚。
- 编译依赖较小。
- 当前阶段改动可控。
- 输出结果可直接写入 `AssetMetadata::FilePath`。
- Registry 可直接基于稳定字符串生成内部 key。
- 真实文件系统边界再由 `AssetSystemFileResolver` 临时转换为 `std::filesystem::path`，职责更清楚。
- 避免路径规范化阶段出现多次 string/path 往返转换。

### 方案 F：引入 `AssetRelativePath` 值对象

Rejected for now.

原因：

- 长期更类型安全。
- 但当前阶段会扩大改动面。
- 需要同时修改 `AssetMetadata`、`AssetRegistry`、`AssetManager` 等多个模块。
- 当前职责重构还不需要这么重的路径类型系统。

## Consequences

正面影响：

- Asset 相对路径规范有唯一来源。
- `AssetManager` 和 `AssetRegistry` 后续不再各自实现不同路径规范化规则。
- `AssetMetadata::FilePath` 语义明确为稳定逻辑路径字符串。
- 引擎内部流程通过 `AssetManager` 使用 Asset 系统。
- 用户项目通过 `UserAssetManager` 使用 Asset 系统。
- Registry key 仍然保持为 `AssetRegistry` 内部实现细节。
- 真实文件系统路径解析只发生在 `AssetSystemFileResolver` 边界。
- 路径规范化流程不再依赖 `std::filesystem::path`。

代价：

- 新增 `AssetPath.h/.cpp` 两个文件。
- 当前仍然依赖约束保证外部不绕过对应入口：
  - 引擎内部通过 `AssetManager` 使用完整 Asset 系统能力。
  - 用户项目通过 `UserAssetManager` 使用用户侧 Asset API。
- `std::string` 仍然不是强类型的已规范化路径，后续如有需要可升级为 `AssetRelativePath` 值对象。

## Rules

- `AssetPath` 只负责相对路径规范化。
- `AssetPath` 不生成 Registry key。
- `AssetPath` 不解析绝对文件系统路径。
- `AssetPath` 不检查文件是否存在。
- `AssetPath` 不负责 Asset 注册。
- `AssetPath` 不负责 Asset 加载。
- `AssetPath::TryNormalizeRelativePath()` 输出的字符串统一使用 `/`。
- `AssetMetadata::FilePath` 保存 `AssetPath::TryNormalizeRelativePath()` 输出的字符串。
- 后续路径输入入口应优先经过 `AssetPath::TryNormalizeRelativePath()`。
- 引擎内部代码通过 `AssetManager` 使用完整 Asset 系统能力。
- 用户项目代码通过 `UserAssetManager` 使用用户侧 Asset API。
- Registry key 生成必须保留在 `AssetRegistry` 内部。
- 真实文件系统路径解析必须保留在 `AssetSystemFileResolver` 边界。

## Current Recommended Code

### AssetPath.h

```cpp
#pragma once

#include <Hazel/Core/Core.h>

#include <string>
#include <string_view>

namespace Hazel::AssetPath
{
    // 尝试将输入路径转换为 Asset 系统使用的规范化相对路径字符串。
    HAZEL_API bool TryNormalizeRelativePath(
        std::string_view inputPath,
        std::string& outNormalizedPath);
}
```

### AssetPath.cpp

```cpp
#include "AssetPath.h"

#include <string>
#include <utility>

namespace Hazel::AssetPath
{
    namespace
    {
        bool IsSeparator(char c)
        {
            return c == '/' || c == '\\';
        }

        bool IsDisallowedCharacter(char c)
        {
            const unsigned char value = static_cast<unsigned char>(c);

            if (value < 0x20)
                return true;

            if (c == ':')
                return true;

            switch (c)
            {
                case '*':
                case '?':
                case '"':
                case '<':
                case '>':
                case '|':
                    return true;
                default:
                    return false;
            }
        }
    }

    bool TryNormalizeRelativePath(
        std::string_view inputPath,
        std::string& outNormalizedPath);
}
```

完整实现以代码文件为准。
