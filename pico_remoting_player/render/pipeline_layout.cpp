#include "pipeline_layout.h"

PipelineLayout::~PipelineLayout() {
    if (m_vkDevice != nullptr) {
        if (layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(m_vkDevice, layout, nullptr);
        }
    }
    layout = VK_NULL_HANDLE;
    m_vkDevice = nullptr;
}

void PipelineLayout::Create(VkDevice device) {
    m_vkDevice = device;

    // MVP matrix is a push_constant
    VkPushConstantRange pcr = {};
    pcr.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pcr.offset = 0;
    pcr.size = 4 * 4 * sizeof(float);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pcr;
    CHECK_VKCMD(vkCreatePipelineLayout(m_vkDevice, &pipelineLayoutCreateInfo, nullptr, &layout));
}
