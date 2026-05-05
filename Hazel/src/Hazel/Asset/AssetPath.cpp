#include "AssetPath.h"

#include <string>
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

    // 不允许空字符和 ASCII 控制字符。
    if (value < 0x20)
        return true;

    // 不允许冒号。
    // 这样可以拒绝 Windows 盘符路径，例如 "C:/Assets/A.png"，
    // 也可以拒绝 URI 风格路径，例如 "engine://A.png"。
    if (c == ':')
        return true;

    // 不允许常见跨平台不安全文件名字符。
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

bool HasDisallowedCharacter(std::string_view path)
{
    for (char c : path)
    {
        if (IsDisallowedCharacter(c))
            return true;
    }

    return false;
}

bool IsAbsoluteLikePath(std::string_view path)
{
    if (path.empty())
        return false;

    // 拒绝 Unix 风格绝对路径：/A/B
    // 拒绝 Windows 根路径或 UNC 风格路径：\A\B 或 \\Server\Share
    return IsSeparator(path.front());
}

char ToUpperAscii(char c)
{
    if (c >= 'a' && c <= 'z')
        return static_cast<char>(c - 'a' + 'A');

    return c;
}

bool EqualsIgnoreCaseAscii(std::string_view value, std::string_view expected)
{
    if (value.size() != expected.size())
        return false;

    for (size_t i = 0; i < value.size(); ++i)
    {
        if (ToUpperAscii(value[i]) != expected[i])
            return false;
    }

    return true;
}

std::string_view GetStemBeforeExtension(std::string_view segment)
{
    const size_t dotPosition = segment.find('.');
    if (dotPosition == std::string_view::npos)
        return segment;

    return segment.substr(0, dotPosition);
}

bool IsWindowsReservedName(std::string_view segment)
{
    const std::string_view stem = GetStemBeforeExtension(segment);

    if (EqualsIgnoreCaseAscii(stem, "CON"))
        return true;
    if (EqualsIgnoreCaseAscii(stem, "PRN"))
        return true;
    if (EqualsIgnoreCaseAscii(stem, "AUX"))
        return true;
    if (EqualsIgnoreCaseAscii(stem, "NUL"))
        return true;

    if (stem.size() == 4)
    {
        const char first = ToUpperAscii(stem[0]);
        const char second = ToUpperAscii(stem[1]);
        const char third = ToUpperAscii(stem[2]);
        const char fourth = stem[3];

        if (fourth >= '1' && fourth <= '9')
        {
            if (first == 'C' && second == 'O' && third == 'M')
                return true;

            if (first == 'L' && second == 'P' && third == 'T')
                return true;
        }
    }

    return false;
}

bool IsInvalidSegment(std::string_view segment)
{
    if (segment.empty())
        return false;

    if (segment == ".")
        return false;

    if (segment == "..")
        return true;

    // 避免真实文件系统边界出现尾随空格或尾随 "." 的跨平台问题。
    const char last = segment.back();
    if (last == ' ' || last == '.')
        return true;

    if (IsWindowsReservedName(segment))
        return true;

    return false;
}

bool AppendSegment(std::string& result, std::string_view segment)
{
    if (segment.empty() || segment == ".")
        return true;

    if (IsInvalidSegment(segment))
        return false;

    if (!result.empty())
        result += '/';

    result.append(segment.data(), segment.size());
    return true;
}
} // namespace

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------
bool TryNormalizeRelativePath(std::string_view inputPath, std::string& outNormalizedPath)
{
    // 1. 检查输入是否为空
    // 2. 检查是否包含非法字符
    // 3. 检查是否像绝对路径
    if (inputPath.empty())
        return false;
    if (HasDisallowedCharacter(inputPath))
        return false;
    if (IsAbsoluteLikePath(inputPath))
        return false;

    // 4. 按 "/" 或 "\\" 切分路径片段
    // 5. 忽略空片段和 "." 片段
    // 6. 遇到 ".." 直接失败
    // 7. 检查每个路径片段是否跨平台安全
    // 8. 用 "/" 重新拼接路径
    std::string normalizedPath;
    size_t segmentStart = 0;
    size_t index = 0;
    while (index <= inputPath.size())
    {
        if (index == inputPath.size() || IsSeparator(inputPath[index]))
        {
            const std::string_view segment = inputPath.substr(segmentStart, index - segmentStart);

            if (!AppendSegment(normalizedPath, segment))
                return false;

            segmentStart = index + 1;
        }

        ++index;
    }

    if (normalizedPath.empty())
        return false;
    outNormalizedPath = std::move(normalizedPath);
    return true;
}
} // namespace Hazel::AssetPath