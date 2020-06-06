#include "SaveLoad.h"

using namespace std;

const double PI = acos(-1);

#pragma once

class Bitmap {
public:
    /*********************************************************
    * 构造函数
    *********************************************************/
    Bitmap() = default;

    explicit Bitmap(BmpImage &bmpImage, int channels) {
        if (!bmpImage.getData().empty() && (channels == 1 || channels == 3)) {
            this->bmpImage = bmpImage;
            this->channels = channels;
            this->bmpData = bmpImage.getData();
        } else {
            exit(1);
        }
    }

    void GrayHistogramEqualization(string postFix);

    void ColorHistogramEqualization(string postFix);

    void doOperation();

private:
    BmpImage bmpImage;
    vector<vector<uchar>> bmpData;
    int channels;
};

/*********************************************************
* 直方图均衡
*********************************************************/
void Bitmap::doOperation() {
    if (channels == 1) {
        GrayHistogramEqualization("_GrayHistogramEqualization.bmp");
    } else if (channels == 3) {
        ColorHistogramEqualization("_ColorHistogramEqualization.bmp");
    }
}

//灰度图直方图均衡
void Bitmap::GrayHistogramEqualization(string postFix) {
    cout << "waiting..." << endl;
    const int L = 256; //像素值的取值范围为[0, L)
    int R[L]; //原图中各像素值出现的次数的前缀和
    int S[L]; //直方图均衡后像素的对应值，原像素值为i的值对应到S[i]

    //统计每个像素值出现次数
    for (int &rl : R) {
        rl = 0;
    }

    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            int pix = (int) (bmpData[i][j]);
            R[pix]++;
        }
    }

    //维护R数组的 累计 求和结果， 后面用于概率的转化
    for (int i = 1; i < L; ++i) {
        R[i] += R[i - 1];
    }

    //计算S数组   R[i]/R[L-1]就是概率的求和
    for (int i = 0; i < L; ++i) {
        S[i] = (int) (1.0 * (L - 1) * R[i] / R[L - 1]); // 直方图均衡方程
    }


    //对原图进行直方图均衡
    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            int pix = (int) (bmpData[i][j]);
            pix = S[pix];
            bmpData[i][j] = (uchar) pix;
        }
    }
    bmpImage.setData(bmpData);
    cout << "operation success!" << endl;
    bmpImage.SaveImage(postFix);
}

//彩色图直方图均衡
void Bitmap::ColorHistogramEqualization(string postFix) {
    cout << "waiting..." << endl;
    const int L = 256; //像素值的取值范围为[0, L)
    int R[L]; //原图中各像素值出现的次数的前缀和
    int S[L]; //直方图均衡后像素的对应值，原像素值为i的值对应到S[i]

    //统计每个像素值出现次数
    for (int &rl : R) {
        rl = 0;
    }
    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            //RGB通道转为HSI通道的I分量（亮度）值
            int b = (int) (bmpData[i][j * 3]);
            int g = (int) (bmpData[i][j * 3 + 1]);
            int r = (int) (bmpData[i][j * 3 + 2]);
            int I = (r + g + b) / 3;
            R[I]++;
        }
    }
    //维护R数组的 累计 求和结果， 后面用于概率的转化
    for (int i = 1; i < L; ++i) {
        R[i] += R[i - 1];
    }

    //计算S数组  R[i]/R[L-1]就是概率的求和
    for (int i = 0; i < L; ++i) {
        S[i] = (int) (1.0 * (L - 1) * R[i] / R[L - 1]); // 对 I 分量进行 均衡
    }


    // 将均衡 后的 I 替换原来的I值 ，图片还原 为 RGB
    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            int b = (int) (bmpData[i][j * 3]);
            int g = (int) (bmpData[i][j * 3 + 1]);
            int r = (int) (bmpData[i][j * 3 + 2]);

            double theta = (r == g && g == b) ? 0 : acos(
                    0.5 * (r - g + r - b) / sqrt((r - g) * (r - g) + (r - b) * (g - b)));
            double I = S[(r + g + b) / 3];// I分量 替换为之前 进行 均衡的 I值  在 146行进行了均衡
            double H = b > g ? PI + PI - theta : theta;
            double S = 1 - 3.0 * min(r, min(g, b)) / (r + g + b);

            //用新的I值返回得到原RGB值
            if (H < PI * 2 / 3) {
                b = I * (1 - S);
                r = I * (1 + S * cos(H) / cos(PI / 3 - H));
                g = 3 * I - r - b;
            } else if (H < PI * 4 / 3) {
                H -= PI * 2 / 3;
                r = I * (1 - S);
                g = I * (1 + S * cos(H) / cos(PI / 3 - H));
                b = 3 * I - r - g;
            } else {
                H -= PI * 4 / 3;
                g = I * (1 - S);
                b = I * (1 + S * cos(H) / cos(PI / 3 - H));
                r = 3 * I - g - b;
            }

            r = min(255, r);
            g = min(255, g);
            b = min(255, b);

            bmpData[i][j * 3] = (uchar) b;
            bmpData[i][j * 3 + 1] = (uchar) g;
            bmpData[i][j * 3 + 2] = (uchar) r;
        }
    }
    bmpImage.setData(bmpData);
    cout << "operation success!" << endl;
    bmpImage.SaveImage(postFix);
}

