#include "render.hpp"

#include <algorithm>
#include <glm/packing.hpp>

#include "barycentric.hpp"
#include "config.hpp"
#include "transform.hpp"

namespace {

    auto vec_x_less          = [](auto& lhs, auto& rhs) { return lhs.x < rhs.x; };

    auto get_flat_top_slopes = [](const auto& p0, const auto& p1, const auto& p2) {
        float inv_p0p2_y = 1.f / (p2.y - p0.y);
        float inv_p0p1_x = 1.f / (p1.x - p0.x);

        float slope_xl   = (p2.x - p0.x) * inv_p0p2_y;
        float slope_xr   = (p2.x - p1.x) / (p2.y - p1.y);

        float slope_z0   = (p1.z - p0.z) * inv_p0p1_x;
        float slope_z1   = (p2.z - p0.z) * inv_p0p2_y;

        float slope_w0   = (p1.w - p0.w) * inv_p0p1_x;
        float slope_w1   = (p2.w - p0.w) * inv_p0p2_y;

        return std::array{ p0.x,     p1.x,     p0.y,     p2.y,     p0.z,     p0.w,
                           slope_xl, slope_xr, slope_z0, slope_z1, slope_w0, slope_w1 };
    };

    auto get_flat_bottom_slopes = [](const auto& p0, const auto& p1, const auto& p2) {
        float inv_p0p2_y = 1.f / (p2.y - p0.y);
        float inv_p1p2_x = 1.f / (p2.x - p1.x);

        float slope_xl   = (p1.x - p0.x) * inv_p0p2_y;
        float slope_xr   = (p2.x - p0.x) * inv_p0p2_y;

        float slope_z0   = (p2.z - p1.z) * inv_p1p2_x;
        float slope_z1   = (p1.z - p0.z) * inv_p0p2_y;

        float slope_w0   = (p2.w - p1.w) * inv_p1p2_x;
        float slope_w1   = (p1.w - p0.w) * inv_p0p2_y;

        return std::array{ p0.x,     p0.x,     p0.y,     p1.y,     p0.z,     p0.w,
                           slope_xl, slope_xr, slope_z0, slope_z1, slope_w0, slope_w1 };
    };

} // namespace

namespace rohan {

    // render directly from ndc-coordinates
    void render_triangle_direct(
        FrameU32& color_buffer, FrameF32& depth_buffer, RenderState& state, glm::vec4 p0,
        glm::vec4 p1, glm::vec4 p2
    ) {

        p0                   = ndc_to_screen(p0, color_buffer.width(), color_buffer.height());
        p1                   = ndc_to_screen(p1, color_buffer.width(), color_buffer.height());
        p2                   = ndc_to_screen(p2, color_buffer.width(), color_buffer.height());

        RenderConfig& config = state.config();
        Barycentric   bary(p0, p1, p2, config.winding_order(), config.cull_backfaces());
        if (bary.backface() && config.cull_backfaces()) return;
        if (!bary.valid()) return;

        glm::vec3 z_pack = { p0.z, p1.z, p2.z };
        glm::vec3 w_pack = { p0.w, p1.w, p2.w };

        // y-ordered
        if (p0.y > p1.y) std::swap(p0, p1);
        if (p1.y > p2.y) std::swap(p1, p2);
        if (p0.y > p1.y) std::swap(p0, p1);

        auto rasterize = [&color_buffer, &depth_buffer, &state, &bary, &z_pack, &w_pack](
                             f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 w0, f32 slope_xl,
                             f32 slope_xr, f32 slope_z0, f32 slope_z1, f32 slope_w0, f32 slope_w1
                         ) {
            glm::vec3 slope_b0 = bary.slope_x();
            glm::vec3 slope_b1 = bary.slope_y(slope_xl);
            glm::vec3 b0       = bary.coordinate(x0, y0);

            u64       row      = u64(y0) * color_buffer.width();
            u32       y_step   = 0;

            for (f32 y = std::trunc(y0); y < std::trunc(y1);
                 ++y, ++y_step, row += color_buffer.width()) {

                glm::vec3 b = b0;
                f32       w = w0;
                f32       z = z0;

                for (f32 x = std::trunc(slope_xl * y_step + x0);
                     x < std::trunc(slope_xr * y_step + x1); ++x) {

                    f32& depth = depth_buffer[row + u32(x)];
                    if (depth > z) {
                        depth               = z;

                        glm::vec3 barycoord = b * w_pack;
                        barycoord /= (barycoord.x + barycoord.y + barycoord.z);

                        color_buffer[row + u32(x)] = glm::packUnorm4x8(glm::vec4(1.f, barycoord));
                    }

                    b += slope_b0;
                    w += slope_w0;
                    z += slope_z0;
                }

                b0 += slope_b1;
                w0 += slope_w1;
                z0 += slope_z1;
            }
        };

        // flat top
        if (u32(p0.y) == u32(p1.y)) {
            auto [left, right] = std::minmax(p0, p1, vec_x_less);
            std::apply(rasterize, get_flat_top_slopes(left, right, p2));
        }

        // flat bottom
        else if (u32(p1.y) == u32(p2.y)) {
            auto [left, right] = std::minmax(p1, p2, vec_x_less);
            std::apply(rasterize, get_flat_bottom_slopes(p0, left, right));
        }

        // general
        else {
            f32       amount   = (p1.y - p0.y) / (p2.y - p0.y);
            glm::vec4 p3       = p0 + amount * (p2 - p0);

            auto [left, right] = std::minmax(p1, p3, vec_x_less);
            std::apply(rasterize, get_flat_bottom_slopes(p0, left, right));
            std::apply(rasterize, get_flat_top_slopes(left, right, p2));
        }
    }

    void
    render_debug_line_direct(FrameU32& color_buffer, u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {

        i32 dx = x1 - x0;
        if (dx == 0) { // vertical
            auto [miny, maxy] = std::minmax(y0, y1);
            u64 index         = miny * color_buffer.width() + x0;

            for (u64 y = miny; y < maxy; ++y) {
                color_buffer[index] = color;
                index += color_buffer.width();
            }
            return;
        }

        i32 dy = y1 - y0;
        if (dy == 0) { // horizontal
            u64 begin = y0 * color_buffer.width() + std::min(x0, x1);
            std::fill_n(color_buffer.data(), std::abs(dx), color);
            return;
        }

        if (std::abs(dx) > std::abs(dy)) { // gentle slope < 45deg
            if (x0 > x1) {
                std::swap(x0, x1);
                y0 = y1;
            }

            f32 slope = f32(dy) / f32(dx);
            f32 y     = y0;

            for (u32 x = x0; x < x1; ++x, y += slope)
                color_buffer[u32(y) * color_buffer.width() + x] = color;
        }

        else { // steep slope >= 45deg
            if (y0 > y1) {
                std::swap(y0, y1);
                x0 = x1;
            }

            f32 slope = f32(dx) / f32(dy);
            u64 row   = y0 * color_buffer.width();
            f32 x     = x0;

            for (u32 y = y0; y < y1; ++y, x += slope) {
                color_buffer[row + u32(x)] = color;
                row += color_buffer.width();
            }
        }
    }

} // namespace rohan
