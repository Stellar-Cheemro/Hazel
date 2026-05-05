#pragma once

#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Containers.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>

namespace Hazel
{

class HAZEL_API AssetRuntimeCache
{
public:
    void Clear();

    bool IsLoaded(AssetHandle Handle) const;

    Ref<Asset> Get(AssetHandle Handle) const;

    bool Store(AssetHandle Handle, Ref<Asset> asset);

    void Remove(AssetHandle Handle);

private:
    HashMap<AssetHandle, Ref<Asset>> m_LoadedAssets;
};

} // namespace Hazel