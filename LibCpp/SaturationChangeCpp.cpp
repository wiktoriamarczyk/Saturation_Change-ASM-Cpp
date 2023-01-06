// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>

#include "../App/Pixel.cpp"

/*
 Project:   The application responsible for changing the color saturation in images.
            The program uses the stb_image library to load and save images and the Qt library to implement the user interface.

 Algorithm: The algorithm is based on the HSV color model. For each pixel of the loaded image, the saturation is changed by adding to saturation parameter its value multiplied
            by a given factor which is calculated based on the slider value representing the percentage of change. Slider value is then clamped to the range of 0 to 1
            (for increasing the saturation) or to the range of -1 to 0 if a decrease in saturation was selected. The new pixel saturation value can be a maximum of 255 and a minimum of 0.
            Modified pixels by the given saturation value are then converted to the RGB model and saved to a new image.

 Author:     Wiktoria Marczyk
 Version:    1.0

 sem. 5, 2022/2023
*/


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}

/*
 Changes the saturation of the given pixels array by the given saturation value.

 @param pixelsBuffer - array of pixels to be modified
 @param bufferSize - size of the pixels array
 @param saturationLvl - saturation value to be applied to the pixels: (-1;0) for decreasing the saturation, (0;1) for increasing the saturation
*/
void changeSaturation(hsv* pixelsBuffer, int bufferSize, float saturationLvl)
{
    // increasing the saturation
    if (saturationLvl > 0)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            // pixels that are close to the grayscale colors have a lower saturation value than the pixels that are close to the pure colors (red, green, blue)
            // and saturating those would result in artifacts
            // colorFactor = 0 - no color at all - gray scale pixel;
            // colorFactor = 1 - max saturation
            float colorFactor = (float)pixelsBuffer[i].s / 255.0;
            // max saturation value is 255
            pixelsBuffer[i].s = pixelsBuffer[i].s + (255 - pixelsBuffer[i].s) * saturationLvl * colorFactor;
        }
    }
    // decreasing the saturation
    else if (saturationLvl < 0)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            // min saturation value is 0
            pixelsBuffer[i].s = pixelsBuffer[i].s + pixelsBuffer[i].s * saturationLvl;
        }
    }
}