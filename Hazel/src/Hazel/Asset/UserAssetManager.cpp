#include "UserAssetManager.h"

#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Core/Log.h>

#include <utility>

namespace Hazel
{

namespace
{

bool IsProjectFileAssetForOperation(const AssetMetadata& metadata, std::string_view operation)
{
    if (metadata.IsProjectAsset() && metadata.IsFileAsset())
        return true;

    HAZEL_CORE_ERROR("Fail:{0}. Asset is not ProjectAsset. Handle: {1}, Domain: {2}, FilePath: {3}",
                     operation, metadata.Handle, static_cast<int>(metadata.Domain),
                     metadata.FilePath);
    return false;
}

bool IsMemoryAssetForOperation(const AssetMetadata& metadata, std::string_view operation)
{
    if (metadata.IsMemoryAsset())
        return true;

    HAZEL_CORE_ERROR("Fail:{0}. Asset is not MemoryAsset. Handle: {1}, Domain: {2}", operation,
                     metadata.Handle, static_cast<int>(metadata.Domain));
    return false;
}

} // namespace

// ----------------------------------------------------------------------------
// ProjectAsset
// ----------------------------------------------------------------------------

AssetHandle UserAssetManager::ImportProjectAsset(std::string_view relativePath)
{
    return AssetManager::ImportProjectAsset(relativePath);
}

// ----------------------------------------------------------------------------
// MemoryAsset
// ----------------------------------------------------------------------------

AssetHandle UserAssetManager::RegisterMemoryAssetInternal(Ref<Asset> asset)
{
    return AssetManager::RegisterMemoryAsset(std::move(asset));
}

// ----------------------------------------------------------------------------
// 获取 Asset
// ----------------------------------------------------------------------------

Ref<Asset> UserAssetManager::GetAssetInternal(AssetHandle Handle)
{
    return AssetManager::GetAsset(Handle);
}

// ----------------------------------------------------------------------------
// Metadata 查询
// ----------------------------------------------------------------------------

const AssetMetadata* UserAssetManager::GetMetadata(AssetHandle Handle)
{
    return AssetManager::GetMetadata(Handle);
}

// ----------------------------------------------------------------------------
// 状态查询
// ----------------------------------------------------------------------------

bool UserAssetManager::IsAssetHandleValid(AssetHandle Handle)
{
    return AssetManager::IsAssetHandleValid(Handle);
}

bool UserAssetManager::IsAssetLoaded(AssetHandle Handle)
{
    return AssetManager::IsAssetLoaded(Handle);
}

// ----------------------------------------------------------------------------
// 生命周期操作
// ----------------------------------------------------------------------------

bool UserAssetManager::TryUnloadProjectAsset(AssetHandle Handle)
{
    const AssetMetadata* metadata = AssetManager::GetMetadata(Handle);

    if (!metadata)
        return false;

    if (!IsProjectFileAssetForOperation(*metadata, "unload project asset"))
        return false;

    return AssetManager::TryUnloadAsset(Handle);
}

bool UserAssetManager::TryRemoveProjectAsset(AssetHandle Handle)
{
    const AssetMetadata* metadata = AssetManager::GetMetadata(Handle);

    if (!metadata)
        return false;

    if (!IsProjectFileAssetForOperation(*metadata, "remove project asset"))
        return false;

    return AssetManager::TryRemoveAsset(Handle);
}

bool UserAssetManager::TryRemoveMemoryAsset(AssetHandle Handle)
{
    const AssetMetadata* metadata = AssetManager::GetMetadata(Handle);

    if (!metadata)
        return false;

    if (!IsMemoryAssetForOperation(*metadata, "remove memory asset"))
        return false;

    return AssetManager::TryRemoveAsset(Handle);
}

} // namespace Hazel