#include "AssetHandleAllocator.h"

#include <Hazel/Core/Log.h>

#include <limits>

namespace Hazel
{

void AssetHandleAllocator::Reset()
{
    m_NextHandle = 1;
}

AssetHandle AssetHandleAllocator::Generate(const AssetRegistry& registry)
{
    while (m_NextHandle != 0 && registry.IsContained(m_NextHandle))
        ++m_NextHandle;

    if (m_NextHandle == 0)
    {
        HAZEL_CORE_ERROR("Fail:generate asset handle. AssetHandle overflow.");
        return 0;
    }

    return m_NextHandle++;
}

void AssetHandleAllocator::AdvancePast(AssetHandle Handle)
{
    if (Handle == 0)
        return;

    if (Handle < m_NextHandle)
        return;

    if (Handle == std::numeric_limits<AssetHandle>::max())
    {
        HAZEL_CORE_ERROR("Fail:advance asset handle. Restored handle is max value. Handle: {0}",
                         Handle);
        return;
    }

    m_NextHandle = Handle + 1;
}

} // namespace Hazel