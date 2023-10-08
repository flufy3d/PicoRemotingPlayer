#pragma once

#include "vulkan_helpers.h"
#include "memory_allocator.h"
#include "depth_buffer.h"
#include "render_pass.h"
#include "pipeline.h"
#include "pipeline_layout.h"
#include "shader_program.h"
#include "vertexbuffer.h"
#include "render_target.h"
#include "geometry.h"


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
