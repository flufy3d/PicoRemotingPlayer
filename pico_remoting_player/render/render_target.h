#pragma once

#include "vulkan_helpers.h"
#include "render/render_pass.h"

// VkImage + framebuffer wrapper
struct RenderTarget {
    VkImage colorImage{VK_NULL_HANDLE};
    VkImage depthImage{VK_NULL_HANDLE};
    VkImageView colorView{VK_NULL_HANDLE};
    VkImageView depthView{VK_NULL_HANDLE};
    VkFramebuffer fb{VK_NULL_HANDLE};

    RenderTarget() = default;
    ~RenderTarget();
    RenderTarget(RenderTarget&& other) noexcept;
    RenderTarget& operator=(RenderTarget&& other) noexcept;
    void Create(VkDevice device, VkImage aColorImage, VkImage aDepthImage, VkExtent2D size, RenderPass& renderPass);

    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator=(const RenderTarget&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
