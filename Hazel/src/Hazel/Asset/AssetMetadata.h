// AssetMetadata 只保存资源的稳定身份信息。
// 不保存运行时加载状态，也不保存绝对文件系统路径。
#pragma once
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Core.h>

#include <string>
namespace Hazel
{

struct HAZEL_API AssetMetadata
{
    AssetHandle Handle = 0;
    AssetType Type = AssetType::None;
    AssetDomain Domain = AssetDomain::None;

    // Engine / Project：保存规范化后的 Asset 相对路径，统一使用 "/"。
    // Memory：必须为空。
    std::string FilePath;
    bool IsValid() const
    {
        return Handle != 0 && Type != AssetType::None && Domain != AssetDomain::None;
    }
    operator bool() const
    {
        return IsValid();
    }
    bool HasFilePath() const
    {
        return !FilePath.empty();
    }

    bool IsEngineAsset() const
    {
        return Domain == AssetDomain::Engine;
    }

    bool IsProjectAsset() const
    {
        return Domain == AssetDomain::Project;
    }

    bool IsMemoryAsset() const
    {
        return Domain == AssetDomain::Memory;
    }
    bool IsFileAsset() const
    {
        return HasFilePath() && Domain != AssetDomain::Memory && Domain != AssetDomain::None;
    }
};
} // namespace Hazel