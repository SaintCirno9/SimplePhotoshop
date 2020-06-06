#pragma once

#include "SaveLoad.h"


using namespace std;

class ModelTrans {
public:
    ModelTrans() = default;

    explicit ModelTrans(BmpImage &bmpImage, int channels) {
        if (!bmpImage.getData().empty() && (channels == 1 || channels == 3)) {
            this->bmpImage = bmpImage;
            this->channels = channels;
            this->bmpData = bmpImage.getData();
        } else {
            exit(1);
        }
    }

    void doOperation();

private:
    BmpImage bmpImage;
    vector<vector<uchar>> bmpData;
    int channels;

    bool singleThreshold(string postFix);

    bool dithering(string postFix);

    bool orderedDithering(string postFix);

    void getDitheringMatrix(int n, vector<vector<uchar>> &D);


    void RGBtoHSI(string postFix);

    void pixelRGBtoHSI(uchar R, uchar G, uchar B, uchar &H, uchar &S, uchar &I);

    void RGBtoYCbCr(string postFix);

    static void pixelRGBtoYCbCr(uchar R, uchar G, uchar B, uchar &Y, uchar &Cb, uchar &Cr);

};

void ModelTrans::doOperation() {
    if (channels == 1) {
        int choose = 0;
        cout << "Please choose an operation:\n1.single threshold\n2.dithering\n3.ordered dithering\n4.exit\n";
        cin >> choose;
        switch (choose) {
            case 1:
                singleThreshold("_SingleThreshold.bmp");
                break;
            case 2:
                dithering("_Dithering.bmp");
                break;
            case 3:
                orderedDithering("_OrderedDithering.bmp");
                break;
            case 4:
                exit(1);
            default:
                cout << "wrong input\n";
        }
    } else if (channels == 3) {
        int choose = 0;
        cout << "Please choose the model you want to transfer:\n1.RGB-->HSI\n2.RGB-->YCbCr\n";
        cin >> choose;
        switch (choose) {
            case 1:
                RGBtoHSI("_RGBtoHSI.bmp");
                break;
            case 2:
                RGBtoYCbCr("_RGBtoYCbCr.bmp");
                break;
            default:
                cout << "wrong input!" << endl;
        }

    } else {
        cout << "1st Image Error!" << endl;
    }
}

bool ModelTrans::singleThreshold(string postFix) {
    bool TransDone = false;
    int thresholdInt = 0;
    uchar threshold;
    cout << "Please input the threshold(1--255):\n";
    //cout << bmpImage.data.size();
    while (true) {
        cin >> thresholdInt;
        if (thresholdInt < 1 || thresholdInt > 255) {
            cout << "Please input the right threshold!" << endl;
        } else {
            cout << "waiting..." << endl;
            threshold = static_cast<unsigned char>(thresholdInt);
            //vector<uchar> dataValues;
            for (size_t i = 0; i <= bmpData.size() - 1; i++) {
                //cout << "i=" << i << "\n";
                for (size_t j = 0; j <= bmpData[i].size() - 1; j++) {
                    //cout << "j=" << j << "\n";
                    if (bmpData[i][j] > threshold) {
                        bmpData[i][j] = 0xff;
                    } else {
                        bmpData[i][j] = 0x00;
                    }
                }
            }
            //cout << "something wrong?";
            //system("PAUSE");
            bmpImage.setData(bmpData);
            cout << "operation success!" << endl;
            bmpImage.SaveImage(postFix);
            TransDone = true;
            break;
        }
    }
    return TransDone;
}

