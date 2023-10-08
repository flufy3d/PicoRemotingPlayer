#pragma once

#include "pch.h"
#include "common.h"

class MemoryAllocator;

struct VertexBufferBase {
    VkBuffer idxBuf{VK_NULL_HANDLE};
    VkDeviceMemory idxMem{VK_NULL_HANDLE};
    VkBuffer vtxBuf{VK_NULL_HANDLE};
    VkDeviceMemory vtxMem{VK_NULL_HANDLE};
    VkVertexInputBindingDescription bindDesc{};
    std::vector<VkVertexInputAttributeDescription> attrDesc{};
    struct {
        uint32_t idx;
        uint32_t vtx;
    } count = {0, 0};

    VertexBufferBase();
    ~VertexBufferBase();

    VertexBufferBase(const VertexBufferBase&) = delete;
    VertexBufferBase& operator=(const VertexBufferBase&) = delete;
    VertexBufferBase(VertexBufferBase&&) = delete;
    VertexBufferBase& operator=(VertexBufferBase&&) = delete;

    void Init(VkDevice device, const MemoryAllocator* memAllocator, const std::vector<VkVertexInputAttributeDescription>& attr);

protected:
    VkDevice m_vkDevice{VK_NULL_HANDLE};
    void AllocateBufferMemory(VkBuffer buf, VkDeviceMemory* mem) const;

private:
    const MemoryAllocator* m_memAllocator{nullptr};
};
