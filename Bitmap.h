#include "SaveLoad.h"

using namespace std;

const double PI = acos(-1);

#pragma once

class Bitmap {
public:
    /*********************************************************
    * ���캯��
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
* ֱ��ͼ����
*********************************************************/
void Bitmap::doOperation() {
    if (channels == 1) {
        GrayHistogramEqualization("_GrayHistogramEqualization.bmp");
    } else if (channels == 3) {
        ColorHistogramEqualization("_ColorHistogramEqualization.bmp");
    }
}

//�Ҷ�ͼֱ��ͼ����
void Bitmap::GrayHistogramEqualization(string postFix) {
    cout << "waiting..." << endl;
    const int L = 256; //����ֵ��ȡֵ��ΧΪ[0, L)
    int R[L]; //ԭͼ�и�����ֵ���ֵĴ�����ǰ׺��
    int S[L]; //ֱ��ͼ��������صĶ�Ӧֵ��ԭ����ֵΪi��ֵ��Ӧ��S[i]

    //ͳ��ÿ������ֵ���ִ���
    for (int &rl : R) {
        rl = 0;
    }

    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            int pix = (int) (bmpData[i][j]);
            R[pix]++;
        }
    }

    //ά��R����� �ۼ� ��ͽ���� �������ڸ��ʵ�ת��
    for (int i = 1; i < L; ++i) {
        R[i] += R[i - 1];
    }

    //����S����   R[i]/R[L-1]���Ǹ��ʵ����
    for (int i = 0; i < L; ++i) {
        S[i] = (int) (1.0 * (L - 1) * R[i] / R[L - 1]); // ֱ��ͼ���ⷽ��
    }


    //��ԭͼ����ֱ��ͼ����
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

//��ɫͼֱ��ͼ����
void Bitmap::ColorHistogramEqualization(string postFix) {
    cout << "waiting..." << endl;
    const int L = 256; //����ֵ��ȡֵ��ΧΪ[0, L)
    int R[L]; //ԭͼ�и�����ֵ���ֵĴ�����ǰ׺��
    int S[L]; //ֱ��ͼ��������صĶ�Ӧֵ��ԭ����ֵΪi��ֵ��Ӧ��S[i]

    //ͳ��ÿ������ֵ���ִ���
    for (int &rl : R) {
        rl = 0;
    }
    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            //RGBͨ��תΪHSIͨ����I���������ȣ�ֵ
            int b = (int) (bmpData[i][j * 3]);
            int g = (int) (bmpData[i][j * 3 + 1]);
            int r = (int) (bmpData[i][j * 3 + 2]);
            int I = (r + g + b) / 3;
            R[I]++;
        }
    }
    //ά��R����� �ۼ� ��ͽ���� �������ڸ��ʵ�ת��
    for (int i = 1; i < L; ++i) {
        R[i] += R[i - 1];
    }

    //����S����  R[i]/R[L-1]���Ǹ��ʵ����
    for (int i = 0; i < L; ++i) {
        S[i] = (int) (1.0 * (L - 1) * R[i] / R[L - 1]); // �� I �������� ����
    }


    // ������ ��� I �滻ԭ����Iֵ ��ͼƬ��ԭ Ϊ RGB
    for (int i = 0; i < bmpImage.getHeight(); ++i) {
        for (int j = 0; j < bmpImage.getWidth(); ++j) {
            int b = (int) (bmpData[i][j * 3]);
            int g = (int) (bmpData[i][j * 3 + 1]);
            int r = (int) (bmpData[i][j * 3 + 2]);

            double theta = (r == g && g == b) ? 0 : acos(
                    0.5 * (r - g + r - b) / sqrt((r - g) * (r - g) + (r - b) * (g - b)));
            double I = S[(r + g + b) / 3];// I���� �滻Ϊ֮ǰ ���� ����� Iֵ  �� 146�н����˾���
            double H = b > g ? PI + PI - theta : theta;
            double S = 1 - 3.0 * min(r, min(g, b)) / (r + g + b);

            //���µ�Iֵ���صõ�ԭRGBֵ
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

