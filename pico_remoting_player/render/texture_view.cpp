// texture_view.cpp
#include "texture_view.h"

#include "my_asset_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureView::TextureView() {

}

TextureView::~TextureView() {
    Cleanup();
}



void TextureView::Create(VkDevice device, const std::string& filename) {
    m_vkDevice = device;
    // Load asset data
    AssetData assetData = MyAssetManager::Instance().LoadAsset(filename.c_str());
    Log::Write(Log::Level::Info, "Loaded " + filename + " :" + std::to_string(assetData.length));

    // Load image using stb
    m_pixels = stbi_load_from_memory(
            reinterpret_cast<const unsigned char*>(assetData.data),
            assetData.length, &m_width, &m_height, &m_channels, 0
    );
    if (!m_pixels) {
        Log::Write(Log::Level::Error, "Failed to load texture image!");
        return;
    }

    // TODO: Create Vulkan image from loaded pixels

    CreateImageView();
}

void TextureView::CreateImageView() {
    // TODO: Create Vulkan image view from image
}

void TextureView::Cleanup() {
    // Cleanup Vulkan resources
    if (imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_vkDevice, imageView, nullptr);
        imageView = VK_NULL_HANDLE;
    }
    if (image != VK_NULL_HANDLE) {
        vkDestroyImage(m_vkDevice, image, nullptr);
        image = VK_NULL_HANDLE;
    }

    // Cleanup stb image data
    if (m_pixels) {
        stbi_image_free(m_pixels);
        m_pixels = nullptr;
    }
}
