#include <chrono>
#include <glm/packing.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <SDL3/SDL.h>

#include "config.hpp"
#include "drawcall.hpp"
#include "frame.hpp"
#include "render.hpp"
#include "shader_type.hpp"

struct Vertex {
    const glm::vec4& position() const noexcept { return pos; }

    Vertex           interpolate(const Vertex& other, float t) const noexcept {
        return { pos + t * other.pos };
    }

    glm::vec4 pos;
};

int main() {

    rohan::RenderConfig config;
    config.cull_backfaces(true);
    config.winding_order(rohan::WindingOrder::CW);

    rohan::RenderState state;
    state.config(config);

    rohan::Program program = {
        {},
        [](const rohan::UniformBuffer& ub, uint64_t index, uint32_t& out) {
            auto bc = ub.get_uniform<glm::vec3>("bc");
            out     = glm::packUnorm4x8(glm::vec4(1.f, bc));
        },
        [](const rohan::UniformBuffer& ub, uint64_t index, const glm::vec4& in) { return in; },
    };
    state.program(program);

    uint32_t              width = 1440, height = 900;
    rohan::FrameU32       color_buffer(width, height);
    rohan::FrameF32       depth_buffer(width, height);

    std::vector<Vertex>   vertices = { { glm::vec4(-1.f, 1.f, 0.f, 1.f) },
                                       { glm::vec4(1.f, 1.f, 0.f, 1.f) },
                                       { glm::vec4(1.f, -1.f, 0.f, 1.f) },
                                       { glm::vec4(-1.f, -1.f, 0.f, 1.f) } };
    std::vector<uint32_t> indices  = { 0, 1, 2, 0, 2, 3 };

    SDL_Window*   window   = SDL_CreateWindow("Rohan - Sandbox", 1440, 900, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture*  frame    = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height
    );
    SDL_SetTextureScaleMode(frame, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(frame, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    float    total_ms     = 0.f;
    uint64_t total_frames = 0;

    for (auto keys = SDL_GetKeyboardState(nullptr); !keys[SDL_SCANCODE_Q]; SDL_PumpEvents()) {
        auto t0 = std::chrono::system_clock::now();

        {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                case SDL_EVENT_QUIT: return 0;
                case SDL_EVENT_WINDOW_RESIZED: {
                    int32_t new_width  = e.window.data1;
                    int32_t new_height = e.window.data2;
                    color_buffer.resize(new_width, new_height);
                    depth_buffer.resize(new_width, new_height);

                    SDL_DestroyTexture(frame);
                    frame = SDL_CreateTexture(
                        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, new_width,
                        new_height
                    );
                    break;
                }
                }
            }
        }

        color_buffer.draw(state, depth_buffer, vertices, indices);

        SDL_UpdateTexture(frame, nullptr, color_buffer.data(), color_buffer.pitch());
        SDL_RenderTexture(renderer, frame, nullptr, nullptr);

        float frametime_ms = (std::chrono::system_clock::now() - t0).count() * 1E-6f;
        SDL_RenderDebugTextFormat(
            renderer, 15.f, 15.f, "ms: %.3f fps: %.3f", frametime_ms, 1E3f / frametime_ms
        );

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
        std::fill_n(color_buffer.data(), color_buffer.count(), 0x000000ff);

        total_ms += frametime_ms;
        total_frames += 1;
    }

    printf("average perf: %.4f ms\n", total_ms / total_frames);
    return 0;
}
