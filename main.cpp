#include <iostream>
#include <complex>
#include <cstdint>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <__filesystem/directory_iterator.h>

int main()
{
    static constexpr int w{500}, h{500}, ch{3};
    static constexpr int warmup{15}, iterations{150};
    unsigned char data[w * h * ch];
    uint32_t *idata;
    idata = new uint32_t[w * h]; // no ch here

    float dx = 2.f / w;
    float dy = 2.f / h;
    float odx = 1.0 / dx;
    float ody = 1.0 / dy;

    // std::complex<float> c{3.69, 4.51};
    // std::complex<float> c{3.64, 1.71};
    std::complex<float> startC{3.89, 4.51};
    std::complex<float> endC{3.2, 5.23};
    static constexpr int frames{1};
    static constexpr float dframe{1.f / std::max((frames - 1), 1)};
    int ffnstart{60};

    for (int fr = 0; fr < frames; ++fr)
    {
        std::cout << "Generating frame " << fr << std::endl;
        memset(data, 255, sizeof(data));
        memset(idata, 0, sizeof(uint32_t) * w * h);
        auto frac = fr * dframe;
        auto omfrac = 1.0f - frac;
        std::complex<float> c{omfrac * startC.real() + frac * endC.real(), omfrac * startC.imag() + frac * endC.imag()};
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < w; ++i)
        {
            float x0 = dx * i - 1.f;

            for (int j = 0; j < h; ++j)
            {
                float y0 = dy * j - 1.f;
                std::complex<float> z{x0, y0};

                for (int q = 0; q < warmup; ++q) // clear initial transients
                {
                    auto tm = z * z + c;
                    z = {std::sin(tm.real()), std::cos(tm.imag())};
                }
                for (int q = 0; q < iterations; ++q)
                {
                    auto tm = z * z + c;
                    z = {std::sin(tm.real()), std::cos(tm.imag())};

                    auto px = (int)((z.real() + 1.f) * odx);
                    auto py = (int)((z.imag() + 1.f) * ody);
                    auto d = py + h * px;
                    idata[d]++;
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Elapsed time: " << elapsed_us.count() << " us ("
                  << 1.f * elapsed_us.count() / (w * h) << " per pt)" << std::endl;

        for (int i = 0; i < w; ++i)
        {
            for (int j = 0; j < h; ++j)
            {
                auto id = j + h * i;
                auto d = id * ch;
                data[d] = 255 - std::min(idata[id] >> 1, 255u);
                data[d + 1] = 255 - std::min(idata[id] >> 2, 255u);
                data[d + 2] = 255 - std::min(idata[id] >> 3, 255u);
            }
        }

        char name[256];
        snprintf(name , 256, "test_%03d.png", fr + ffnstart);
        stbi_write_png(name, w, h, ch, data, w * ch);
    }

    delete[] idata;
    return 0;
}
