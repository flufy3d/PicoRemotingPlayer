// texture_view.h
#pragma once

#include "pch.h"
#include "common.h"


struct TextureView {
    VkImage image;
    VkImageView imageView;

    TextureView();
    ~TextureView();
    TextureView(const TextureView&) = delete;
    TextureView& operator=(const TextureView&) = delete;
    TextureView(TextureView&&) = delete;
    TextureView& operator=(TextureView&&) = delete;

    void Create(VkDevice device,const std::string& filename);
    void CreateImageView();


private:
    VkDevice m_vkDevice{VK_NULL_HANDLE};  // Assuming VkDevice is stored here
    int m_width, m_height, m_channels;
    unsigned char* m_pixels{nullptr};

    void Cleanup();
};
