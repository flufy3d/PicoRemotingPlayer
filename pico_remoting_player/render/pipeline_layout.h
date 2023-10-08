#pragma once

#include "vulkan_helpers.h"

// Simple vertex MVP xform & color fragment shader layout
struct PipelineLayout {
    VkPipelineLayout layout{VK_NULL_HANDLE};

    PipelineLayout() = default;
    ~PipelineLayout();
    void Create(VkDevice device);

    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) = delete;

private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
};
