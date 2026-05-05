#include "AssetRegistry.h"

#include <Hazel/Asset/AssetPath.h>
#include <Hazel/Core/Log.h>

#include <string>
#include <utility>

namespace Hazel
{

namespace
{
bool TryMakeDomainPathKey(std::string_view normalizedRelativePath, std::string& outKey,
                          AssetDomain domain)
{
    char domainPrefix = '\0';

    switch (domain)
    {
        case AssetDomain::Engine:
            domainPrefix = 'E';
            break;

        case AssetDomain::Project:
            domainPrefix = 'P';
            break;

        default:
            return false;
    }

    std::string verifiedPath;
    if (!AssetPath::TryNormalizeRelativePath(normalizedRelativePath, verifiedPath))
        return false;

    // Registry 不负责偷偷修正路径。
    // 如果传进来的不是已经规范化后的稳定路径，则拒绝生成 key。
    if (verifiedPath != normalizedRelativePath)
        return false;

    std::string key;
    key.reserve(2 + verifiedPath.size());
    key += domainPrefix;
    key += ':';
    key += verifiedPath;

    outKey = std::move(key);
    return true;
}
} // namespace

// ----------------------------------------------------------------------------
// 查询
// ----------------------------------------------------------------------------

bool AssetRegistry::IsContained(AssetHandle Handle) const
{
    return m_HandleToMetadata.find(Handle) != m_HandleToMetadata.end();
}

bool AssetRegistry::IsContained(std::string_view normalizedRelativePath, AssetDomain domain) const
{
    std::string key;
    if (!TryMakeDomainPathKey(normalizedRelativePath, key, domain))
        return false;

    return m_PathToHandle.find(key) != m_PathToHandle.end();
}

const AssetMetadata* AssetRegistry::GetMetadata(AssetHandle Handle) const
{
    auto it = m_HandleToMetadata.find(Handle);
    if (it == m_HandleToMetadata.end())
        return nullptr;

    return &it->second;
}

AssetMetadata* AssetRegistry::GetMetadata(AssetHandle Handle)
{
    auto it = m_HandleToMetadata.find(Handle);
    if (it == m_HandleToMetadata.end())
        return nullptr;

    return &it->second;
}

AssetHandle AssetRegistry::GetHandleFromPath(std::string_view normalizedRelativePath,
                                             AssetDomain domain) const
{
    std::string key;
    if (!TryMakeDomainPathKey(normalizedRelativePath, key, domain))
        return 0;

    auto it = m_PathToHandle.find(key);
    if (it == m_PathToHandle.end())
        return 0;

    return it->second;
}

// ----------------------------------------------------------------------------
// 注册
// ----------------------------------------------------------------------------

bool AssetRegistry::TryRegisterAsset(const AssetMetadata& metadata)
{
    if (!metadata.IsValid())
    {
        HAZEL_CORE_ERROR("Fail:register asset. Invalid metadata. Handle: {0}, Type: {1}, Domain: "
                         "{2}, FilePath: {3}",
                         metadata.Handle, static_cast<int>(metadata.Type),
                         static_cast<int>(metadata.Domain), metadata.FilePath);
        return false;
    }

    if (m_HandleToMetadata.find(metadata.Handle) != m_HandleToMetadata.end())
    {
        HAZEL_CORE_ERROR("Fail:register asset. Duplicate handle. Handle: {0}, FilePath: {1}",
                         metadata.Handle, metadata.FilePath);
        return false;
    }

    if (metadata.IsMemoryAsset())
        return TryRegisterMemoryAsset(metadata);

    if (metadata.IsFileAsset())
        return TryRegisterFileAsset(metadata);

    HAZEL_CORE_ERROR("Fail:register asset. Metadata is neither memory nor file asset. Handle: {0}, "
                     "Domain: {1}, FilePath: {2}",
                     metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
    return false;
}

bool AssetRegistry::TryRegisterMemoryAsset(const AssetMetadata& metadata)
{
    if (metadata.HasFilePath())
    {
        HAZEL_CORE_ERROR("Fail:register memory asset. MemoryAsset must not have FilePath. Handle: "
                         "{0}, FilePath: {1}",
                         metadata.Handle, metadata.FilePath);
        return false;
    }

    AssetMetadata storedMetadata;
    storedMetadata.Handle = metadata.Handle;
    storedMetadata.Type = metadata.Type;
    storedMetadata.Domain = AssetDomain::Memory;
    storedMetadata.FilePath.clear();

    m_HandleToMetadata.emplace(storedMetadata.Handle, std::move(storedMetadata));
    return true;
}

bool AssetRegistry::TryRegisterFileAsset(const AssetMetadata& metadata)
{
    std::string normalizedPath;
    if (!AssetPath::TryNormalizeRelativePath(metadata.FilePath, normalizedPath))
    {
        HAZEL_CORE_ERROR("Fail:register file asset. Invalid relative path. Handle: {0}, Domain: "
                         "{1}, FilePath: {2}",
                         metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
        return false;
    }

    std::string key;
    if (!TryMakeDomainPathKey(normalizedPath, key, metadata.Domain))
    {
        HAZEL_CORE_ERROR(
            "Fail:register file asset. Invalid path key. Handle: {0}, Domain: {1}, FilePath: {2}",
            metadata.Handle, static_cast<int>(metadata.Domain), normalizedPath);
        return false;
    }

    if (m_PathToHandle.find(key) != m_PathToHandle.end())
    {
        HAZEL_CORE_ERROR(
            "Fail:register file asset. Duplicate path key. Handle: {0}, Domain: {1}, FilePath: {2}",
            metadata.Handle, static_cast<int>(metadata.Domain), normalizedPath);
        return false;
    }

    AssetMetadata storedMetadata;
    storedMetadata.Handle = metadata.Handle;
    storedMetadata.Type = metadata.Type;
    storedMetadata.Domain = metadata.Domain;
    storedMetadata.FilePath = std::move(normalizedPath);

    const AssetHandle handle = storedMetadata.Handle;

    m_HandleToMetadata.emplace(handle, std::move(storedMetadata));
    m_PathToHandle.emplace(std::move(key), handle);

    return true;
}

// ----------------------------------------------------------------------------
// 移除
// ----------------------------------------------------------------------------

bool AssetRegistry::TryRemoveAsset(AssetHandle Handle)
{
    auto it = m_HandleToMetadata.find(Handle);
    if (it == m_HandleToMetadata.end())
    {
        HAZEL_CORE_ERROR("Fail:remove asset. Handle does not exist. Handle: {0}", Handle);
        return false;
    }

    const AssetMetadata metadata = it->second;

    if (metadata.IsFileAsset())
    {
        std::string key;
        if (!TryMakeDomainPathKey(metadata.FilePath, key, metadata.Domain))
        {
            HAZEL_CORE_ERROR(
                "Fail:remove asset. Invalid path key. Handle: {0}, Domain: {1}, FilePath: {2}",
                metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
            return false;
        }

        m_PathToHandle.erase(key);
    }

    m_HandleToMetadata.erase(it);
    return true;
}

// ----------------------------------------------------------------------------
// 清空
// ----------------------------------------------------------------------------

void AssetRegistry::Clear()
{
    m_HandleToMetadata.clear();
    m_PathToHandle.clear();
}

} // namespace Hazel