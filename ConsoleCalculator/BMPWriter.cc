#include "BMPWriter.h"
#include <Windows.h>
#include <string>

float BMPWriter::max_float = 1;

void BMPWriter::write_image(float* image, int height, int width, char* file_name)
{
    HANDLE file = CreateFileA
    (
            file_name,
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
    );

    std::string file_cnt;

    file_cnt.append("P3\n");
    file_cnt.append(std::to_string(width) + " " + std::to_string(height) + "\n");
    file_cnt.append("255\n");



    for(int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            int offset = (y * width + x);
            int val = (int)(*(image + offset) * ((float)255 / max_float));
            std::string gray_scale = std::to_string(val);
            file_cnt.append(gray_scale + " " + gray_scale + " " + gray_scale + "\n");
        }


    DWORD written = 0;
    char* ptr = (char*)file_cnt.c_str();
    WriteFile(file,ptr,file_cnt.size(),&written,NULL);
    CloseHandle(file);
}
