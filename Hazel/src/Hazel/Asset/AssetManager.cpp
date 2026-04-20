#include "AssetManager.h"
#include <Hazel/Asset/TextureSerializer.h>
#include <Hazel/Core/Log.h>
#include <Hazel/Project/Project.h>
#include <filesystem>

namespace Hazel
{
Ref<AssetRegistry> AssetManager::s_AssetRegistry = nullptr;
std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::s_LoadedAssets;
std::unordered_map<AssetType, Scope<AssetSerializer>> AssetManager::s_Serializers;

void AssetManager::Init()
{
    s_AssetRegistry = Ref<AssetRegistry>::Create();
    s_LoadedAssets.clear();
    s_Serializers.clear();

    s_Serializers[AssetType::Texture2D] = std::make_unique<TextureAssetSerializer>();
}
void AssetManager::Shutdown()
{
    s_LoadedAssets.clear();
    s_AssetRegistry = nullptr;
    s_Serializers.clear();
}
AssetHandle AssetManager::GenerateHandle()
{
    // 0默认unvalid
    static AssetHandle currentHandle = 1;
    return currentHandle++;
}
AssetType AssetManager::GetAssetTypeFromExtension(const std::filesystem::path& path)
{
    auto extension = path.extension().string();
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg")
        return AssetType::Texture2D;
    if (extension == ".shader")
        return AssetType::Shader;
    if (extension == ".scene")
        return AssetType::Scene;
    if (extension == ".mesh")
        return AssetType::Mesh;
    if (extension == ".audio")
        return AssetType::Audio;
    return AssetType::None;
}
Ref<Asset> AssetManager::LoadAsset(const AssetMetadata& metadata)
{
    auto serializerIt = s_Serializers.find(metadata.Type);
    if (serializerIt == s_Serializers.end())
    {
        HAZEL_CORE_ERROR("No serializer found for asset type: {0}",
                         static_cast<int>(metadata.Type));
        return nullptr;
    }
    Ref<Asset> asset;
    if (!serializerIt->second->TrySerialize(metadata, asset))
    {
        HAZEL_CORE_ERROR("Failed to serialize asset with handle: {0}", metadata.handle);
        return nullptr;
    }
    return asset;
}

AssetHandle AssetManager::ImportAsset(const std::filesystem::path& relaticePath)
{
    if (!s_AssetRegistry)
    {
        HAZEL_CORE_ERROR(
            "AssetRegistry not initialized. Call AssetManager::Init() before importing assets.");
        return 0;
    }

    auto normalizedPath = std::filesystem::weakly_canonical(relaticePath);
    if (s_AssetRegistry->IsContained(normalizedPath))
    {
        return s_AssetRegistry->GetHandleFromPath(normalizedPath);
    }
    else
    {
        AssetMetadata metadata;

        metadata.Type = GetAssetTypeFromExtension(normalizedPath);
        if (metadata.Type == AssetType::None)
        {
            HAZEL_CORE_ERROR("Unknown asset type for path: {0}", normalizedPath.string());
            return 0;
        }
        metadata.handle = GenerateHandle();
        metadata.FilePath = normalizedPath;
        s_AssetRegistry->RegisterAsset(metadata);
        return metadata.handle;
    }
}
const AssetMetadata* AssetManager::GetMetadata(AssetHandle handle)
{
    if (!s_AssetRegistry)
    {
        HAZEL_CORE_ERROR("AssetRegistry not initialized. Call AssetManager::Init() before "
                         "accessing asset metadata.");
        return nullptr;
    }
    return s_AssetRegistry->GetMetadata(handle);
}
AssetMetadata* AssetManager::GetMetadataMutable(AssetHandle handle)
{
    if (!s_AssetRegistry)
    {
        HAZEL_CORE_ERROR("AssetRegistry not initialized. Call AssetManager::Init() before "
                         "accessing asset metadata.");
        return nullptr;
    }
    return s_AssetRegistry->GetMetadata(handle);
}
bool AssetManager::IsAssetHandleValid(AssetHandle handle)
{
    if (!s_AssetRegistry)
    {
        HAZEL_CORE_ERROR("AssetRegistry not initialized. Call AssetManager::Init() before "
                         "checking asset handle validity.");
        return false;
    }
    return s_AssetRegistry->IsContained(handle);
}
bool AssetManager::IsAssetLoaded(AssetHandle handle)
{
    return s_LoadedAssets.find(handle) != s_LoadedAssets.end();
}

std::filesystem::path AssetManager::GetFileSystemPath(const AssetMetadata& metadata)
{
    Ref<Project> currentProject = Project::GetActive();
    if (!currentProject)
    {
        HAZEL_CORE_ERROR("No active project. Cannot get file system path for asset handle: {0}",
                         metadata.handle);
        return {};
    }
    return currentProject->GetAssetAbsolutePath(metadata.FilePath);
}
std::filesystem::path AssetManager::GetFileSystemPath(AssetHandle handle)
{
    const AssetMetadata* metadata = GetMetadata(handle);
    if (!metadata)
    {
        HAZEL_CORE_ERROR("Invalid asset handle: {0} In GetFileSystemPath", handle);
        return {};
    }
    return GetFileSystemPath(*metadata);
}
} // namespace Hazel