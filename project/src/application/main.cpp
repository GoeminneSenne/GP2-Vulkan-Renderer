#pragma once
#include <iostream>

//#define VULKAN_DEMO_APP
//#define SPONZA_APP
//#define RENDERPASS_EXAMPLE_APP
#define DEFERRED_RENDERING_APPLICATION

#ifdef VULKAN_DEMO_APP
#include "VulkanDemoApplication.h"
typedef dae::VulkanDemoApplication application;
#endif

#ifdef SPONZA_APP
#include "SponzaApplication.h"
typedef dae::SponzaApplication application;
#endif

#ifdef RENDERPASS_EXAMPLE_APP
#include "RenderPassExample.h"
typedef dae::RenderPassExampleApplication application;
#endif

#ifdef DEFERRED_RENDERING_APPLICATION
#include "DeferredApplication.h"
typedef dae::DeferredApplication application;
#endif

int main() {
    application app{};

    try
    {
        app.Init();

        while (!app.ShouldClose())
        {
            app.PollEvents();
            app.Update();
            app.DrawFrame();
        }

        app.Cleanup();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}