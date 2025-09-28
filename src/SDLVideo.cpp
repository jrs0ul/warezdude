/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 SDL window
 mod. 2025.09.28
 */

#include "SDLVideo.h"
#include <cstdio>

bool SDLVideo::InitWindow(const char * title,
                          const char * iconPath,
                          bool isWindowed,
                          bool useVulkan)
{


    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        return false;
    }

    int rSize, gSize, bSize, aSize;
    rSize = gSize = bSize = aSize = 8;
    int buffsize = ((rSize + gSize + bSize + aSize + 7) & 0xf8);

    if (!useVulkan)
    {

        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8);
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, rSize );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, gSize );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, bSize );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, aSize );
        SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, buffsize );

        _flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    }
    else  //Vulkan
    {
        SDL_Vulkan_LoadLibrary(nullptr);

        _flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    }

    if (!isWindowed){
        _flags = _flags |SDL_WINDOW_FULLSCREEN;
    }

    icon = SDL_LoadBMP(iconPath);
   

    mainWindow = SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         _width,
                         _height,
                         _flags );
    if( mainWindow == 0 )
    {
        puts("can't set video mode");
        return false;
    }

    if (icon)
    {
         SDL_SetWindowIcon(mainWindow, icon);
    }

    if (!useVulkan)
    {
        SDL_GL_CreateContext(mainWindow);
    }
    else //  Vulkan
    {
        VkInstance vkInstance;

        uint32_t extensionCount;
        const char** extensionNames = 0;

        SDL_Vulkan_GetInstanceExtensions(mainWindow, &extensionCount, nullptr);
        extensionNames = new const char *[extensionCount];
        SDL_Vulkan_GetInstanceExtensions(mainWindow, &extensionCount, extensionNames);

        const VkInstanceCreateInfo instanceInfo = {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
            nullptr,                                // pNext
            0,                                      // flags
            nullptr,                                // pApplicationInfo
            0,                                      // enabledLayerCount
            nullptr,                                // ppEnabledLayerNames
            extensionCount,                         // enabledExtensionCount
            extensionNames,                         // ppEnabledExtensionNames
        };

        vkCreateInstance(&instanceInfo, nullptr, &vkInstance);

        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(mainWindow, vkInstance, &surface);

        uint32_t physicalDeviceCount;
        vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
        VkPhysicalDevice physicalDevice = physicalDevices[0];


        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        uint32_t graphicsQueueIndex = UINT32_MAX;
        uint32_t presentQueueIndex = UINT32_MAX;

        VkBool32 support;
        uint32_t i = 0;

        for (VkQueueFamilyProperties queueFamily : queueFamilies) 
        {
            if (graphicsQueueIndex == UINT32_MAX && 
                    queueFamily.queueCount > 0 && 
                    queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueIndex = i;
            }
            if (presentQueueIndex == UINT32_MAX) 
            {
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &support);

                if(support)
                {
                    presentQueueIndex = i;
                }
            }
            ++i;
        }

        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueInfo = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
            nullptr,                                    // pNext
            0,                                          // flags
            graphicsQueueIndex,                         // graphicsQueueIndex
            1,                                          // queueCount
            &queuePriority,                             // pQueuePriorities
        };

        VkPhysicalDeviceFeatures deviceFeatures = {};
        const char* deviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo deviceCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,   // sType
            nullptr,                                // pNext
            0,                                      // flags
            1,                                      // queueCreateInfoCount
            &queueInfo,                             // pQueueCreateInfos
            0,                                      // enabledLayerCount
            nullptr,                                // ppEnabledLayerNames
            1,                                      // enabledExtensionCount
            deviceExtensionNames,                   // ppEnabledExtensionNames
            &deviceFeatures,                        // pEnabledFeatures
        };

        VkDevice device;
        vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

        VkQueue graphicsQueue;
        vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);

        VkQueue presentQueue;
        vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);
    }

    printf("Context created\n");

    return true;


}
//-------------------------------------------
void SDLVideo::setMetrics(unsigned w, unsigned h){
    _width = w;
    _height = h;
}

//--------------------------------------------
void SDLVideo::Quit(){

    SDL_Quit();
}

void SDLVideo::swap()
{
    SDL_GL_SwapWindow(mainWindow);
}
