#pragma once

#include "vulkan_helpers.h"


struct ShaderProgram {
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderInfo{
        {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}, {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO}}};

    ShaderProgram();
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    void LoadVertexShader(const std::vector<uint32_t>& code);
    void LoadFragmentShader(const std::vector<uint32_t>& code);
    void Init(VkDevice device);

   private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    void Load(uint32_t index, const std::vector<uint32_t>& code);
};
