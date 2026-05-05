#pragma once

#include <Hazel/Asset/AssetRegistry.h>
#include <Hazel/Asset/AssetTypes.h>
#include <Hazel/Core/Core.h>

namespace Hazel
{

class HAZEL_API AssetHandleAllocator
{
public:
    void Reset();

    AssetHandle Generate(const AssetRegistry& registry);
    void AdvancePast(AssetHandle Handle);

private:
    AssetHandle m_NextHandle = 1;
};

} // namespace Hazel