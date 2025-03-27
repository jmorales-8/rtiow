// Local includes
#include "rtweekend.hpp"

#include "scene/material.hpp"
#include "scene/hittable_list.hpp"
#include "scene/sphere.hpp"
#include "scene/camera.hpp"
#include "image/image_exporter.hpp"

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

void setup_args(int argc, char **argv, argparse::ArgumentParser &argparse);

int main(int argc, char **argv)
{
    using namespace jmrtiow;

    argparse::ArgumentParser argparser("rtiow", "0.1");
    setup_args(argc, argv, argparser);

    std::string filepath = argparser.get<std::string>("--filepath");
    std::string image_type_string = argparser.get("--image-type");
    image::image_type image_type_selection = image::image_type::Unknown;
    std::string scene_type = argparser.get<std::string>("--scene");

    // Image

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 720;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1;
    const int max_depth = 25;

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

    std::vector<math::color3> image_data{};
    std::vector<math::color3> *image_data_reference = &image_data;
    image_data.resize(image_height * image_width, {0.0, 0.0, 0.0});

    bool keep_working = true;
    bool *keep_working_reference = &keep_working;
    float frame = 0;

    uint32_t threads_supported = 7; // std::thread::hardware_concurrency() * 0.75f;
    std::vector<std::thread> work_threads{};
    work_threads.reserve(threads_supported);
    for (size_t i = 1; i < threads_supported + 1; i++)
    {
        // Should change this from a lambda to its own function for clarity.
        uint32_t local_image_range = image_height / threads_supported;
        uint32_t local_image_height = local_image_range * i;
        uint32_t local_image_start = local_image_height - local_image_range;
        uint32_t local_image_width = image_width;

        // On the last thread, we should set the height and range to the height of the image.
        if (i == threads_supported)
            local_image_height = image_height;

        std::cout << "Info for thread " << i << ":\n";
        std::cout << "image range: " << local_image_range << "\n";
        std::cout << "image height: " << local_image_height << "\n";
        std::cout << "image start: " << local_image_start << "\n";
        std::cout << "image width: " << local_image_width << "\n";
        work_threads.emplace_back([image_data_reference, keep_working_reference, cam, image_width, image_height, local_image_start, local_image_width, local_image_height, world, max_depth, &frame, threads_supported]()
                                  {
        uint32_t local_frame = 0;
        while (*keep_working_reference)
        {
            for (int j = local_image_start; j < local_image_height; j++)
            {
                for (int i = 0; i < local_image_width; i++)
                {
                    math::color3 pixel_color(0, 0, 0);
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    math::ray r = cam.get_ray(u, v);
                    pixel_color += jmrtiow::scene::ray_color(r, world, max_depth);

                    // For better readability.
                    auto& image_data_element = (*image_data_reference)[j * image_width + i];
                    uint32_t number_of_samples = local_frame + 1;

                    // Pixel data is normalized, be sure to un-normalize it before averaging.
                    image_data_element = image_data_element * image_data_element;

                    // Add new color and average
                    // New average = old average * (n-1)/n + new value /n
                    image_data_element = (image_data_element * (number_of_samples - 1) / number_of_samples + pixel_color / number_of_samples);

                    // image_data_element = (pixel_color / number_of_samples);
                    // if (local_frame)
                    //     // TODO: This blending drops off in effectiveness very quickly. Some other blending should be used instead.
                    //     image_data_element /= (1.0 + (1.0 / (double)number_of_samples));

                    // Normalize the color samples and gamma correct before passing off to pixel data.
                    image_data_element.r = sqrt(image_data_element.r);
                    image_data_element.g = sqrt(image_data_element.g);
                    image_data_element.b = sqrt(image_data_element.b);
                }

                if (!*keep_working_reference)
                {
                    break;
                }
            }
            frame += (1.0f / threads_supported);
            local_frame++;
        } });
    }

    for (size_t i = 0; i < work_threads.size(); i++)
    {
        std::cout << "Thread " << i << " is joinable? " << work_threads[i].joinable() << '\n';
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
    SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, image_width, image_height, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
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
    ImGuiIO &io = ImGui::GetIO();
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

    SDL_Texture *render_surface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, image_width, image_height);

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
            ImGui::Text("Frame %f, Spp %f", frame, frame);
            ImGui::Checkbox("Toggle Demo Window", &show_demo_window);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        // Convert double to bytes
        auto image_bytes = jmrtiow::image::convert_to_bytes_rgba(image_data);

        // Get pixels of the texture
        char *image_now;
        SDL_LockTexture(render_surface, NULL, (void **)&image_now, &stride);

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
    keep_working = false;
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

void setup_args(int argc, char **argv, argparse::ArgumentParser &argparser)
{
    argparser.add_argument("--image-type", "-t")
        .default_value(std::string{"png"})
        .choices("png", "jpg", "jpeg", "bmp", "tga", "hdr", "ppm", "webp")
        .nargs(1)
        .help("The type of image to output [choices: png, jpg, jpeg, bmp, tga, hdr, ppm, webp]")
        .metavar("TYPE");

    argparser.add_argument("--filepath", "-f")
        .default_value(std::string{"img.png"})
        .help("The file location to output to")
        .metavar("PATH");

    argparser.add_argument("--scene", "-s")
        .default_value(std::string{"random"})
        .choices("random", "demo", "demo2")
        .help("The scene to render")
        .metavar("SCENE");

    try
    {
        argparser.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << argparser;
        std::exit(1);
    }
}
