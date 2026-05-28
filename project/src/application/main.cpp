#pragma once
#include <iostream>
#include "SponzaApplication.h"

int main() {
    dae::SponzaApplication app{};

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