#pragma once

#include "vulkan_helpers.h"

struct RenderPass {
    VkFormat colorFmt{};
    VkFormat depthFmt{};
    VkRenderPass pass{VK_NULL_HANDLE};

    RenderPass();
    bool Create(VkDevice device, VkFormat aColorFmt, VkFormat aDepthFmt);
    ~RenderPass();

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass(RenderPass&&) = delete;
    RenderPass& operator=(RenderPass&&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
