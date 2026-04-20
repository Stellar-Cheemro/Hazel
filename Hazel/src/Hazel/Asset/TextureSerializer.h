#pragma once
#include <Hazel/Asset/AssetSerializer.h>
#include <Hazel/Core/Core.h>
namespace Hazel
{
class HAZEL_API TextureAssetSerializer : public AssetSerializer
{
public:
    bool TrySerialize(const AssetMetadata& metadata, Ref<Asset>& asset) override;
};
} // namespace Hazel