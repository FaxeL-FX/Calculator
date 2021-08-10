#pragma once
#include <string>

class BMPWriter
{
public:
    static void write_image(float* image, int height, int width, char* file_name);
    static float max_float;
};