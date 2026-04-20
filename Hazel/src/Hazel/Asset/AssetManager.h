#pragma once
#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Asset/AssetRegistry.h>
#include <Hazel/Asset/AssetSerializer.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
namespace Hazel
{
class HAZEL_API AssetManager
{
public:
    static void Init();
    static void Shutdown();

    static AssetHandle ImportAsset(const std::filesystem::path& relativePath);

    static const AssetMetadata* GetMetadata(AssetHandle handle);
    static AssetMetadata* GetMetadataMutable(AssetHandle handle);

    static std::filesystem::path GetFileSystemPath(AssetHandle handle);
    static std::filesystem::path GetFileSystemPath(const AssetMetadata& metadata);

    static bool IsAssetHandleValid(AssetHandle handle);
    static bool IsAssetLoaded(AssetHandle handle);

    template <typename T> static Ref<T> GetAsset(AssetHandle handle)
    {
        auto loadedIt = s_LoadedAssets.find(handle);
        if (loadedIt != s_LoadedAssets.end())
        {
            return loadedIt->second.As<T>();
        }

        AssetMetadata* metadata = GetMetadataMutable(handle);
        if (!metadata)
        {
            return nullptr;
        }

        Ref<Asset> asset = LoadAsset(*metadata);
        if (!asset)
        {
            return nullptr;
        }
        metadata->IsLoaded = true;
        s_LoadedAssets[handle] = asset;
        return asset.As<T>();
    }

private:
    static AssetHandle GenerateHandle();
    static AssetType GetAssetTypeFromExtension(const std::filesystem::path& path);
    static Ref<Asset> LoadAsset(const AssetMetadata& metadata);

private:
    static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
    static std::unordered_map<AssetType, Scope<AssetSerializer>> s_Serializers;
    static Ref<AssetRegistry> s_AssetRegistry;
};
} // namespace Hazel