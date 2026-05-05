#include "AssetRootLocator.h"

#include <array>
#include <system_error>

namespace Hazel
{

namespace
{

constexpr const char* EngineAssetMarker = ".hazel_engine_assets";

std::filesystem::path TryWeaklyCanonical(const std::filesystem::path& path)
{
    std::error_code error;
    std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path, error);

    if (error)
        return path.lexically_normal();

    return canonicalPath;
}

bool IsSamePath(const std::filesystem::path& left, const std::filesystem::path& right)
{
    return TryWeaklyCanonical(left) == TryWeaklyCanonical(right);
}

} // namespace

bool AssetRootLocator::IsEngineAssetRoot(const std::filesystem::path& path)
{
    std::error_code error;

    if (!std::filesystem::is_directory(path, error))
        return false;

    return std::filesystem::exists(path / EngineAssetMarker, error);
}

std::optional<std::filesystem::path> AssetRootLocator::FindEngineAssetRoot(
    const std::filesystem::path& startDirectory)
{
    // 适配当前仓库结构：
    //
    // RepoRoot/
    //     Hazel/
    //         src/
    //             Hazel/
    //                 Assets/
    //
    // 同时预留后续可能迁移到 Hazel/Assets 或 Hazel/assets 的结构。
    static constexpr std::array<const char*, 8> CandidateRelativePaths = {".",
                                                                          "Assets",
                                                                          "assets",
                                                                          "Hazel/Assets",
                                                                          "Hazel/assets",
                                                                          "Hazel/src/Hazel/Assets",
                                                                          "Hazel/src/Hazel/assets",
                                                                          "src/Hazel/Assets"};

    std::filesystem::path current = TryWeaklyCanonical(startDirectory);

    while (!current.empty())
    {
        for (const char* relativePath : CandidateRelativePaths)
        {
            const std::filesystem::path candidate = TryWeaklyCanonical(current / relativePath);

            if (IsEngineAssetRoot(candidate))
                return candidate;
        }

        const std::filesystem::path parent = current.parent_path();

        if (parent.empty() || IsSamePath(parent, current))
            break;

        current = parent;
    }

    return std::nullopt;
}

} // namespace Hazel