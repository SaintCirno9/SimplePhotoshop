#include "SaveLoad.h"
#include "ModelTrans.h"

int main() {
    auto *bmpImage = new BmpImage(R"(D:\Download\test-images\child_gray.bmp)");
    auto modelTrans = ModelTrans(bmpImage, bmpImage->getChannels());
    modelTrans.doOperation();

    //bmpImage.SaveImage();
    return 0;
}
