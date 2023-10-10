// texture_view.h
#pragma once

#include "pch.h"
#include "common.h"

class MemoryAllocator;
class CmdBuffer;

struct TextureView {
    VkImage image;
    VkImageView imageView;

    TextureView();
    ~TextureView();
    TextureView(const TextureView&) = delete;
    TextureView& operator=(const TextureView&) = delete;
    TextureView(TextureView&&) = delete;
    TextureView& operator=(TextureView&&) = delete;

    void Create(VkDevice device,VkQueue vkQueue,MemoryAllocator* memAllocator,CmdBuffer* cmdBuffer, const std::string& filename);



private:
    void CreateTextureImage();
    void CreateImageView();

    void TransitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    );

    void CopyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height
    );

    VkDevice m_vkDevice{VK_NULL_HANDLE};  // Assuming VkDevice is stored here
    VkQueue m_vkQueue{VK_NULL_HANDLE};  // Assuming VkQueue is stored here
    int m_width, m_height, m_channels;
    unsigned char* m_pixels{nullptr};
    MemoryAllocator* m_memAllocator;
    CmdBuffer* m_cmdBuffer;

    VkFormat m_format;

    void Cleanup();
};
