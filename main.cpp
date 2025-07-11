#include <iostream>
#include <complex>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    static constexpr int w{500}, h{500}, ch{3};
    unsigned char data[w * h * ch];
    memset(data, 0, sizeof(data));


    float dx = 2.f / w;
    float dy = 2.f / h;
    for (int i=0; i<w; ++i)
    {
        for (int j=0; j<h; ++j)
        {
            float x0 = dx * i - 1.f;
            float y0 = dy * j - 1.f;
            std::complex<float> z{x0, y0};
            // std::complex<float> c{3.69, 4.51};
            std::complex<float> c{3.64, 1.71};

            for (int q=0; q<250; ++q)
            {
                auto tm = z * z + c;
                z = {std::sin(tm.real()), std::cos(tm.imag()) };
                auto px = std::clamp((int)((z.real() + 1.f )/ dx), 0, w-1);
                auto py = std::clamp((int)((z.imag() + 1.f )/ dx), 0, h-1);;
                auto d = ch * (py + h * px);
                if (q > 75)
                {
                    data[d] = std::min(data[d] + 1, 255);
                    if (q % 5 == 0)
                        data[d+1] = std::min(data[d+1]+ 1, 255);
                    if (q % 10 == 0)
                        data[d+2] = std::min(data[d+2] + 1, 255);
                }
            }

        }
    }

    stbi_write_png("test.png", w, h, ch, data, w * ch);

    return 0;
}