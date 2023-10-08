#pragma once

#include "pch.h"
#include "common.h"

#include "depth_buffer.h"
#include "render_pass.h"
#include "pipeline.h"
#include "geometry.h"
#include "vertexbuffer.h"
#include "render_target.h"


class MemoryAllocator;
class PipelineLayout;
class ShaderProgram;

struct SwapchainImageContext {
    SwapchainImageContext(XrStructureType _swapchainImageType);
    SwapchainImageContext() = default;

    // A packed array of XrSwapchainImageVulkan2KHR's for xrEnumerateSwapchainImages
    std::vector<XrSwapchainImageVulkan2KHR> swapchainImages;
    std::vector<RenderTarget> renderTarget;
    VkExtent2D size{};
    DepthBuffer depthBuffer{};
    RenderPass rp{};
    Pipeline pipe{};
    XrStructureType swapchainImageType;

    std::vector<XrSwapchainImageBaseHeader*> Create(
        VkDevice device, MemoryAllocator* memAllocator, uint32_t capacity,
        const XrSwapchainCreateInfo& swapchainCreateInfo, const PipelineLayout& layout,
        const ShaderProgram& sp, const VertexBuffer<Geometry::Vertex>& vb
    );

    uint32_t ImageIndex(const XrSwapchainImageBaseHeader* swapchainImageHeader);
    void BindRenderTarget(uint32_t index, VkRenderPassBeginInfo* renderPassBeginInfo);

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
