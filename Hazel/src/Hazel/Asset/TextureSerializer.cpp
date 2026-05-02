#include "TextureSerializer.h"
#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/TextureAsset.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Texture.h>
#include <filesystem>
namespace Hazel
{
bool Hazel::TextureAssetSerializer::TrySerialize(const AssetMetadata& metadata, Ref<Asset>& asset)
{
    if (metadata.Type != AssetType::Texture2D)
    {
        return false;
    }
    std::filesystem::path FullPath = AssetManager::GetFileSystemPath(metadata);
    if (FullPath.empty())
    {
        HAZEL_CORE_ERROR("Texture file does not exist: {0} while trying to serialize",
                         FullPath.string());
        return false;
    }

    Ref<Texture2D> texture = Texture2D::Create(FullPath.string());
    if (!texture)
    {
        HAZEL_CORE_ERROR("Failed to create texture from file: {0} while trying to serialize",
                         FullPath.string());
        return false;
    }

    Ref<TextureAsset> textureAsset = CreateRef<TextureAsset>(texture);
    textureAsset->Handle = metadata.handle;

    asset = textureAsset.As<Asset>();
    return true;
}
} // namespace Hazel