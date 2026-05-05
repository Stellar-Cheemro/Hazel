// AssetPath 只负责 Asset 系统中的相对路径规范。
// 不负责 Registry key 生成，不负责绝对路径解析。
#pragma once

namespace Hazel::AssetPath
{
// 尝试将输入路径转换为 Asset 系统使用的规范化相对路径字符串。
//
// 规则：
// 1. 路径不能为空。
// 2. 路径不能是绝对路径。
// 3. 路径不能包含盘符或 URI 风格的冒号。
// 4. 路径中的 "\\" 会统一转换为 "/"。
// 5. "." 路径片段会被忽略。
// 6. 不允许出现 ".." 路径片段。
// 7. 输出统一使用 "/"。
//
// 跨平台约束：
// 1. 不允许 Windows 文件名非法字符：* ? " < > |
// 2. 不允许 ASCII 控制字符。
// 3. 不允许路径片段以空格或 "." 结尾。
// 4. 不允许 Windows 设备保留名，例如 CON、PRN、AUX、NUL、COM1、LPT1。
//
// 注意：
// 这里只负责 Asset 逻辑路径规范。
// 不负责 Registry key 生成。
// 不负责 Engine / Project 根目录拼接。
// 不负责检查文件是否真实存在。
bool TryNormalizeRelativePath(std::string_view inputPath, std::string& outNormalizedPath);
} // namespace Hazel::AssetPath