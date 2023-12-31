#include "vertexbuffer_base.h"
#include "memory_allocator.h"

VertexBufferBase::VertexBufferBase() = default;

VertexBufferBase::~VertexBufferBase() {
    if (m_vkDevice != nullptr) {
        if (idxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_vkDevice, idxBuf, nullptr);
        }
        if (idxMem != VK_NULL_HANDLE) {
            vkFreeMemory(m_vkDevice, idxMem, nullptr);
        }
        if (vtxBuf != VK_NULL_HANDLE) {
            vkDestroyBuffer(m_vkDevice, vtxBuf, nullptr);
        }
        if (vtxMem != VK_NULL_HANDLE) {
            vkFreeMemory(m_vkDevice, vtxMem, nullptr);
        }
    }
    idxBuf = VK_NULL_HANDLE;
    idxMem = VK_NULL_HANDLE;
    vtxBuf = VK_NULL_HANDLE;
    vtxMem = VK_NULL_HANDLE;
    bindDesc = {};
    attrDesc.clear();
    count = {0, 0};
    m_vkDevice = nullptr;
}

void VertexBufferBase::Init(VkDevice device, const MemoryAllocator* memAllocator, const std::vector<VkVertexInputAttributeDescription>& attr) {
    m_vkDevice = device;
    m_memAllocator = memAllocator;
    attrDesc = attr;
}

void VertexBufferBase::AllocateBufferMemory(VkBuffer buf, VkDeviceMemory* mem) const {
    VkMemoryRequirements memReq = {};
    vkGetBufferMemoryRequirements(m_vkDevice, buf, &memReq);
    m_memAllocator->Allocate(memReq, mem);
}
