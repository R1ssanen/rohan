#ifndef ROHAN_BUFFER_HPP_
#define ROHAN_BUFFER_HPP_

#include <cassert>
#include <immintrin.h>
#include <iostream>

#include "rhdefs.hpp"

namespace rohan {

    template <typename T>
        requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    class Buffer {
      public:

        Buffer() = delete;

        ~Buffer() {
            if (!m_data) return;
            else if (m_owning)
                _mm_free(m_data); // free(m_data);
        }

        Buffer(u32 width, u32 height, T* ptr)
            : m_count(width * height), m_width(width), m_height(height), m_pitch(width * stride()) {
            assert(ptr != nullptr);
            m_data   = ptr;
            m_owning = false;
        }

        Buffer(u32 width, u32 height)
            : m_count(width * height), m_width(width), m_height(height), m_pitch(width * stride()) {
            void* allocated = _mm_malloc(m_count * stride(), 32);
            m_data          = reinterpret_cast<T*>(allocated);
            m_owning        = true;
        }

        T* data() noexcept { return m_data; }

        T& operator[](u64 i) noexcept { return m_data[i]; }

        // IMPORTANT NOTE: remember to reallocate the
        // data buffer in case it's not owned by the buffer
        void resize(u32 width, u32 height) {
            m_width  = width;
            m_height = height;
            m_count  = width * height;
            m_pitch  = width * stride();

            if (m_owning) {
                void* resized = realloc(m_data, m_count * stride());
                m_data        = reinterpret_cast<T*>(resized);
            }
        }

        u64           count() const noexcept { return m_count; }

        u32           width() const noexcept { return m_width; }

        u32           height() const noexcept { return m_height; }

        u32           pitch() const noexcept { return m_pitch; }

        bool          owning() const noexcept { return m_owning; }

        constexpr u32 stride() const noexcept { return sizeof(T); }

      protected:

        T*   m_data   = nullptr;
        u64  m_count  = 0;
        u32  m_width  = 0;
        u32  m_height = 0;
        u32  m_pitch  = 0;
        bool m_owning = false;
    };

} // namespace rohan

namespace rohan {

    template <typename T>
    inline std::ostream& operator<<(std::ostream& stream, const Buffer<T>& resource) {
        stream << std::boolalpha;
        stream << "[rohan::Buffer]" << '\n'
               << '\t' << "data: " << resource.data() << '\n'
               << '\t' << "count: " << resource.count() << '\n'
               << '\t' << "width: " << resource.width() << '\n'
               << '\t' << "height: " << resource.height() << '\n'
               << '\t' << "pitch: " << resource.pitch() << '\n'
               << '\t' << "stride: " << resource.stride() << '\n'
               << '\t' << "owned: " << resource.owning();

        return stream;
    }

} // namespace rohan

#endif
