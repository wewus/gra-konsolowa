#include <iostream>
#include <fstream>
#include <cmath>

int main() {
    const int width = 512;
    const int height = 512;

    std::ofstream fout("output.ppm", std::ios::binary);
    if (!fout) {
        std::cerr << "Unable to create image file\n";
        return 1;
    }

    fout << "P6\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char r = static_cast<unsigned char>((x * 255) / (width - 1));
            unsigned char g = static_cast<unsigned char>((y * 255) / (height - 1));
            unsigned char b = static_cast<unsigned char>(128 + 127 * std::sin(x * 0.1) * std::cos(y * 0.1));
            fout << r << g << b;
        }
    }

    fout.close();
    std::cout << "Image saved as output.ppm\n";
    return 0;
}