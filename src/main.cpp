// Local includes
#include "rtweekend.hpp"

#include "scene/material.hpp"
#include "scene/hittable_list.hpp"
#include "scene/sphere.hpp"
#include "scene/camera.hpp"
#include "image/image_exporter.hpp"
#include "graphics/cpu_renderer.hpp"

// ImGui includes
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// STL includes
#include <stdio.h>
#include <thread>

// External includes
#include <argparse/argparse.hpp>

void setup_args(int argc, char** argv, argparse::ArgumentParser& argparse);

int main(int argc, char** argv)
{
    using namespace jmrtiow;

    argparse::ArgumentParser argparser("rtiow", "0.1");
    setup_args(argc, argv, argparser);

    std::string filepath = argparser.get<std::string>("--filepath");
    std::string image_type_string = argparser.get("--image-type");
    image::image_type image_type_selection = image::image_type::Unknown;
    std::string scene_type = argparser.get<std::string>("--scene");

    // Image

    const double aspect_ratio = 3.0 / 2.0;
    const uint32_t image_width = 720;
    const uint32_t image_height = static_cast<int>(image_width / aspect_ratio);
    const uint32_t samples_per_pixel = 1;
    const uint32_t max_depth = 25;

    // World

    scene::hittable_list world;
    if (scene_type.compare("demo2") == 0)
        world = scene::demo_scene2();
    else if (scene_type.compare("demo") == 0)
        world = scene::demo_scene();
    else
        world = scene::random_scene();

    math::point3 lookfrom(13, 2, 3);
    math::point3 lookat(0, 0, 0);
    math::vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    scene::camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render

    // Image data as R,G,B math::vec3, no alpha.

    math::color3* image_data = new math::color3[image_height * image_width];
    math::color3** image_data_reference = &image_data;

    for (size_t i = 0; i < image_height * image_width; i++)
    {
        image_data[i] = { 0.0, 0.0, 0.0 };
    }


    bool pause = false;
    float frame = 0;

    // Create rt rendering context and renderer.
    // TODO: Enable switching of multiple renderers.
    graphics::renderer_context rt_context {
        .camera = &cam,
        .scene = &world,
        .max_depth = max_depth,
        .samples_per_pixel = samples_per_pixel,
        .pause = &pause,
        .blend_callback = [](const math::color3& a, const math::color3& b, const uint32_t& iteration)->math::color3
        {
            uint32_t n = iteration + 1;
            return a * (n - 1) / n + b / n;
        },
    };

    graphics::cpu_renderer rt_renderer {};

    uint32_t threads_supported = std::thread::hardware_concurrency() * 0.75f;
    std::vector<std::thread> work_threads {};
    work_threads.reserve(threads_supported);
    for (size_t i = 1; i < threads_supported + 1; i++)
    {
        // Should change this from a lambda to its own function for clarity.
        uint32_t local_image_range = image_width / threads_supported;
        uint32_t local_image_height = image_height;
        uint32_t local_image_width = local_image_range * i;
        uint32_t local_image_start = local_image_width - local_image_range;

        // On the last thread, we should set the height and range to the height of the image.
        if (i == threads_supported)
            local_image_width = image_width;

        std::cout << "Info for thread " << i << ":\n";
        std::cout << "image range: " << local_image_range << "\n";
        std::cout << "image height: " << local_image_height << "\n";
        std::cout << "image start: " << local_image_start << "\n";
        std::cout << "image width: " << local_image_width << "\n";
        work_threads.emplace_back([&rt_renderer, &rt_context, image_data_reference, &pause, &frame, threads_supported, image_height, image_width, local_image_width, local_image_height, local_image_start, local_image_range]()
            {
                graphics::view_context local_view_context {
                    .data = image_data_reference,
                    .iteration = 0,
                    .data_height = image_height,
                    .data_width = image_width,
                    .x = local_image_start,
                    .y = 0,
                    .height = local_image_height,
                    .width = local_image_range
                };

                while (!pause)
                {
                    rt_renderer.render(rt_context, local_view_context);
                    frame += (1.0f / threads_supported);
                    local_view_context.iteration++;
                }
            });
    }

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image_width, image_height, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }

    // SDL_RendererInfo info;
    // SDL_GetRendererInfo(renderer, &info);
    // SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = false;
    int stride = 0;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Texture* render_surface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, image_width, image_height);

    std::cout << "Image width " << image_width << " height " << image_height << '\n';

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Information"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Application width %.0f, height %.0f", ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y);
            ImGui::Text("Image stride %d", stride);
            ImGui::Text("Active Threads %d", threads_supported);
            ImGui::Text("Frame %.2f", frame);
            ImGui::Checkbox("Toggle Demo Window", &show_demo_window);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        // Convert double to bytes
        auto image_bytes = jmrtiow::image::convert_to_bytes_rgba(image_data, image_width * image_height);

        // Get pixels of the texture
        char* image_now;
        SDL_LockTexture(render_surface, NULL, (void**)&image_now, &stride);

        // int w, h;
        // unsigned int format;
        // SDL_QueryTexture(render_surface, &format, NULL, &w, &h);
        // std::cout << "Format is " << format << '\n';
        // std::cout << "Stride is " << stride << '\n';
        // std::cout << "Format type is " << SDL_PIXELTYPE(format) << '\n';

        // Copy texture data to new texture
        for (size_t i = 0; i < image_height; i++)
        {
            memcpy(&image_now[i * stride], &image_bytes.data()[(image_height - i) * image_width], stride);
        }

        SDL_UnlockTexture(render_surface);
        SDL_RenderCopy(renderer, render_surface, NULL, NULL);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    pause = true;
    for (size_t i = 0; i < work_threads.size(); i++)
    {
        work_threads[i].join();
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void setup_args(int argc, char** argv, argparse::ArgumentParser& argparser)
{
    argparser.add_argument("--image-type", "-t")
        .default_value(std::string { "png" })
        .choices("png", "jpg", "jpeg", "bmp", "tga", "hdr", "ppm", "webp")
        .nargs(1)
        .help("The type of image to output [choices: png, jpg, jpeg, bmp, tga, hdr, ppm, webp]")
        .metavar("TYPE");

    argparser.add_argument("--filepath", "-f")
        .default_value(std::string { "img.png" })
        .help("The file location to output to")
        .metavar("PATH");

    argparser.add_argument("--scene", "-s")
        .default_value(std::string { "random" })
        .choices("random", "demo", "demo2")
        .help("The scene to render")
        .metavar("SCENE");

    try
    {
        argparser.parse_args(argc, argv);
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << argparser;
        std::exit(1);
    }
}
