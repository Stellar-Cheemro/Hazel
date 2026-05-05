#pragma once
#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
namespace Hazel
{
class HAZEL_API AssetSerializer
{
public:
    virtual ~AssetSerializer() = default;
    virtual bool CanLoad(AssetType type) const = 0;
    virtual bool TryLoad(const AssetMetadata& metadata, Ref<Asset>& asset) = 0;
};
} // namespace Hazel