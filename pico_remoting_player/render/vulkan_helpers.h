#pragma once

#include "pch.h"
#include "common.h"

static std::string vkResultString(VkResult res) {
    switch (res) {
        case VK_SUCCESS:
            return "SUCCESS";
        case VK_NOT_READY:
            return "NOT_READY";
        case VK_TIMEOUT:
            return "TIMEOUT";
        case VK_EVENT_SET:
            return "EVENT_SET";
        case VK_EVENT_RESET:
            return "EVENT_RESET";
        case VK_INCOMPLETE:
            return "INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "ERROR_INVALID_SHADER_NV";
        default:
            return std::to_string(res);
    }
}

[[noreturn]] inline void ThrowVkResult(VkResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    Throw(Fmt("VkResult failure [%s]", vkResultString(res).c_str()), originator, sourceLocation);
}

inline VkResult CheckVkResult(VkResult res, const char* originator = nullptr, const char* sourceLocation = nullptr) {
    if ((res) < VK_SUCCESS) {
        ThrowVkResult(res, originator, sourceLocation);
    }

    return res;
}

// XXX These really shouldn't have trailing ';'s
#define THROW_VK(res, cmd) ThrowVkResult(res, #cmd, FILE_AND_LINE);
#define CHECK_VKCMD(cmd) CheckVkResult(cmd, #cmd, FILE_AND_LINE);
#define CHECK_VKRESULT(res, cmdStr) CheckVkResult(res, cmdStr, FILE_AND_LINE);


inline VkBool32 debugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t /*location*/,
                     int32_t /*messageCode*/, const char* pLayerPrefix, const char* pMessage) {
    std::string flagNames;
    std::string objName;
    Log::Level level = Log::Level::Error;

    if ((flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0u) {
        flagNames += "DEBUG:";
        level = Log::Level::Verbose;
    }
    if ((flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0u) {
        flagNames += "INFO:";
        level = Log::Level::Info;
    }
    if ((flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0u) {
        flagNames += "PERF:";
        level = Log::Level::Warning;
    }
    if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u) {
        flagNames += "WARN:";
        level = Log::Level::Warning;
    }
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u) {
        flagNames += "ERROR:";
        level = Log::Level::Error;
    }

#define LIST_OBJECT_TYPES(_) \
    _(UNKNOWN)               \
    _(INSTANCE)              \
    _(PHYSICAL_DEVICE)       \
    _(DEVICE)                \
    _(QUEUE)                 \
    _(SEMAPHORE)             \
    _(COMMAND_BUFFER)        \
    _(FENCE)                 \
    _(DEVICE_MEMORY)         \
    _(BUFFER)                \
    _(IMAGE)                 \
    _(EVENT)                 \
    _(QUERY_POOL)            \
    _(BUFFER_VIEW)           \
    _(IMAGE_VIEW)            \
    _(SHADER_MODULE)         \
    _(PIPELINE_CACHE)        \
    _(PIPELINE_LAYOUT)       \
    _(RENDER_PASS)           \
    _(PIPELINE)              \
    _(DESCRIPTOR_SET_LAYOUT) \
    _(SAMPLER)               \
    _(DESCRIPTOR_POOL)       \
    _(DESCRIPTOR_SET)        \
    _(FRAMEBUFFER)           \
    _(COMMAND_POOL)          \
    _(SURFACE_KHR)           \
    _(SWAPCHAIN_KHR)         \
    _(DISPLAY_KHR)           \
    _(DISPLAY_MODE_KHR)

        switch (objectType) {
            default:
#define MK_OBJECT_TYPE_CASE(name)                  \
    case VK_DEBUG_REPORT_OBJECT_TYPE_##name##_EXT: \
        objName = #name;                           \
        break;
                LIST_OBJECT_TYPES(MK_OBJECT_TYPE_CASE)

#if VK_HEADER_VERSION >= 46
                MK_OBJECT_TYPE_CASE(DESCRIPTOR_UPDATE_TEMPLATE_KHR)
#endif
#if VK_HEADER_VERSION >= 70
                MK_OBJECT_TYPE_CASE(DEBUG_REPORT_CALLBACK_EXT)
#endif
    }

    if ((objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT) && (strcmp(pLayerPrefix, "Loader Message") == 0) &&
        (strncmp(pMessage, "Device Extension:", 17) == 0)) {
        return VK_FALSE;
    }

    Log::Write(level, Fmt("%s (%s 0x%llx) [%s] %s", flagNames.c_str(), objName.c_str(), object, pLayerPrefix, pMessage));
    if ((flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0u) {
        return VK_FALSE;
    }
    if ((flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0u) {
        return VK_FALSE;
    }
    return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportThunk(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
                                                       uint64_t object, size_t location, int32_t messageCode,
                                                       const char* pLayerPrefix, const char* pMessage, void* pUserData) {
    return debugReport(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
}


inline XrStructureType GetGraphicsBindingType()  { return XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR; }
inline XrStructureType GetSwapchainImageType()  { return XR_TYPE_SWAPCHAIN_IMAGE_VULKAN2_KHR; }

inline XrResult CreateVulkanInstanceKHR(XrInstance instance, const XrVulkanInstanceCreateInfoKHR* createInfo,
                                         VkInstance* vulkanInstance, VkResult* vulkanResult) {
    PFN_xrCreateVulkanInstanceKHR pfnCreateVulkanInstanceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanInstanceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnCreateVulkanInstanceKHR)));

    return pfnCreateVulkanInstanceKHR(instance, createInfo, vulkanInstance, vulkanResult);
}

inline XrResult CreateVulkanDeviceKHR(XrInstance instance, const XrVulkanDeviceCreateInfoKHR* createInfo,
                                       VkDevice* vulkanDevice, VkResult* vulkanResult) {
    PFN_xrCreateVulkanDeviceKHR pfnCreateVulkanDeviceKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrCreateVulkanDeviceKHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnCreateVulkanDeviceKHR)));

    return pfnCreateVulkanDeviceKHR(instance, createInfo, vulkanDevice, vulkanResult);
}

inline XrResult GetVulkanGraphicsDevice2KHR(XrInstance instance, const XrVulkanGraphicsDeviceGetInfoKHR* getInfo,
                                             VkPhysicalDevice* vulkanPhysicalDevice) {
    PFN_xrGetVulkanGraphicsDevice2KHR pfnGetVulkanGraphicsDevice2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDevice2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsDevice2KHR)));

    return pfnGetVulkanGraphicsDevice2KHR(instance, getInfo, vulkanPhysicalDevice);
}

inline XrResult GetVulkanGraphicsRequirements2KHR(XrInstance instance, XrSystemId systemId,
                                                   XrGraphicsRequirementsVulkan2KHR* graphicsRequirements) {
    PFN_xrGetVulkanGraphicsRequirements2KHR pfnGetVulkanGraphicsRequirements2KHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirements2KHR",
                                      reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetVulkanGraphicsRequirements2KHR)));

    return pfnGetVulkanGraphicsRequirements2KHR(instance, systemId, graphicsRequirements);
}