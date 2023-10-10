// texture_view.cpp
#include "texture_view.h"

#include "my_asset_manager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cmd_buffer.h"
#include "memory_allocator.h"

TextureView::TextureView() {

}

TextureView::~TextureView() {
    Cleanup();
}



void TextureView::Create(VkDevice device,VkQueue vkQueue,MemoryAllocator* memAllocator,CmdBuffer* cmdBuffer, const std::string& filename) {
    m_vkDevice = device;
    m_memAllocator = memAllocator;
    m_cmdBuffer = cmdBuffer;
    m_vkQueue = vkQueue;

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
    // Logging image size and format
    Log::Write(Log::Level::Info, "Image size: " + std::to_string(m_width) + "x" + std::to_string(m_height));
    std::string format;
    switch(m_channels) {
        case 1: format = "Gray"; break;
        case 3: format = "RGB";  m_format = VK_FORMAT_R8G8B8_SRGB; break;
        case 4: format = "RGBA"; m_format = VK_FORMAT_R8G8B8A8_SRGB;break;
        default: format = "Unknown"; break;
    }
    Log::Write(Log::Level::Info, "Image format: " + format);

    // Create Vulkan image from loaded pixels
    CreateTextureImage();


    // Create Vulkan image view from image
    CreateImageView();
}

void TextureView::CreateTextureImage() {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceMemory imageMemory;

    VkDeviceSize imageSize = m_width * m_height * m_channels;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_vkDevice, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        THROW("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_vkDevice, stagingBuffer, &memRequirements);
    m_memAllocator->Allocate(memRequirements, &stagingBufferMemory);

    vkBindBufferMemory(m_vkDevice, stagingBuffer, stagingBufferMemory, 0);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(m_vkDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        THROW("Failed to create image");
    }

    vkGetImageMemoryRequirements(m_vkDevice, image, &memRequirements);
    m_memAllocator->Allocate(memRequirements, &imageMemory);

    vkBindImageMemory(m_vkDevice, image, imageMemory, 0);

    m_cmdBuffer->Begin();
    TransitionImageLayout(
        image,
        m_format,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );
    CopyBufferToImage(
        stagingBuffer,
        image,
        static_cast<uint32_t>(m_width),
        static_cast<uint32_t>(m_height)
    );
    TransitionImageLayout(
        image,
        m_format,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );
    m_cmdBuffer->End();
    m_cmdBuffer->Exec(m_vkQueue);
    m_cmdBuffer->Wait();

    vkDestroyBuffer(m_vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_vkDevice, stagingBufferMemory, nullptr);

}


void TextureView::CreateImageView() {
    
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

void TextureView::TransitionImageLayout(
    VkImage image,
    VkFormat format,
    VkImageLayout oldLayout,
    VkImageLayout newLayout
) {
    VkCommandBuffer commandBuffer = m_cmdBuffer->buf;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                      

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_vkQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_vkQueue);
}

void TextureView::CopyBufferToImage(
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height
) {
    VkCommandBuffer commandBuffer = m_cmdBuffer->buf;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_vkQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_vkQueue);
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
