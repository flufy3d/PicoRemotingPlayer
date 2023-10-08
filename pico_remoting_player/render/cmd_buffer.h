// cmd_buffer.h
#pragma once

#include "pch.h"
#include "common.h"

class CmdBuffer {

#define LIST_CMDBUFFER_STATES(_) \
    _(Undefined)                 \
    _(Initialized)               \
    _(Recording)                 \
    _(Executable)                \
    _(Executing)
    enum class CmdBufferState {
#define MK_ENUM(name) name,
        LIST_CMDBUFFER_STATES(MK_ENUM)
#undef MK_ENUM
    };
    
public:
    CmdBuffer();
    CmdBuffer(const CmdBuffer&) = delete;
    CmdBuffer& operator=(const CmdBuffer&) = delete;
    CmdBuffer(CmdBuffer&&) = delete;
    CmdBuffer& operator=(CmdBuffer&&) = delete;
    ~CmdBuffer();

    bool Init(VkDevice device, uint32_t queueFamilyIndex);
    bool Begin();
    bool End();
    bool Exec(VkQueue queue);
    bool Wait();
    bool Reset();

public:
    VkCommandBuffer buf{VK_NULL_HANDLE};

private:
    void SetState(CmdBufferState newState);

    CmdBufferState state{CmdBufferState::Undefined};
    VkCommandPool pool{VK_NULL_HANDLE};
    
    VkFence execFence{VK_NULL_HANDLE};
    VkDevice m_vkDevice{VK_NULL_HANDLE};

    std::string StateString(CmdBufferState s);
#define CHECK_CBSTATE(s)                                                                                           \
    do                                                                                                             \
        if (state != (s)) {                                                                                        \
            Log::Write(Log::Level::Error,                                                                          \
                       std::string("Expecting state " #s " from ") + __FUNCTION__ + ", in " + StateString(state)); \
            return false;                                                                                          \
        }                                                                                                          \
    while (0)

};
