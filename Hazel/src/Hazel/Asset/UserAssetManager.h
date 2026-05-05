#pragma once

#include <Hazel/Asset/Asset.h>
#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>

#include <string_view>

namespace Hazel
{

// UserAssetManager 是面向用户项目的 Asset 门面。
//
// 职责：
// 1. 只暴露用户项目允许访问的 Asset API。
// 2. 不持有 AssetRegistry、RuntimeCache、SerializerRegistry 等状态。
// 3. 不允许用户项目直接操作 EngineAsset。
// 4. 不允许用户项目获取真实文件系统解析路径。
// 5. 不允许用户项目修改 Metadata。
//
// 用户侧允许：
// 1. 导入 ProjectAsset。
// 2. 注册 MemoryAsset。
// 3. 通过 Handle 获取 Asset。
// 4. 查询只读 Metadata。
// 5. 查询 Handle / Loaded 状态。
// 6. 卸载 ProjectAsset 的运行时缓存。
// 7. 移除 ProjectAsset / MemoryAsset 的注册记录。
class HAZEL_API UserAssetManager
{
public:
    // ------------------------------------------------------------------------
    // ProjectAsset
    // ------------------------------------------------------------------------

    static AssetHandle ImportProjectAsset(std::string_view relativePath);

    // ------------------------------------------------------------------------
    // MemoryAsset
    // ------------------------------------------------------------------------

    template <AssetDerived T> static AssetHandle RegisterMemoryAsset(const Ref<T>& asset)
    {
        return RegisterMemoryAssetInternal(asset.template As<Asset>());
    }

    // ------------------------------------------------------------------------
    // 获取 Asset
    // ------------------------------------------------------------------------

    template <AssetDerived T> static Ref<T> GetAsset(AssetHandle Handle)
    {
        Ref<Asset> asset = GetAssetInternal(Handle);
        if (!asset)
            return nullptr;

        return asset.As<T>();
    }

    // ------------------------------------------------------------------------
    // Metadata 查询
    // ------------------------------------------------------------------------

    static const AssetMetadata* GetMetadata(AssetHandle Handle);

    // ------------------------------------------------------------------------
    // 状态查询
    // ------------------------------------------------------------------------

    static bool IsAssetHandleValid(AssetHandle Handle);
    static bool IsAssetLoaded(AssetHandle Handle);

    // ------------------------------------------------------------------------
    // 生命周期操作
    // ------------------------------------------------------------------------

    // 只卸载 ProjectAsset 的运行时缓存，不移除 Metadata。
    // 后续 GetAsset(handle) 可以重新加载。
    static bool TryUnloadProjectAsset(AssetHandle Handle);

    // 移除 ProjectAsset 的缓存和 Metadata。
    // 不删除磁盘文件。
    static bool TryRemoveProjectAsset(AssetHandle Handle);

    // 移除 MemoryAsset 的缓存和 Metadata。
    // MemoryAsset 不支持单独卸载，只能移除。
    static bool TryRemoveMemoryAsset(AssetHandle Handle);

private:
    static AssetHandle RegisterMemoryAssetInternal(Ref<Asset> asset);
    static Ref<Asset> GetAssetInternal(AssetHandle Handle);
};

} // namespace Hazel