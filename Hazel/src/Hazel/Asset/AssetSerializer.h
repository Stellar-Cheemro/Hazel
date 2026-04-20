#pragma once
#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>
namespace Hazel
{
class HAZEL_API AssetSerializer
{
public:
    virtual ~AssetSerializer() = default;
    virtual bool TrySerialize(const AssetMetadata& metadata, Ref<Asset>& asset) = 0;
};
} // namespace Hazel