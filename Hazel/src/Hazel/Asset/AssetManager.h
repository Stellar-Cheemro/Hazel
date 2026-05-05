#pragma once

#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>

#include <filesystem>
#include <string>
#include <string_view>

namespace Hazel
{
class AssetRegistry;
class AssetRuntimeCache;
class AssetSerializerRegistry;
class AssetHandleAllocator;
class AssetSystemFileResolver;
class AssetRootLocator;
// AssetManager 是引擎内部核心 Asset 管理器。
//
// 职责：
// 1. 作为 Engine / Project / Memory Asset 的统一内部入口。
// 2. 编排 Asset 导入、恢复、加载、卸载、移除等流程。
// 3. 协调 Registry、RuntimeCache、SerializerRegistry、HandleAllocator、FileSystemResolver。
//
// 边界：
// 1. 用户项目应使用 UserAssetManager。
// 2. 用户公开总头不应直接包含 AssetManager.h。
// 3. AssetManager 不删除磁盘文件。
// 4. AssetManager 不生成 Registry key。
// 5. AssetManager 不直接实现具体资源类型加载。
class HAZEL_API AssetManager
{
public:
    // ------------------------------------------------------------------------
    // 生命周期
    // ------------------------------------------------------------------------
    static void Init();
    static void Init(const std::filesystem::path& engineAssetRoot);
    static void Shutdown();

    // ------------------------------------------------------------------------
    // 文件资源
    // ------------------------------------------------------------------------

    static AssetHandle ImportEngineAsset(std::string_view relativePath);
    static AssetHandle ImportProjectAsset(std::string_view relativePath);

    // 从已保存的 Registry / 项目文件恢复 Metadata。
    //
    // 不生成新 Handle。
    // 不加载资源。
    // 不强制校验真实文件是否存在。
    static bool TryRestoreAssetMetadata(const AssetMetadata& metadata);

    // ------------------------------------------------------------------------
    // MemoryAsset
    // ------------------------------------------------------------------------

    // 注册运行时内存资源。
    // MemoryAsset 不走路径、不走 Serializer。
    static AssetHandle RegisterMemoryAsset(Ref<Asset> asset);

    template <AssetDerived T> static AssetHandle RegisterMemoryAsset(const Ref<T>& asset)
    {
        return RegisterMemoryAsset(asset.template As<Asset>());
    }

    // ------------------------------------------------------------------------
    // 获取 Asset
    // ------------------------------------------------------------------------

    static Ref<Asset> GetAsset(AssetHandle Handle);

    template <AssetDerived T> static Ref<T> GetAsset(AssetHandle Handle)
    {
        Ref<Asset> asset = GetAsset(Handle);
        if (!asset)
            return nullptr;

        return asset.As<T>();
    }

    // ------------------------------------------------------------------------
    // Metadata
    // ------------------------------------------------------------------------

    static const AssetMetadata* GetMetadata(AssetHandle Handle);

    // ------------------------------------------------------------------------
    // 路径解析
    // ------------------------------------------------------------------------

    // 根据 AssetMetadata 的 Domain 和 FilePath 解析真实文件系统路径。
    // 返回值统一使用 "/"。
    static std::string ResolveAssetPath(const AssetMetadata& metadata);

    // ------------------------------------------------------------------------
    // 缓存与移除
    // ------------------------------------------------------------------------

    // 只卸载已加载的文件资源运行时缓存。
    // 不移除 Metadata。
    // MemoryAsset 不允许单独卸载，应使用 TryRemoveAsset。
    static bool TryUnloadAsset(AssetHandle Handle);

    // 从 Asset 系统中移除资源。
    // 会移除 LoadedAssets 缓存和 Registry Metadata。
    // 不删除磁盘文件。
    static bool TryRemoveAsset(AssetHandle Handle);

    // ------------------------------------------------------------------------
    // 状态查询
    // ------------------------------------------------------------------------

    static bool IsAssetHandleValid(AssetHandle Handle);
    static bool IsAssetLoaded(AssetHandle Handle);

private:
    // ------------------------------------------------------------------------
    // 初始化实现
    // ------------------------------------------------------------------------
    static void ResetState();             // 重置缓存、Registry、HandleAllocator、Resolver
    static void SetEngineAssetRootAuto(); // 自动查找 EngineAssetRoot
    static void SetEngineAssetRootExplicit(const std::filesystem::path& rootPath);
    static void RegisterSerializers();
    static void ValidateSerializers();

private:
    // ------------------------------------------------------------------------
    // 文件资源流程实现
    // ------------------------------------------------------------------------
    static AssetHandle ImportFileAsset(std::string_view relativePath, AssetDomain domain);

    static Ref<Asset> LoadAsset(const AssetMetadata& metadata);

private:
    // ------------------------------------------------------------------------
    // 校验函数
    //
    // 只做校验和日志，不改变状态。
    // ------------------------------------------------------------------------

    static bool IsRegistryReady(std::string_view operation);

    static const AssetMetadata* TryGetMetadataForOperation(AssetHandle Handle,
                                                           std::string_view operation);

    static bool IsValidRestoredFileMetadata(const AssetMetadata& metadata);

    static bool IsValidImportDomain(AssetDomain domain, std::string_view relativePath);

private:
    // ------------------------------------------------------------------------
    // Internal Components
    // ------------------------------------------------------------------------

    static Ref<AssetRegistry> s_AssetRegistry;

    static AssetRuntimeCache s_RuntimeCache;
    static AssetSerializerRegistry s_SerializerRegistry;
    static AssetHandleAllocator s_HandleAllocator;
    static AssetSystemFileResolver s_SystemFileResolver;
};

} // namespace Hazel