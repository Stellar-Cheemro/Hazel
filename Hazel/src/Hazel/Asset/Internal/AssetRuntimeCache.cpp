#include "AssetRuntimeCache.h"

#include <Hazel/Core/Log.h>

#include <utility>

namespace Hazel
{

void AssetRuntimeCache::Clear()
{
    m_LoadedAssets.clear();
}

bool AssetRuntimeCache::IsLoaded(AssetHandle Handle) const
{
    return m_LoadedAssets.find(Handle) != m_LoadedAssets.end();
}

Ref<Asset> AssetRuntimeCache::Get(AssetHandle Handle) const
{
    auto it = m_LoadedAssets.find(Handle);
    if (it == m_LoadedAssets.end())
        return nullptr;

    return it->second;
}

bool AssetRuntimeCache::Store(AssetHandle Handle, Ref<Asset> asset)
{
    if (Handle == 0)
    {
        HAZEL_CORE_ERROR("Fail:store runtime asset. Invalid handle. Handle: {0}", Handle);
        return false;
    }

    if (!asset)
    {
        HAZEL_CORE_ERROR("Fail:store runtime asset. Asset is null. Handle: {0}", Handle);
        return false;
    }

    m_LoadedAssets[Handle] = std::move(asset);
    return true;
}

void AssetRuntimeCache::Remove(AssetHandle Handle)
{
    m_LoadedAssets.erase(Handle);
}

} // namespace Hazel