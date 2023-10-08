#pragma once

#include "pch.h"
#include "common.h"


class MemoryAllocator;
class CmdBuffer;

struct DepthBuffer {
    VkDeviceMemory depthMemory{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};

    DepthBuffer() = default;
    ~DepthBuffer();
    DepthBuffer(DepthBuffer&& other) noexcept;
    DepthBuffer& operator=(DepthBuffer&& other) noexcept;
    void Create(VkDevice device, MemoryAllocator* memAllocator, VkFormat depthFormat,
                const XrSwapchainCreateInfo& swapchainCreateInfo);
    void TransitionLayout(CmdBuffer* cmdBuffer, VkImageLayout newLayout);

    DepthBuffer(const DepthBuffer&) = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    VkImageLayout m_vkLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};
