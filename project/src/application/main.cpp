#pragma once
#include <iostream>
#include "SponzaApplication.h"
//#include "VulkanDemoApplication.h"

int main() {
    dae::SponzaApplication app{};
    //dae::VulkanDemoApplication app{};

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