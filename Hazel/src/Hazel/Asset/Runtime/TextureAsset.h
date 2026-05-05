#pragma once

#include <Hazel/Asset/Asset.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/Texture.h>

namespace Hazel
{

class HAZEL_API TextureAsset : public Asset
{
public:
    TextureAsset() = default;

    explicit TextureAsset(const Ref<Texture2D>& texture) : m_Texture(texture)
    {
    }

    AssetType GetAssetType() const override
    {
        return AssetType::Texture2D;
    }

    const Ref<Texture2D>& GetTexture() const
    {
        return m_Texture;
    }

private:
    Ref<Texture2D> m_Texture;
};

} // namespace Hazel