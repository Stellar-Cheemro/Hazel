#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace Hazel
{

using AssetHandle = uint64_t;

enum class AssetType : uint8_t
{
    None = 0,

    Texture2D,
    Shader,
    Scene,
    Mesh,
    Audio
};

enum class AssetDomain : uint8_t
{
    None = 0,

    Engine,
    Project,
    Memory
};

// 当前可从文件导入、并需要 Serializer 支持的 AssetType。
// MemoryAsset 不在这里，因为它不走文件导入和 Serializer。
inline constexpr std::array<AssetType, 2> SupportedFileAssetTypes = {
    AssetType::Texture2D, AssetType::Shader
    // , AssetType::Scene, AssetType::Mesh,
    // AssetType::Audio
};

inline constexpr bool IsFileAssetType(AssetType type)
{
    for (AssetType fileAssetType : SupportedFileAssetTypes)
    {
        if (fileAssetType == type)
            return true;
    }

    return false;
}

// 从规范化后的 Asset 相对路径中提取扩展名。
// 输入应当已经由 AssetPath::TryNormalizeRelativePath() 处理过。
// 这里只负责类型识别所需的扩展名提取，不负责路径合法性校验。
inline constexpr std::string_view GetAssetExtension(std::string_view normalizedAssetPath)
{
    const size_t slashPosition = normalizedAssetPath.find_last_of('/');
    const size_t dotPosition = normalizedAssetPath.find_last_of('.');

    if (dotPosition == std::string_view::npos)
        return {};

    if (dotPosition + 1 >= normalizedAssetPath.size())
        return {};

    if (slashPosition != std::string_view::npos && dotPosition < slashPosition)
        return {};

    return normalizedAssetPath.substr(dotPosition);
}

// 根据扩展名判断 AssetType。
// extension 应包含 "."，例如 ".png"、".glsl"。
inline constexpr AssetType GetAssetTypeFromExtension(std::string_view extension)
{
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
        return AssetType::Texture2D;

    if (extension == ".shader" || extension == ".glsl")
        return AssetType::Shader;

    if (extension == ".scene")
        return AssetType::Scene;

    if (extension == ".mesh")
        return AssetType::Mesh;

    if (extension == ".audio")
        return AssetType::Audio;

    return AssetType::None;
}

// 根据规范化后的 Asset 相对路径判断 AssetType。
inline constexpr AssetType GetAssetTypeFromPath(std::string_view normalizedAssetPath)
{
    return GetAssetTypeFromExtension(GetAssetExtension(normalizedAssetPath));
}
} // namespace Hazel
