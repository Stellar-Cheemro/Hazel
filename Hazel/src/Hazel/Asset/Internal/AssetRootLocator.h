#pragma once

#include <filesystem>
#include <optional>

namespace Hazel
{

class AssetRootLocator
{
public:
    static std::optional<std::filesystem::path> FindEngineAssetRoot(
        const std::filesystem::path& startDirectory);

private:
    static bool IsEngineAssetRoot(const std::filesystem::path& path);
};

} // namespace Hazel