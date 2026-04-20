#include "AssetRegistry.h"

namespace Hazel
{
bool AssetRegistry::IsContained(AssetHandle handle) const
{
    return m_HandleToMetadata.find(handle) != m_HandleToMetadata.end();
}

bool AssetRegistry::IsContained(const std::filesystem::path& path) const
{
    auto normalizedPath = NormalizePath(path);
    return m_PathToHandle.find(normalizedPath) != m_PathToHandle.end();
}

const AssetMetadata* AssetRegistry::GetMetadata(AssetHandle handle) const
{
    auto it = m_HandleToMetadata.find(handle);
    if (it != m_HandleToMetadata.end())
    {
        return &it->second;
    }
    return nullptr;
}
AssetMetadata* AssetRegistry::GetMetadata(AssetHandle handle)
{
    auto it = m_HandleToMetadata.find(handle);
    if (it != m_HandleToMetadata.end())
    {
        return &it->second;
    }
    return nullptr;
}

AssetHandle AssetRegistry::GetHandleFromPath(const std::filesystem::path& relativePath) const
{
    auto normalizedPath = NormalizePath(relativePath);
    auto it = m_PathToHandle.find(normalizedPath);
    if (it != m_PathToHandle.end())
    {
        return it->second;
    }
    return 0; // Return 0 if not found, indicating an invalid handle
}

void AssetRegistry::RegisterAsset(const AssetMetadata& metadata)
{
    if (!metadata)
        return;
    AssetMetadata storedMetadata = metadata;
    auto normalizedPath = NormalizePath(metadata.FilePath);
    storedMetadata.FilePath = normalizedPath;

    m_HandleToMetadata[metadata.handle] = storedMetadata;
    m_PathToHandle[normalizedPath] = metadata.handle;
}
bool AssetRegistry::RemoveAsset(AssetHandle handle)
{
    auto it = m_HandleToMetadata.find(handle);
    if (it == m_HandleToMetadata.end())
        return false;

    auto normalizedPath = NormalizePath(it->second.FilePath);
    m_HandleToMetadata.erase(it);
    m_PathToHandle.erase(normalizedPath);
    return true;
}

void AssetRegistry::Clear()
{
    m_HandleToMetadata.clear();
    m_PathToHandle.clear();
}

std::filesystem::path AssetRegistry::NormalizePath(const std::filesystem::path& path)
{
    return path.lexically_normal();
}
} // namespace Hazel