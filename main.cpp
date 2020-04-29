#include "SaveLoad.h"

int main() {
    BmpImage bmpImage;
    bmpImage.LoadImage(R"(C:\Users\Cirno\Desktop\lenna_gray.bmp)");
    bmpImage.ShowBMPInfo();
    bmpImage.SaveImage();
    return 0;
}
