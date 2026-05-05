#pragma once

#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Containers.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>

#include <string>
#include <string_view>

namespace Hazel
{

// AssetRegistry 只负责 AssetMetadata 索引。
// 不负责加载资源。
// 不负责解析真实文件系统路径。
// 不负责删除磁盘文件。
class HAZEL_API AssetRegistry : public RefCounted
{
public:
    bool IsContained(AssetHandle Handle) const;

    bool IsContained(std::string_view normalizedRelativePath,
                     AssetDomain domain = AssetDomain::Engine) const;

    const AssetMetadata* GetMetadata(AssetHandle Handle) const;
    AssetMetadata* GetMetadata(AssetHandle Handle);

    AssetHandle GetHandleFromPath(std::string_view normalizedRelativePath,
                                  AssetDomain domain = AssetDomain::Engine) const;

    bool TryRegisterAsset(const AssetMetadata& metadata);
    bool TryRemoveAsset(AssetHandle Handle);

    void Clear();

private:
    bool TryRegisterMemoryAsset(const AssetMetadata& metadata);
    bool TryRegisterFileAsset(const AssetMetadata& metadata);

private:
    HashMap<AssetHandle, AssetMetadata> m_HandleToMetadata;
    HashMap<std::string, AssetHandle> m_PathToHandle;
};

} // namespace Hazel