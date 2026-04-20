#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <filesystem>
#include <string>

namespace Hazel
{
struct ProjectConfig
{
    std::string Name = "Sandbox";
    std::filesystem::path ProjectDirectory;
    std::filesystem::path AssetDirectory = "assets";
    std::filesystem::path WorkingDirectory = ".";
};

class HAZEL_API Project : public RefCounted
{
public:
    Project() = default;
    explicit Project(const ProjectConfig& config);

    const ProjectConfig& GetConfig() const;

    std::filesystem::path GetAssetDirectory() const;
    std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& relativePath) const;

    static void SetActive(const Ref<Project>& project);
    static Ref<Project> GetActive();

private:
    ProjectConfig m_Config;

private:
    static Ref<Project> s_ActiveProject;
};
} // namespace Hazel