// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <windows.h>

#include "../App/Pixel.cpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    return TRUE;
}
void changeSaturation(Pixel* pixelsBuffer, int bufferSize, float saturationLvl)
{
    // increasing the saturation
    if (saturationLvl > 0)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            // colorFactor = 0 - no color at all, gray scale pixel; colorFactor = 1 - max saturation
            float colorFactor = (float)pixelsBuffer[i].getS() / 255.0;
            // max saturation value is 255
            pixelsBuffer[i].setS(pixelsBuffer[i].getS() + (255 - pixelsBuffer[i].getS()) * saturationLvl * colorFactor);
        }
    }
    // decreasing the saturation
    else if (saturationLvl < 0)
    {
        for (int i = 0; i < bufferSize; ++i)
        {
            // min saturation value is 0
            pixelsBuffer[i].setS(pixelsBuffer[i].getS() + pixelsBuffer[i].getS() * saturationLvl);
        }
    }
}