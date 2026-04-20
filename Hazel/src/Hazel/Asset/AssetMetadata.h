#pragma once
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Core.h>
#include <filesystem>
namespace Hazel
{
struct HAZEL_API AssetMetadata
{
    AssetHandle handle = 0;
    AssetType Type = AssetType::None;
    std::filesystem::path FilePath;
    bool IsLoaded = false;
    bool IsMemoryOnly = false;

    operator bool() const
    {
        return handle != 0 && Type != AssetType::None;
    }
    bool IsValid() const
    {
        return handle != 0 && Type != AssetType::None;
    }
    bool HasFilePath() const
    {
        return !FilePath.empty();
    }
    bool IsFileAsset() const
    {
        return HasFilePath() && !IsMemoryOnly;
    }
    bool IsMemoryAsset() const
    {
        return IsMemoryOnly;
    }
    std::filesystem::path GetFileSystemPath() const
    {
        return FilePath.lexically_normal();
    }
};
} // namespace Hazel