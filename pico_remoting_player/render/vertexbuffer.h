// vertexbuffer.h
#pragma once

#include "pch.h"
#include "common.h"

template <typename T>
struct VertexBuffer : public VertexBufferBase {
    bool Create(uint32_t idxCount, uint32_t vtxCount) {
        VkBufferCreateInfo bufInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufInfo.size = sizeof(uint16_t) * idxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &idxBuf));
        AllocateBufferMemory(idxBuf, &idxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, idxBuf, idxMem, 0));

        bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufInfo.size = sizeof(T) * vtxCount;
        CHECK_VKCMD(vkCreateBuffer(m_vkDevice, &bufInfo, nullptr, &vtxBuf));
        AllocateBufferMemory(vtxBuf, &vtxMem);
        CHECK_VKCMD(vkBindBufferMemory(m_vkDevice, vtxBuf, vtxMem, 0));

        bindDesc.binding = 0;
        bindDesc.stride = sizeof(T);
        bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        count = {idxCount, vtxCount};

        return true;
    }

    void UpdateIndicies(const uint16_t* data, uint32_t elements, uint32_t offset = 0) {
        uint16_t* map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, idxMem, sizeof(map[0]) * offset, sizeof(map[0]) * elements, 0, (void**)&map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, idxMem);
    }

    void UpdateVertices(const T* data, uint32_t elements, uint32_t offset = 0) {
        T* map = nullptr;
        CHECK_VKCMD(vkMapMemory(m_vkDevice, vtxMem, sizeof(map[0]) * offset, sizeof(map[0]) * elements, 0, (void**)&map));
        for (size_t i = 0; i < elements; ++i) {
            map[i] = data[i];
        }
        vkUnmapMemory(m_vkDevice, vtxMem);
    }
};