bool ModelTrans::dithering(string postFix) {
    bool transDone = false;
    vector<vector<uchar>> D;//dither matrix
    vector<vector<uchar>> I;//matrix after divided.
//	vector<vector<uchar>> O;//output matrix
    float input;
    float check;
    int n;
    cout << "Please input the dithering matrix:\n(for example:2)\n";
    while (true) {
        cin >> input;
        n = input;
        check = input - n;
        if (n & (n - 1) || check != 0) {//if n is not the index times of 2 or not a integer
            cout << "please input the index times of 2!(for example:4)" << endl;
        } else {
            cout << "waiting..." << endl;
            break;
        }
    }

    float T = round(256.0f / ((float) n * (float) n + 1.0f));
    for (size_t i = 0; i <= bmpData.size() - 1; i++) {
        vector<uchar> tempi;
        for (size_t j = 0; j <= bmpData[i].size() - 1; j++) {
            float temp = 0.0f;
            temp = (float) bmpData[i][j] / T;
            tempi.push_back((uchar) round(temp));
        }
        I.push_back(tempi);
    }
    //cout << "end first" << endl;
    getDitheringMatrix(n, D);
    bmpData.clear();
    for (size_t x = 0; x < bmpImage.getHeight() * n; x++) {//Initial the Output Matrix
        vector<uchar> temp;
        for (size_t y = 0; y < bmpImage.getHeight() * n; y++) {
            temp.push_back(0xff);
        }
        bmpData.push_back(temp);
    }
    //cout << "end first" << endl;
    for (size_t x = 0; x < bmpImage.getHeight(); x++) {
        for (size_t y = 0; y < bmpImage.getWidth(); y++) {
            int i = x * n;
            int j = y * n;
            for (size_t ii = 0; ii < n; ii++) {
                for (size_t jj = 0; jj < n; jj++) {
                    //cout << "height = "<< ii + i << endl;
                    //cout << "width = " << jj + j << endl;
                    if (D[ii][jj] < I[x][y]) {
                        bmpData[i + ii][j + jj] = 0xff;
                    } else {
                        bmpData[i + ii][j + jj] = 0x00;
                    }
                }
            }
        }
    }
    bmpImage.setHeight(bmpImage.getHeight() * n);
    bmpImage.setWidth(bmpImage.getWidth() * n);
    BmpFileHeader bmpFileHeader = bmpImage.getBmpFileHeader();
    bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpImage.getWidth() * bmpImage.getHeight();
    bmpImage.setBmpFileHeader(bmpFileHeader);

    bmpImage.setData(bmpData);
    cout << "operation success!" << endl;
    bmpImage.SaveImage(postFix);
    transDone = true;
    return transDone;
}

bool ModelTrans::orderedDithering(string postFix) {
    bool transDone = false;
    vector<vector<uchar>> D;//dither matrix
    vector<vector<uchar>> I;//matrix after divided.

    float input;
    float check;
    int n;
    cout << "Please input the dithering matrix:\n(for example:2)\n";
    while (true) {
        cin >> input;
        n = input;
        check = input - n;
        if (n & (n - 1) || check != 0) {//if n is not the index times of 2
            cout << "please input the index times of 2!(for example:4)" << endl;
        } else {
            cout << "waiting..." << endl;
            break;
        }
    }

    float T = round(256.0f / ((float) n * (float) n + 1.0f));
    for (size_t i = 0; i <= bmpData.size() - 1; i++) {
        vector<uchar> tempi;
        for (size_t j = 0; j <= bmpData[i].size() - 1; j++) {
            float temp = 0.0f;
            temp = (float) bmpData[i][j] / T;
            tempi.push_back((uchar) round(temp));
        }
        I.push_back(tempi);
    }
    //cout << "end first" << endl;
    getDitheringMatrix(n, D);
    for (size_t x = 0; x < bmpImage.getHeight(); x++) {
        int i = x % n;
        for (size_t y = 0; y < bmpImage.getWidth(); y++) {
            int j = y % n;
            if (I[x][y] > D[i][j]) {
                bmpData[x][y] = 0xff;
            } else {
                bmpData[x][y] = 0x00;
            }
        }
    }
    bmpImage.setData(bmpData);
    cout << "operation success!" << endl;
    bmpImage.SaveImage(postFix);
    transDone = true;
    return transDone;
}


