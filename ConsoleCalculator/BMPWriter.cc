#include "BMPWriter.h"
#include <Windows.h>
#include <string>

float BMPWriter::max_float = 1;

void BMPWriter::write_image(float* red, float* green, float* blue, int height, int width, char* file_name)
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



    for(int y = height - 1; y >= 0; y--)
        for (int x = 0; x < width; x++)
        {
            int offset = (y * width + x);
            int val_red = (int)(*(red + offset) * ((float)255 / max_float));
            int val_green = (int)(*(green + offset) * ((float)255 / max_float));
            int val_blue = (int)(*(blue + offset) * ((float)255 / max_float));
            file_cnt.append(std::to_string(val_red) + " " + std::to_string(val_green) + " " + std::to_string(val_blue) + "\n");
        }


    DWORD written = 0;
    char* ptr = (char*)file_cnt.c_str();
    WriteFile(file,ptr,file_cnt.size(),&written,NULL);
    CloseHandle(file);
}
