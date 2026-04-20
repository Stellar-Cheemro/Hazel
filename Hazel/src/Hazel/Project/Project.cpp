#include "Hazel/Project/Project.h"

namespace Hazel
{
Ref<Project> Project::s_ActiveProject = nullptr;

Project::Project(const ProjectConfig& config) : m_Config(config)
{
}

const ProjectConfig& Project::GetConfig() const
{
    return m_Config;
}

std::filesystem::path Project::GetAssetDirectory() const
{
    return m_Config.ProjectDirectory / m_Config.AssetDirectory;
}

std::filesystem::path Project::GetAssetAbsolutePath(const std::filesystem::path& relativePath) const
{
    return GetAssetDirectory() / relativePath;
}

void Project::SetActive(const Ref<Project>& project)
{
    s_ActiveProject = project;
}

Ref<Project> Project::GetActive()
{
    return s_ActiveProject;
}
} // namespace Hazel