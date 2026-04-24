#pragma once
#include <iostream>
#include "Application.h"

int main() {
    dae::Application app{};

    try
    {
        app.Init();

        while (!app.ShouldClose())
        {
            app.PollEvents();
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