#include "TextureSerializer.h"

#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/Runtime/TextureAsset.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Texture.h>

#include <string>
#include <string_view>

namespace Hazel
{

namespace
{

bool TryLoadTexture2D(const AssetMetadata& metadata, std::string_view resolvedPath,
                      Ref<Asset>& asset)
{
    const std::string resolvedPathString(resolvedPath);

    Ref<Texture2D> texture = Texture2D::Create(resolvedPathString);
    if (!texture)
    {
        HAZEL_CORE_ERROR(
            "Fail:load texture asset. Texture2D creation failed. Handle: {0}, Path: {1}",
            metadata.Handle, resolvedPathString);
        return false;
    }

    Ref<TextureAsset> textureAsset = CreateRef<TextureAsset>(texture);
    textureAsset->Handle = metadata.Handle;

    asset = textureAsset.As<Asset>();
    return true;
}

bool TryLoadTextureAssetByType(const AssetMetadata& metadata, std::string_view resolvedPath,
                               Ref<Asset>& asset)
{
    switch (metadata.Type)
    {
        case AssetType::Texture2D:
            return TryLoadTexture2D(metadata, resolvedPath, asset);

            // 后续新增纹理类型时在这里分派：
            //
            // case AssetType::TextureCube:
            //     return TryLoadTextureCube(metadata, resolvedPath, asset);
            //
            // case AssetType::Texture2DArray:
            //     return TryLoadTexture2DArray(metadata, resolvedPath, asset);
            //
            // case AssetType::HDRTexture:
            //     return TryLoadHDRTexture(metadata, resolvedPath, asset);

        default:
            HAZEL_CORE_ERROR("Fail:load texture asset. No texture loader for AssetType. Handle: "
                             "{0}, Type: {1}, FilePath: {2}",
                             metadata.Handle, static_cast<int>(metadata.Type), metadata.FilePath);
            return false;
    }
}

} // namespace

bool TextureAssetSerializer::CanLoad(AssetType type) const
{
    switch (type)
    {
        case AssetType::Texture2D:
            return true;

            // 后续新增纹理类型时在这里扩展：
            //
            // case AssetType::TextureCube:
            // case AssetType::Texture2DArray:
            // case AssetType::HDRTexture:
            //     return true;

        default:
            return false;
    }
}

bool TextureAssetSerializer::TryLoad(const AssetMetadata& metadata, Ref<Asset>& asset)
{
    if (!CanLoad(metadata.Type))
    {
        HAZEL_CORE_ERROR(
            "Fail:load texture asset. Unsupported AssetType. Handle: {0}, Type: {1}, FilePath: {2}",
            metadata.Handle, static_cast<int>(metadata.Type), metadata.FilePath);
        return false;
    }

    const std::string resolvedPath = AssetManager::ResolveAssetPath(metadata);
    if (resolvedPath.empty())
        return false;

    return TryLoadTextureAssetByType(metadata, resolvedPath, asset);
}

} // namespace Hazel