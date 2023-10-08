#pragma once

#include "pch.h"
#include "common.h"

#include "vertexbuffer_base.h"

class PipelineLayout;
class RenderPass;
class ShaderProgram;


// Pipeline wrapper for rendering pipeline state
struct Pipeline {
    VkPipeline pipe{VK_NULL_HANDLE};
    VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    std::vector<VkDynamicState> dynamicStateEnables;

    Pipeline() = default;
    void Dynamic(VkDynamicState state);
    void Create(VkDevice device, VkExtent2D size, const PipelineLayout& layout, const RenderPass& rp, const ShaderProgram& sp,
                const VertexBufferBase& vb);
    void Release();

    private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
