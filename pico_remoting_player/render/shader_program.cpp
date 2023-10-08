#include "shader_program.h"

#include "vulkan_helpers.h"

ShaderProgram::ShaderProgram() = default;

ShaderProgram::~ShaderProgram() {
    if (m_vkDevice != nullptr) {
        for (auto& si : shaderInfo) {
            if (si.module != VK_NULL_HANDLE) {
                vkDestroyShaderModule(m_vkDevice, shaderInfo[0].module, nullptr);
            }
            si.module = VK_NULL_HANDLE;
        }
    }
    shaderInfo = {};
    m_vkDevice = nullptr;
}

void ShaderProgram::LoadVertexShader(const std::vector<uint32_t>& code) {
    Load(0, code);
}

void ShaderProgram::LoadFragmentShader(const std::vector<uint32_t>& code) {
    Load(1, code);
}

void ShaderProgram::Init(VkDevice device) {
    m_vkDevice = device;
}

void ShaderProgram::Load(uint32_t index, const std::vector<uint32_t>& code) {
    VkShaderModuleCreateInfo modInfo{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    auto& si = shaderInfo[index];
    si.pName = "main";
    std::string name;
    
    switch (index) {
        case 0:
            si.stage = VK_SHADER_STAGE_VERTEX_BIT;
            name = "vertex";
            break;
        case 1:
            si.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            name = "fragment";
            break;
        default:
            THROW(Fmt("Unknown code index %d", index));
    }

    modInfo.codeSize = code.size() * sizeof(code[0]);
    modInfo.pCode = &code[0];
    CHECK_MSG((modInfo.codeSize > 0) && modInfo.pCode, Fmt("Invalid %s shader ", name.c_str()));
    CHECK_VKCMD(vkCreateShaderModule(m_vkDevice, &modInfo, nullptr, &si.module));
    Log::Write(Log::Level::Info, Fmt("Loaded %s shader", name.c_str()));
}
