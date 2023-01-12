#include "gui.h"

#include <string.h>
#include <time.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <implot.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "dstream_packet.h"
#include "queue.h"

#define PLOT_WIDTH 1000

void guiThread(void *p) {
    queue_t *q = (queue_t*)p;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window =
        SDL_CreateWindow("imgui test", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    const char *glsl_version = "#version 130";
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    for(;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT ||
                (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_CLOSE &&
                 event.window.windowID == SDL_GetWindowID(window))) {
                queueClose(q);
                goto exit;
            }
        }

        dstream_packet_t *packet;
        if (queuePopBlock(q, (void**)&packet) == -1) {
            break;
        }

        int d_ty;
        const char *nm;
        void *data;
        size_t sz;
        dstreamPacketUnpack(packet, &d_ty, &nm, &data, &sz);
        const size_t len = dstreamPacketGetDataLen(d_ty, sz);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("imgui-test");

        static float y[PLOT_WIDTH] = {0};
        memmove(y, &y[len], sizeof(y) - len * sizeof(*y));

        for (size_t i = 0; i < len; i++) {
            const size_t yi = PLOT_WIDTH - len + i;
            switch(d_ty) {
            case U8:
                y[yi] = *(uint8_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case U16:
                y[yi] = *(uint16_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case U32:
                y[yi] = *(uint32_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case U64:
                y[yi] = *(uint64_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case I8:
                y[yi] = *(int8_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case I16:
                y[yi] = *(int16_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case I32:
                y[yi] = *(int32_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case I64:
                y[yi] = *(int64_t*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case F32:
                y[yi] = *(float*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            case F64:
                y[yi] = *(double*)dstreamPacketGetDataElem(data, d_ty, sz, i);
                break;
            default:
                assert(false);
                break;
            }
        }

        free(packet);

        if (ImPlot::BeginPlot("Line Plots")) {
            ImPlot::SetupAxes("x", "y");
            ImPlot::PlotLine("f(x)", y, sizeof(y) / sizeof(*y));
            ImPlot::EndPlot();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
exit:

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
