#pragma once
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <filesystem>
#include <unordered_map>

namespace Hazel
{
class HAZEL_API AssetRegistry : public RefCounted
{
public:
    bool IsContained(AssetHandle handle) const;
    bool IsContained(const std::filesystem::path& path) const;

    const AssetMetadata* GetMetadata(AssetHandle handle) const;
    AssetMetadata* GetMetadata(AssetHandle handle);
    AssetHandle GetHandleFromPath(const std ::filesystem::path& relativePath) const;

    void RegisterAsset(const AssetMetadata& metadata);
    bool RemoveAsset(AssetHandle handle);
    void Clear();

private:
    static std::filesystem::path NormalizePath(const std::filesystem::path& path);

private:
    std::unordered_map<AssetHandle, AssetMetadata> m_HandleToMetadata;
    std::unordered_map<std::filesystem::path, AssetHandle> m_PathToHandle;
};
} // namespace Hazel
