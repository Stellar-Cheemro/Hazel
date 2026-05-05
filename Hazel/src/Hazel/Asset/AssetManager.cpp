#include "AssetManager.h"

#include <Hazel/Asset/AssetPath.h>
#include <Hazel/Asset/AssetRegistry.h>
#include <Hazel/Asset/Internal/AssetHandleAllocator.h>
#include <Hazel/Asset/Internal/AssetRootLocator.h>
#include <Hazel/Asset/Internal/AssetRuntimeCache.h>
#include <Hazel/Asset/Internal/AssetSerializerRegistry.h>
#include <Hazel/Asset/Internal/AssetSystemFileResolver.h>
#include <Hazel/Asset/Serialization/ShaderSerializer.h>
#include <Hazel/Asset/Serialization/TextureSerializer.h>

#include <Hazel/Core/Log.h>

#include <string>

namespace Hazel
{

Ref<AssetRegistry> AssetManager::s_AssetRegistry = nullptr;

AssetRuntimeCache AssetManager::s_RuntimeCache;
AssetSerializerRegistry AssetManager::s_SerializerRegistry;
AssetHandleAllocator AssetManager::s_HandleAllocator;
AssetSystemFileResolver AssetManager::s_SystemFileResolver;

// ----------------------------------------------------------------------------
// 生命周期
// ----------------------------------------------------------------------------

void AssetManager::Init()
{
    ResetState();
    SetEngineAssetRootAuto();
    RegisterSerializers();
    ValidateSerializers();
}

void AssetManager::Init(const std::filesystem::path& engineAssetRoot)
{
    ResetState();
    SetEngineAssetRootExplicit(engineAssetRoot);
    RegisterSerializers();
    ValidateSerializers();
}

void AssetManager::Shutdown()
{
    s_RuntimeCache.Clear();
    s_SerializerRegistry.Clear();
    s_HandleAllocator.Reset();
    s_SystemFileResolver.Clear();

    s_AssetRegistry = nullptr;
}

void AssetManager::ResetState() // 重置缓存、Registry、HandleAllocator、Resolver
{
    s_AssetRegistry = CreateRef<AssetRegistry>();
    s_RuntimeCache.Clear();
    s_SerializerRegistry.Clear();
    s_HandleAllocator.Reset();
    s_SystemFileResolver.Clear();
}

void AssetManager::SetEngineAssetRootAuto() // 自动查找 EngineAssetRoot
{
    auto root = AssetRootLocator::FindEngineAssetRoot(std::filesystem::current_path());
    if (!root)
    {
        HAZEL_CORE_ERROR("Fail: locate engine asset root. Current path: {0}",
                         std::filesystem::current_path().string());
        return;
    }

    SetEngineAssetRootExplicit(*root);
}
void AssetManager::SetEngineAssetRootExplicit(const std::filesystem::path& rootPath)
{
    if (!s_SystemFileResolver.SetEngineAssetRoot(rootPath))
    {
        HAZEL_CORE_ERROR("Fail: set engine asset root. Path invalid: {0}", rootPath.string());
    }
}
void AssetManager::RegisterSerializers()
{
    s_SerializerRegistry.Register(CreateScope<TextureAssetSerializer>());
    s_SerializerRegistry.Register(CreateScope<ShaderSerializer>());
}
void AssetManager::ValidateSerializers()
{
    s_SerializerRegistry.Validate();
}

// ----------------------------------------------------------------------------
// 文件资源
// ----------------------------------------------------------------------------

AssetHandle AssetManager::ImportEngineAsset(std::string_view relativePath)
{
    return ImportFileAsset(relativePath, AssetDomain::Engine);
}

AssetHandle AssetManager::ImportProjectAsset(std::string_view relativePath)
{
    return ImportFileAsset(relativePath, AssetDomain::Project);
}

AssetHandle AssetManager::ImportFileAsset(std::string_view relativePath, AssetDomain domain)
{
    if (!IsRegistryReady("import file asset"))
        return 0;

    if (!IsValidImportDomain(domain, relativePath))
        return 0;

    std::string normalizedPath;
    if (!AssetPath::TryNormalizeRelativePath(relativePath, normalizedPath))
    {
        HAZEL_CORE_ERROR("Fail:import file asset. Invalid relative path. Path: {0}",
                         std::string(relativePath));
        return 0;
    }

    if (s_AssetRegistry->IsContained(normalizedPath, domain))
        return s_AssetRegistry->GetHandleFromPath(normalizedPath, domain);

    const AssetType type = GetAssetTypeFromPath(normalizedPath);
    if (type == AssetType::None)
    {
        HAZEL_CORE_ERROR("Fail:import file asset. Unknown asset type. Path: {0}", normalizedPath);
        return 0;
    }

    AssetMetadata metadata;
    metadata.Handle = s_HandleAllocator.Generate(*s_AssetRegistry);
    if (metadata.Handle == 0)
        return 0;

    metadata.Type = type;
    metadata.Domain = domain;
    metadata.FilePath = normalizedPath;

    const std::string resolvedPath = s_SystemFileResolver.ResolveAssetPath(metadata);

    if (resolvedPath.empty())
        return 0;

    if (!s_SystemFileResolver.ValidateExistingFile(resolvedPath, "asset file"))
        return 0;

    if (!s_AssetRegistry->TryRegisterAsset(metadata))
        return 0;

    return metadata.Handle;
}

bool AssetManager::TryRestoreAssetMetadata(const AssetMetadata& metadata)
{
    if (!IsRegistryReady("restore asset metadata"))
        return false;

    if (!IsValidRestoredFileMetadata(metadata))
        return false;

    if (!s_AssetRegistry->TryRegisterAsset(metadata))
        return false;

    s_HandleAllocator.AdvancePast(metadata.Handle);
    return true;
}

// ----------------------------------------------------------------------------
// MemoryAsset
// ----------------------------------------------------------------------------

AssetHandle AssetManager::RegisterMemoryAsset(Ref<Asset> asset)
{
    if (!IsRegistryReady("register memory asset"))
        return 0;

    if (!asset)
    {
        HAZEL_CORE_ERROR("Fail:register memory asset. Asset is null.");
        return 0;
    }

    if (asset->Handle != 0)
    {
        HAZEL_CORE_ERROR("Fail:register memory asset. Asset already has handle. Handle: {0}",
                         asset->Handle);
        return 0;
    }

    const AssetType type = asset->GetAssetType();
    if (type == AssetType::None)
    {
        HAZEL_CORE_ERROR("Fail:register memory asset. AssetType is None.");
        return 0;
    }

    AssetMetadata metadata;
    metadata.Handle = s_HandleAllocator.Generate(*s_AssetRegistry);
    if (metadata.Handle == 0)
        return 0;

    metadata.Type = type;
    metadata.Domain = AssetDomain::Memory;
    metadata.FilePath.clear();

    if (!s_AssetRegistry->TryRegisterAsset(metadata))
        return 0;

    asset->Handle = metadata.Handle;

    if (!s_RuntimeCache.Store(metadata.Handle, asset))
    {
        asset->Handle = 0;
        s_AssetRegistry->TryRemoveAsset(metadata.Handle);
        return 0;
    }

    return metadata.Handle;
}

// ----------------------------------------------------------------------------
// 获取 Asset
// ----------------------------------------------------------------------------

Ref<Asset> AssetManager::GetAsset(AssetHandle Handle)
{
    Ref<Asset> loadedAsset = s_RuntimeCache.Get(Handle);
    if (loadedAsset)
        return loadedAsset;

    const AssetMetadata* metadata = TryGetMetadataForOperation(Handle, "get asset");

    if (!metadata)
        return nullptr;

    if (metadata->IsMemoryAsset())
    {
        HAZEL_CORE_ERROR("Fail:get asset. MemoryAsset is registered but not loaded. Handle: {0}",
                         Handle);
        return nullptr;
    }

    Ref<Asset> asset = LoadAsset(*metadata);
    if (!asset)
        return nullptr;

    asset->Handle = metadata->Handle;

    if (!s_RuntimeCache.Store(Handle, asset))
        return nullptr;

    return asset;
}

// ----------------------------------------------------------------------------
// Metadata
// ----------------------------------------------------------------------------

const AssetMetadata* AssetManager::GetMetadata(AssetHandle Handle)
{
    return TryGetMetadataForOperation(Handle, "get metadata");
}

// ----------------------------------------------------------------------------
// 路径解析
// ----------------------------------------------------------------------------

std::string AssetManager::ResolveAssetPath(const AssetMetadata& metadata)
{
    return s_SystemFileResolver.ResolveAssetPath(metadata);
}

// ----------------------------------------------------------------------------
// 缓存与移除
// ----------------------------------------------------------------------------

bool AssetManager::TryUnloadAsset(AssetHandle Handle)
{
    const AssetMetadata* metadata = TryGetMetadataForOperation(Handle, "unload asset");

    if (!metadata)
        return false;

    if (metadata->IsMemoryAsset())
    {
        HAZEL_CORE_ERROR("Fail:unload asset. MemoryAsset cannot be unloaded. Handle: {0}", Handle);
        return false;
    }

    if (!metadata->IsFileAsset())
    {
        HAZEL_CORE_ERROR("Fail:unload asset. Metadata is not file asset. Handle: {0}, Domain: {1}, "
                         "FilePath: {2}",
                         metadata->Handle, static_cast<int>(metadata->Domain), metadata->FilePath);
        return false;
    }

    if (!s_RuntimeCache.IsLoaded(Handle))
    {
        HAZEL_CORE_ERROR("Fail:unload asset. Asset is not loaded. Handle: {0}", Handle);
        return false;
    }

    s_RuntimeCache.Remove(Handle);
    return true;
}

bool AssetManager::TryRemoveAsset(AssetHandle Handle)
{
    if (!TryGetMetadataForOperation(Handle, "remove asset"))
        return false;

    if (!s_AssetRegistry->TryRemoveAsset(Handle))
        return false;

    s_RuntimeCache.Remove(Handle);
    return true;
}

// ----------------------------------------------------------------------------
// 状态查询
// ----------------------------------------------------------------------------

bool AssetManager::IsAssetHandleValid(AssetHandle Handle)
{
    if (!IsRegistryReady("check asset handle"))
        return false;

    return s_AssetRegistry->IsContained(Handle);
}

bool AssetManager::IsAssetLoaded(AssetHandle Handle)
{
    return s_RuntimeCache.IsLoaded(Handle);
}

// ----------------------------------------------------------------------------
// 加载实现
// ----------------------------------------------------------------------------

Ref<Asset> AssetManager::LoadAsset(const AssetMetadata& metadata)
{
    if (!metadata.IsValid())
    {
        HAZEL_CORE_ERROR(
            "Fail:load asset. Invalid metadata. Handle: {0}, Type: {1}, Domain: {2}, FilePath: {3}",
            metadata.Handle, static_cast<int>(metadata.Type), static_cast<int>(metadata.Domain),
            metadata.FilePath);
        return nullptr;
    }

    if (!metadata.IsFileAsset())
    {
        HAZEL_CORE_ERROR(
            "Fail:load asset. Metadata is not file asset. Handle: {0}, Domain: {1}, FilePath: {2}",
            metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
        return nullptr;
    }

    AssetSerializer* serializer = s_SerializerRegistry.GetSerializer(metadata.Type);

    if (!serializer)
    {
        HAZEL_CORE_ERROR("Fail:load asset. No serializer. Handle: {0}, Type: {1}, FilePath: {2}",
                         metadata.Handle, static_cast<int>(metadata.Type), metadata.FilePath);
        return nullptr;
    }

    Ref<Asset> asset;
    if (!serializer->TryLoad(metadata, asset))
        return nullptr;

    return asset;
}

// ----------------------------------------------------------------------------
// 校验函数
// ----------------------------------------------------------------------------

bool AssetManager::IsRegistryReady(std::string_view operation)
{
    if (s_AssetRegistry)
        return true;

    HAZEL_CORE_ERROR("Fail:{0}. AssetRegistry is not initialized.", operation);
    return false;
}

const AssetMetadata* AssetManager::TryGetMetadataForOperation(AssetHandle Handle,
                                                              std::string_view operation)
{
    if (!IsRegistryReady(operation))
        return nullptr;

    if (Handle == 0)
    {
        HAZEL_CORE_ERROR("Fail:{0}. Invalid handle. Handle: {1}", operation, Handle);
        return nullptr;
    }

    const AssetMetadata* metadata = s_AssetRegistry->GetMetadata(Handle);
    if (metadata)
        return metadata;

    HAZEL_CORE_ERROR("Fail:{0}. Handle does not exist. Handle: {1}", operation, Handle);
    return nullptr;
}

bool AssetManager::IsValidRestoredFileMetadata(const AssetMetadata& metadata)
{
    if (!metadata.IsValid())
    {
        HAZEL_CORE_ERROR("Fail:restore asset metadata. Invalid metadata. Handle: {0}, Type: {1}, "
                         "Domain: {2}, FilePath: {3}",
                         metadata.Handle, static_cast<int>(metadata.Type),
                         static_cast<int>(metadata.Domain), metadata.FilePath);
        return false;
    }

    if (!metadata.IsFileAsset())
    {
        HAZEL_CORE_ERROR("Fail:restore asset metadata. Metadata is not file asset. Handle: {0}, "
                         "Domain: {1}, FilePath: {2}",
                         metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
        return false;
    }

    const AssetType typeFromPath = GetAssetTypeFromPath(metadata.FilePath);
    if (typeFromPath == AssetType::None)
    {
        HAZEL_CORE_ERROR(
            "Fail:restore asset metadata. Unknown asset type. Handle: {0}, FilePath: {1}",
            metadata.Handle, metadata.FilePath);
        return false;
    }

    if (typeFromPath != metadata.Type)
    {
        HAZEL_CORE_ERROR("Fail:restore asset metadata. Type does not match extension. Handle: {0}, "
                         "MetadataType: {1}, PathType: {2}, FilePath: {3}",
                         metadata.Handle, static_cast<int>(metadata.Type),
                         static_cast<int>(typeFromPath), metadata.FilePath);
        return false;
    }

    return true;
}

bool AssetManager::IsValidImportDomain(AssetDomain domain, std::string_view relativePath)
{
    if (domain == AssetDomain::Engine || domain == AssetDomain::Project)
        return true;

    HAZEL_CORE_ERROR("Fail:import file asset. Invalid import domain. Domain: {0}, Path: {1}",
                     static_cast<int>(domain), std::string(relativePath));
    return false;
}

} // namespace Hazel