#pragma once

#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/Texture.h>

#include <string_view>

namespace Hazel
{

class Asset;

class HAZEL_API UserAssetManager
{
public:
    // ProjectAsset
    static AssetHandle ImportProjectAsset(std::string_view relativePath);

    // Load by explicit handle.
    // Handle 可以来自 ProjectAsset、MemoryAsset，也可以来自 EngineAssets。
    static Ref<Shader> LoadShader(AssetHandle handle);
    static Ref<Texture2D> LoadTexture2D(AssetHandle handle);

    // MemoryAsset
    static AssetHandle RegisterMemoryShader(const Ref<Shader>& shader);
    static AssetHandle RegisterMemoryTexture2D(const Ref<Texture2D>& texture);

    // State
    static bool IsAssetHandleValid(AssetHandle handle);
    static bool IsAssetLoaded(AssetHandle handle);

    // Lifecycle
    static bool TryUnloadProjectAsset(AssetHandle handle);
    static bool TryRemoveProjectAsset(AssetHandle handle);
    static bool TryRemoveMemoryAsset(AssetHandle handle);

private:
    static const AssetMetadata* GetMetadata(AssetHandle handle, std::string_view operation);

    static bool HasType(const AssetMetadata& metadata, AssetType expectedType,
                        std::string_view operation);

    static bool IsProjectFileAsset(const AssetMetadata& metadata, std::string_view operation);

    static bool IsMemoryAsset(const AssetMetadata& metadata, std::string_view operation);
};

} // namespace Hazel