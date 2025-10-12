/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 SDL window
 mod. 2025.09.28
 */

#ifndef _SDL_VIDEO_WRAP_H
#define _SDL_VIDEO_WRAP_H

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL.h>
    #else
        #include <SDL2/SDL.h>
    #endif
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_vulkan.h>
  #include <vulkan/vulkan.hpp>
#endif

class SDLVideo{

    int _bpp;
    int _flags;

    unsigned _width;
    unsigned _height;

    SDL_Surface * icon;
    SDL_Window* mainWindow;

    //Vulkan stuff
    std::vector<VkFence>         vkFences;
    std::vector<VkCommandBuffer> vkCommandBuffers;
    std::vector<VkImage>         vkSwapchainImages;
    std::vector<VkImageView>     vkSwapchainImageViews;
    std::vector<VkFramebuffer>   vkSwapchainFramebuffers;
    VkDevice                     vkDevice;
    VkPhysicalDevice             vkPhysicalDevice;
    VkInstance                   vkInstance;
    VkImage                      vkImage;
    VkSwapchainKHR               vkSwapchain;
    VkCommandBuffer              vkCommandBuffer;
    VkSurfaceFormatKHR           vkSurfaceFormat;
    VkSurfaceCapabilitiesKHR     vkSurfaceCapabilities;
    VkExtent2D                   vkSwapchainSize;
    VkFormat                     vkDepthFormat;
    VkRenderPass                 vkRenderPass;
    VkCommandPool                vkCommandPool;
    VkSemaphore                  vkImageAvailableSemaphore;
    VkSemaphore                  vkRenderingFinishedSemaphore;
    VkQueue                      vkGraphicsQueue;
    VkQueue                      vkPresentQueue;
    uint32_t                     vkFrameIndex;
    uint32_t                     vkSwapchainImageCount;

public:

    SDLVideo(){_width = 640; _height = 480;}

    unsigned width(){return _width;}
    unsigned height(){return _height;}
    int      colorBits(){return _bpp;}
    int      flags(){return _flags;}
    void     setMetrics(unsigned w, unsigned h);

    bool     initWindow(const char * title,
                        const char * iconPath,
                        bool isWindowed = true,
                        bool useVulkan = true);
    void     quit(bool useVulkan);

    void     beginRenderPass(bool useVulkan);
    void     swap(bool useVulkan);

    void     VkAcquireNextImage();
    void     VkResetCommandBuffer();
    void     VkBeginCommandBuffer();
    void     VkBeginRenderPass(VkClearColorValue clearColor,
                               VkClearDepthStencilValue clearDepthStencil);
    void     VkEndRenderPass();
    void     VkEndCommandBuffer();
    void     VkQueueSubmit();
    void     VkQueuePresent();

    VkDevice* getVkDevice(){return &vkDevice;}
private:
    VkImageView createImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, 
                        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, 
                        VkDeviceMemory& imageMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createSemaphore(VkSemaphore *semaphore);


};


#endif //_SDL_VIDEO_WRAP_H