void ModelTrans::getDitheringMatrix(int n, vector<vector<uchar>> &D) {
    D = {{(uchar) 0, (uchar) 2},
         {(uchar) 3, (uchar) 1}};

    int length = 2;
    vector<vector<uchar>> tempD;
    while (length < n) {
        for (size_t x = 0; x < length; x++) {
            vector<uchar> temp;
            for (size_t y = 0; y < length; y++) {
                temp.push_back(D[x][y]);
            }
            tempD.push_back(temp);
        }
        D.clear();
        int x = 0;
        int y = 0;
        for (x = 0; x < length; x++) {
            vector<uchar> temp;
            for (y = 0; y < length; y++) {
                temp.push_back((uchar) (4 * (int) tempD[x][y]));
            }
            for (y = length; y < 2 * length; y++) {
                temp.push_back((uchar) (4 * (int) tempD[x][y - length] + 2));
            }
            D.push_back(temp);
        }
        for (x = length; x < 2 * length; x++) {
            vector<uchar> temp;
            for (y = 0; y < length; y++) {
                temp.push_back((uchar) (4 * (int) tempD[x - length][y] + 3));
            }
            for (y = length; y < 2 * length; y++) {
                temp.push_back((uchar) (4 * (int) tempD[x - length][y - length] + 1));
            }
            D.push_back(temp);
        }
        tempD.clear();

        length = length << 1;
    }
}

void ModelTrans::RGBtoHSI(string postFix) {
    size_t step = 3;
    for (size_t i = 0; i <= bmpData.size() - 1; i++) {
        //cout << i << endl;
        for (size_t j = 0; j <= bmpData[i].size() - 3; j += step) {
            //	cout << j << endl;
            pixelRGBtoHSI(bmpData[i][j], bmpData[i][j + 1], bmpData[i][j + 2], bmpData[i][j],
                          bmpData[i][j + 1], bmpData[i][j + 2]);
        }
    }

    bmpImage.setData(bmpData);
    bmpImage.SaveImage();
}

void ModelTrans::pixelRGBtoHSI(uchar R, uchar G, uchar B, uchar &H, uchar &S, uchar &I) {
    const uchar RGBMAX = 255;
    const float PI2 = 2 * 3.1415926;
    float r = float(R) / RGBMAX;
    float g = float(G) / RGBMAX;
    float b = float(B) / RGBMAX;
    float maxVal = max(max(r, g), b);
    float minVal = min(min(r, g), b);

    float i = (r + g + b) / 3;
    float s = 0.0f;
    float h = 0.0f;
    if (i != 0.0f && (maxVal - minVal) != 0.0f) {
        float diff = 0.5f * (r - g + r - b);
        float diff2 = (r - g) * (r - g) + (r - b) * (g - b);
        float sita = acos(diff / sqrt(diff2)) / PI2;
        h = (g >= b) ? sita : 1.0f - sita;
        s = 1.0f - minVal / i;
    }
    if (h < 0.0f) {
        h += 1.0f;
    } else if (h > 1.0f) {
        h -= 1.0f;
    }
    H = (uchar) round(h * RGBMAX);    //range: 0 to 255;
    S = (uchar) round(s * RGBMAX);     //range: 0 to 255;
    I = (uchar) round(i * RGBMAX);     //range:  0 to 255;

}

void ModelTrans::RGBtoYCbCr(string postFix) {
    size_t step = 3;
    for (size_t i = 0; i <= bmpData.size() - 1; i++) {
        //cout << i << endl;
        for (size_t j = 0; j <= bmpData[i].size() - 3; j += step) {
            //	cout << j << endl;
            pixelRGBtoYCbCr(bmpData[i][j], bmpData[i][j + 1], bmpData[i][j + 2], bmpData[i][j],
                            bmpData[i][j + 1], bmpData[i][j + 2]);
        }
    }
    bmpImage.setData(bmpData);
    bmpImage.SaveImage(postFix);
}

void ModelTrans::pixelRGBtoYCbCr(uchar R, uchar G, uchar B, uchar &Y, uchar &Cb, uchar &Cr) {
    auto r = float(R);
    auto g = float(G);
    auto b = float(B);
    Y = (uchar) round(0.299 * r + 0.587 * g + 0.114 * b);
    Cb = (uchar) round(-0.1687 * r - 0.3313 * g + 0.5 * b + 128);
    Cr = (uchar) round(0.5 * r - 0.4187 * g - 0.0813 * b + 128);

}

