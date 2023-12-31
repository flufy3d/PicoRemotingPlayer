// cmd_buffer.cpp
#include "cmd_buffer.h"

#include "vulkan_helpers.h"

CmdBuffer::CmdBuffer() = default;

CmdBuffer::~CmdBuffer() {
    SetState(CmdBufferState::Undefined);
    if (m_vkDevice != nullptr) {
        if (buf != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(m_vkDevice, pool, 1, &buf);
        }
        if (pool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_vkDevice, pool, nullptr);
        }
        if (execFence != VK_NULL_HANDLE) {
            vkDestroyFence(m_vkDevice, execFence, nullptr);
        }
    }
    buf = VK_NULL_HANDLE;
    pool = VK_NULL_HANDLE;
    execFence = VK_NULL_HANDLE;
    m_vkDevice = nullptr;
}

std::string CmdBuffer::StateString(CmdBufferState s) {
    switch (s) {
#define MK_CASE(name)          \
    case CmdBufferState::name: \
        return #name;
        LIST_CMDBUFFER_STATES(MK_CASE)
#undef MK_CASE
    }
    return "(Unknown)";
}

bool CmdBuffer::Init(VkDevice device, uint32_t queueFamilyIndex) {
    CHECK_CBSTATE(CmdBufferState::Undefined);

    m_vkDevice = device;

    // Create a command pool to allocate our command buffer from
    VkCommandPoolCreateInfo cmdPoolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
    CHECK_VKCMD(vkCreateCommandPool(m_vkDevice, &cmdPoolInfo, nullptr, &pool));

    // Create the command buffer from the command pool
    VkCommandBufferAllocateInfo cmd{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    cmd.commandPool = pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;
    CHECK_VKCMD(vkAllocateCommandBuffers(m_vkDevice, &cmd, &buf));

    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    CHECK_VKCMD(vkCreateFence(m_vkDevice, &fenceInfo, nullptr, &execFence));

    SetState(CmdBufferState::Initialized);
    return true;
}

bool CmdBuffer::Begin() {
    CHECK_CBSTATE(CmdBufferState::Initialized);
    VkCommandBufferBeginInfo cmdBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    CHECK_VKCMD(vkBeginCommandBuffer(buf, &cmdBeginInfo));
    SetState(CmdBufferState::Recording);
    return true;
}

bool CmdBuffer::End() {
    CHECK_CBSTATE(CmdBufferState::Recording);
    CHECK_VKCMD(vkEndCommandBuffer(buf));
    SetState(CmdBufferState::Executable);
    return true;
}

bool CmdBuffer::Exec(VkQueue queue) {
    CHECK_CBSTATE(CmdBufferState::Executable);

    VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buf;
    CHECK_VKCMD(vkQueueSubmit(queue, 1, &submitInfo, execFence));

    SetState(CmdBufferState::Executing);
    return true;
}

bool CmdBuffer::Wait() {
    // Waiting on a not-in-flight command buffer is a no-op
    if (state == CmdBufferState::Initialized) {
        return true;
    }

    CHECK_CBSTATE(CmdBufferState::Executing);

    const uint32_t timeoutNs = 1 * 1000 * 1000 * 1000;
    for (int i = 0; i < 5; ++i) {
        auto res = vkWaitForFences(m_vkDevice, 1, &execFence, VK_TRUE, timeoutNs);
        if (res == VK_SUCCESS) {
            // Buffer can be executed multiple times...
            SetState(CmdBufferState::Executable);
            return true;
        }
        Log::Write(Log::Level::Info, "Waiting for CmdBuffer fence timed out, retrying...");
    }

    return false;
}

bool CmdBuffer::Reset() {
    if (state != CmdBufferState::Initialized) {
        CHECK_CBSTATE(CmdBufferState::Executable);

        CHECK_VKCMD(vkResetFences(m_vkDevice, 1, &execFence));
        CHECK_VKCMD(vkResetCommandBuffer(buf, 0));

        SetState(CmdBufferState::Initialized);
    }

    return true;
}

void CmdBuffer::SetState(CmdBufferState newState) {
    state = newState;
}
