#include "SaveLoad.h"
#include "ModelTrans.h"
#include "ImageCompress.h"

int main() {
    auto bmpImage = BmpImage(R"(C:\Users\Cirno\Desktop\lenna_gray.bmp)");
    int channels = bmpImage.getChannels();
    ModelTrans modelTrans;
    Bitmap bitmap;
    ImageCompress imageCompress;
    int choice = 0;
    cout << "Please choose an module to operate: (1) Mode Conversion (2) Image Enhancement (3) Image Compression"
         << endl;
    cin >> choice;
    while (choice < 1 || choice > 3) {
        cout << "Wrong input! Please type a num between 1-3. " << endl;
        cin >> choice;
    }
    switch (choice) {
        case 1:
            modelTrans = ModelTrans(bmpImage, channels);
            modelTrans.doOperation();
            break;
        case 2:
            bitmap = Bitmap(bmpImage, channels);
            bitmap.doOperation();
            break;
        case 3:
            if (channels == 3) {
                cout << "This image can not be compressed!" << endl;
                break;
            }
            imageCompress = ImageCompress(bmpImage, channels);
            imageCompress.doOperation();
            break;
        default:
            return 0;
    }
    return 0;
}
