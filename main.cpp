#include <iostream>
#include <complex>
#include <cstdint>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    static constexpr int w{800}, h{800}, ch{3};
    unsigned char data[w * h * ch];
    uint32_t *idata;
    idata = new uint32_t[w * h]; // no ch here
    memset(data, 0, sizeof(data));
    memset(idata, 0, sizeof(idata));


    float dx = 2.f / w;
    float dy = 2.f / h;
    float odx = 1.0 / dx;
    float ody = 1.0 / dy;
    auto start = std::chrono::high_resolution_clock::now();

    // std::complex<float> c{3.69, 4.51};
    std::complex<float> c{3.64, 1.71};

    for (int i=0; i<w; ++i)
    {
        float x0 = dx * i - 1.f;

        for (int j=0; j<h; ++j)
        {
            float y0 = dy * j - 1.f;
            std::complex<float> z{x0, y0};

            for (int q=0; q<15; ++q) // clear initial transients
            {
                auto tm = z * z + c;
                z = {std::sin(tm.real()), std::cos(tm.imag()) };
            }
            for (int q=0; q<150; ++q)
            {
                auto tm = z * z + c;
                z = {std::sin(tm.real()), std::cos(tm.imag()) };

                auto px = (int)((z.real() + 1.f )* odx);
                auto py = (int)((z.imag() + 1.f )* ody);
                auto d = py + h * px;
                idata[d]++;
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed_us.count() << " us (" <<
        1.f * elapsed_us.count() / ( w * h) << " per pt)" << std::endl;

    for (int i=0; i<w; ++i)
    {
        for (int j=0; j<h; ++j)
        {
            auto id = j + h * i;
            auto d = id * ch;
            data[d] = 255 - std::min(idata[id] >> 1, 255u);
            data[d+1] = 255 - std::min(idata[id] >> 2, 255u);
            data[d+2] = 255 - std::min(idata[id] >> 3, 255u);
        }
    }

    delete[] idata;
    stbi_write_png("test.png", w, h, ch, data, w * ch);

    return 0;
}