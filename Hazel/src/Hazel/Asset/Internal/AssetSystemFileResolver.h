#pragma once

#include <Hazel/Asset/AssetMetadata.h>
#include <Hazel/Core/Core.h>

#include <filesystem>
#include <string>
#include <string_view>

namespace Hazel
{

class HAZEL_API AssetSystemFileResolver
{
public:
    void Clear();

    bool SetEngineAssetRoot(const std::filesystem::path& rootPath);

    bool HasEngineAssetRoot() const;
    const std::filesystem::path& GetEngineAssetRoot() const;

    std::string ResolveAssetPath(const AssetMetadata& metadata) const;

    bool ValidateExistingFile(std::string_view resolvedPath, std::string_view pathName) const;

private:
    static std::string ToGenericNormalizedString(const std::filesystem::path& path);

    static bool TryResolveExistingDirectoryPath(const std::filesystem::path& inputPath,
                                                std::filesystem::path& outResolvedPath,
                                                std::string_view pathName);

private:
    std::filesystem::path m_EngineAssetRoot;
};

} // namespace Hazel