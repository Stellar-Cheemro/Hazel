#include "UserAssetManager.h"

#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/Runtime/ShaderAsset.h>
#include <Hazel/Asset/Runtime/TextureAsset.h>
#include <Hazel/Core/Log.h>

namespace Hazel
{

AssetHandle UserAssetManager::ImportProjectAsset(std::string_view relativePath)
{
    return AssetManager::ImportProjectAsset(relativePath);
}

Ref<Shader> UserAssetManager::LoadShader(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle, "load shader");
    if (!metadata)
        return nullptr;

    if (!HasType(*metadata, AssetType::Shader, "load shader"))
        return nullptr;

    Ref<ShaderAsset> shaderAsset = AssetManager::GetAsset<ShaderAsset>(handle);
    if (!shaderAsset)
        return nullptr;

    return shaderAsset->GetShader();
}

Ref<Texture2D> UserAssetManager::LoadTexture2D(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle, "load texture2d");
    if (!metadata)
        return nullptr;

    if (!HasType(*metadata, AssetType::Texture2D, "load texture2d"))
        return nullptr;

    Ref<TextureAsset> textureAsset = AssetManager::GetAsset<TextureAsset>(handle);
    if (!textureAsset)
        return nullptr;

    return textureAsset->GetTexture();
}

AssetHandle UserAssetManager::RegisterMemoryShader(const Ref<Shader>& shader)
{
    if (!shader)
    {
        HAZEL_CORE_ERROR("Fail:register memory shader. Shader is null.");
        return 0;
    }

    Ref<ShaderAsset> asset = CreateRef<ShaderAsset>(shader);
    return AssetManager::RegisterMemoryAsset(asset.As<Asset>());
}

AssetHandle UserAssetManager::RegisterMemoryTexture2D(const Ref<Texture2D>& texture)
{
    if (!texture)
    {
        HAZEL_CORE_ERROR("Fail:register memory texture2d. Texture2D is null.");
        return 0;
    }

    Ref<TextureAsset> asset = CreateRef<TextureAsset>(texture);
    return AssetManager::RegisterMemoryAsset(asset.As<Asset>());
}

bool UserAssetManager::IsAssetHandleValid(AssetHandle handle)
{
    return AssetManager::IsAssetHandleValid(handle);
}

bool UserAssetManager::IsAssetLoaded(AssetHandle handle)
{
    return AssetManager::IsAssetLoaded(handle);
}

bool UserAssetManager::TryUnloadProjectAsset(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle, "unload project asset");
    if (!metadata)
        return false;

    if (!IsProjectFileAsset(*metadata, "unload project asset"))
        return false;

    return AssetManager::TryUnloadAsset(handle);
}

bool UserAssetManager::TryRemoveProjectAsset(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle, "remove project asset");
    if (!metadata)
        return false;

    if (!IsProjectFileAsset(*metadata, "remove project asset"))
        return false;

    return AssetManager::TryRemoveAsset(handle);
}

bool UserAssetManager::TryRemoveMemoryAsset(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle, "remove memory asset");
    if (!metadata)
        return false;

    if (!IsMemoryAsset(*metadata, "remove memory asset"))
        return false;

    return AssetManager::TryRemoveAsset(handle);
}

const AssetMetadata* UserAssetManager::GetMetadata(AssetHandle handle, std::string_view operation)
{
    const AssetMetadata* metadata = AssetManager::GetMetadata(handle);
    if (metadata)
        return metadata;

    HAZEL_CORE_ERROR("Fail:{0}. Invalid asset handle. Handle: {1}", operation, handle);
    return nullptr;
}

bool UserAssetManager::HasType(const AssetMetadata& metadata, AssetType expectedType,
                               std::string_view operation)
{
    if (metadata.Type == expectedType)
        return true;

    HAZEL_CORE_ERROR(
        "Fail:{0}. AssetType mismatch. Handle: {1}, ExpectedType: {2}, ActualType: {3}", operation,
        metadata.Handle, static_cast<int>(expectedType), static_cast<int>(metadata.Type));

    return false;
}

bool UserAssetManager::IsProjectFileAsset(const AssetMetadata& metadata, std::string_view operation)
{
    if (metadata.IsProjectAsset() && metadata.IsFileAsset())
        return true;

    HAZEL_CORE_ERROR("Fail:{0}. Asset is not ProjectAsset. Handle: {1}, Domain: {2}", operation,
                     metadata.Handle, static_cast<int>(metadata.Domain));

    return false;
}

bool UserAssetManager::IsMemoryAsset(const AssetMetadata& metadata, std::string_view operation)
{
    if (metadata.IsMemoryAsset())
        return true;

    HAZEL_CORE_ERROR("Fail:{0}. Asset is not MemoryAsset. Handle: {1}, Domain: {2}", operation,
                     metadata.Handle, static_cast<int>(metadata.Domain));

    return false;
}

} // namespace Hazel