#include "AssetSystemFileResolver.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Project/Project.h>

#include <string>
#include <system_error>
#include <utility>

namespace Hazel
{

void AssetSystemFileResolver::Clear()
{
    m_EngineAssetRoot.clear();
}

bool AssetSystemFileResolver::SetEngineAssetRoot(const std::filesystem::path& rootPath)
{
    std::filesystem::path resolvedRootPath;

    if (!TryResolveExistingDirectoryPath(rootPath, resolvedRootPath, "engine asset root"))
    {
        m_EngineAssetRoot.clear();
        return false;
    }

    m_EngineAssetRoot = std::move(resolvedRootPath);
    return true;
}

const std::filesystem::path& AssetSystemFileResolver::GetEngineAssetRoot() const
{
    return m_EngineAssetRoot;
}

bool AssetSystemFileResolver::HasEngineAssetRoot() const
{
    return !m_EngineAssetRoot.empty();
}

std::string AssetSystemFileResolver::ResolveAssetPath(const AssetMetadata& metadata) const
{
    if (!metadata.IsValid())
    {
        HAZEL_CORE_ERROR("Fail:resolve asset path. Invalid metadata. Handle: {0}, Type: {1}, "
                         "Domain: {2}, FilePath: {3}",
                         metadata.Handle, static_cast<int>(metadata.Type),
                         static_cast<int>(metadata.Domain), metadata.FilePath);
        return {};
    }

    if (!metadata.IsFileAsset())
    {
        HAZEL_CORE_ERROR("Fail:resolve asset path. Metadata is not file asset. Handle: {0}, "
                         "Domain: {1}, FilePath: {2}",
                         metadata.Handle, static_cast<int>(metadata.Domain), metadata.FilePath);
        return {};
    }

    const std::filesystem::path relativePath(metadata.FilePath);

    if (metadata.IsProjectAsset())
    {
        Ref<Project> currentProject = Project::GetActive();
        if (!currentProject)
        {
            HAZEL_CORE_ERROR(
                "Fail:resolve project asset path. No active project. Handle: {0}, FilePath: {1}",
                metadata.Handle, metadata.FilePath);
            return {};
        }

        return ToGenericNormalizedString(currentProject->GetAssetAbsolutePath(relativePath));
    }

    if (metadata.IsEngineAsset())
    {
        if (m_EngineAssetRoot.empty())
        {
            HAZEL_CORE_ERROR("Fail:resolve engine asset path. Engine asset root is empty. Handle: "
                             "{0}, FilePath: {1}",
                             metadata.Handle, metadata.FilePath);
            return {};
        }

        return ToGenericNormalizedString(m_EngineAssetRoot / relativePath);
    }

    HAZEL_CORE_ERROR("Fail:resolve asset path. Unsupported asset domain. Handle: {0}, Domain: {1}",
                     metadata.Handle, static_cast<int>(metadata.Domain));
    return {};
}

bool AssetSystemFileResolver::ValidateExistingFile(std::string_view resolvedPath,
                                                   std::string_view pathName) const
{
    if (resolvedPath.empty())
    {
        HAZEL_CORE_ERROR("Fail:validate file path. Path is empty. Name: {0}", pathName);
        return false;
    }

    const std::string resolvedPathString(resolvedPath);
    const std::filesystem::path filePath{resolvedPathString};

    std::error_code errorCode;
    const bool exists = std::filesystem::exists(filePath, errorCode);
    if (errorCode)
    {
        HAZEL_CORE_ERROR(
            "Fail:validate file path. Failed to check existence. Name: {0}, Path: {1}, Error: {2}",
            pathName, resolvedPathString, errorCode.message());
        return false;
    }

    if (!exists)
    {
        HAZEL_CORE_ERROR("Fail:validate file path. File does not exist. Name: {0}, Path: {1}",
                         pathName, resolvedPathString);
        return false;
    }

    errorCode.clear();
    const bool isRegularFile = std::filesystem::is_regular_file(filePath, errorCode);

    if (errorCode)
    {
        HAZEL_CORE_ERROR("Fail:validate file path. Failed to check regular file type. Name: {0}, "
                         "Path: {1}, Error: {2}",
                         pathName, resolvedPathString, errorCode.message());
        return false;
    }

    if (!isRegularFile)
    {
        HAZEL_CORE_ERROR(
            "Fail:validate file path. Path is not a regular file. Name: {0}, Path: {1}", pathName,
            resolvedPathString);
        return false;
    }

    return true;
}

std::string AssetSystemFileResolver::ToGenericNormalizedString(const std::filesystem::path& path)
{
    return path.lexically_normal().generic_string();
}

bool AssetSystemFileResolver::TryResolveExistingDirectoryPath(
    const std::filesystem::path& inputPath, std::filesystem::path& outResolvedPath,
    std::string_view pathName)
{
    if (inputPath.empty())
    {
        HAZEL_CORE_ERROR("Fail:resolve directory path. Path is empty. Name: {0}", pathName);
        return false;
    }

    std::error_code errorCode;
    std::filesystem::path absolutePath = std::filesystem::absolute(inputPath, errorCode);

    if (errorCode)
    {
        HAZEL_CORE_ERROR("Fail:resolve directory path. Failed to get absolute path. Name: {0}, "
                         "Path: {1}, Error: {2}",
                         pathName, inputPath.string(), errorCode.message());
        return false;
    }

    absolutePath = absolutePath.lexically_normal();

    errorCode.clear();
    const bool exists = std::filesystem::exists(absolutePath, errorCode);
    if (errorCode)
    {
        HAZEL_CORE_ERROR("Fail:resolve directory path. Failed to check existence. Name: {0}, Path: "
                         "{1}, Error: {2}",
                         pathName, absolutePath.string(), errorCode.message());
        return false;
    }

    if (!exists)
    {
        HAZEL_CORE_ERROR("Fail:resolve directory path. Path does not exist. Name: {0}, Path: {1}",
                         pathName, absolutePath.string());
        return false;
    }

    errorCode.clear();
    const bool isDirectory = std::filesystem::is_directory(absolutePath, errorCode);

    if (errorCode)
    {
        HAZEL_CORE_ERROR("Fail:resolve directory path. Failed to check directory type. Name: {0}, "
                         "Path: {1}, Error: {2}",
                         pathName, absolutePath.string(), errorCode.message());
        return false;
    }

    if (!isDirectory)
    {
        HAZEL_CORE_ERROR(
            "Fail:resolve directory path. Path is not a directory. Name: {0}, Path: {1}", pathName,
            absolutePath.string());
        return false;
    }

    outResolvedPath = std::move(absolutePath);
    return true;
}

} // namespace Hazel