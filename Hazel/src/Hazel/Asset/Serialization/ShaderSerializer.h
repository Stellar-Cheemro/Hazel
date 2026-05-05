#pragma once

#include <Hazel/Asset/Serialization/AssetSerializer.h>
#include <Hazel/Core/Core.h>

namespace Hazel
{

class HAZEL_API ShaderSerializer : public AssetSerializer
{
public:
    bool CanLoad(AssetType type) const override;
    bool TryLoad(const AssetMetadata& metadata, Ref<Asset>& asset) override;
};

} // namespace Hazel