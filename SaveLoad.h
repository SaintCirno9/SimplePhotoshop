//
// Created by Cirno on 2020/4/28.
//

#ifndef SIMPLEPHOTOSHOP_SAVELOAD_H
#define SIMPLEPHOTOSHOP_SAVELOAD_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;

#pragma pack(1)
struct BmpFileHeader //�ļ�ͷ, 14�ֽ�
{
    ushort bfType;  //2�ֽڣ��ļ����ͣ���ʶ���ļ�Ϊbmp�ļ�,�ж��ļ��Ƿ�Ϊbmp�ļ������ø�ֵ��"0x4d42"�Ƚ��Ƿ���ȼ��ɣ�0x4d42 = 19778
    ulong bfSize;   //4�ֽڣ��ļ���С
    ushort bfReserved1; //2�ֽڣ�Ԥ����λ,��������Ϊ0
    ushort bfReserved2; //2�ֽڣ�Ԥ����λ,��������Ϊ0
    ulong bfOffBits;    //4�ֽڣ���ͷ��λͼ���ݵ�ƫ�ƣ���ͼ������������ʼλ��
};

struct BmpInfoHeader //��Ϣͷ, 40�ֽ�
{
    ulong biSize;    //4�ֽڣ���Ϣͷ�Ĵ�С����40
    long biWidth;    //4�ֽڣ�������Ϊ��λ˵��ͼ��Ŀ��
    long biHeight;    //4�ֽڣ�������Ϊ��λ˵��ͼ��ĸ߶ȣ�ͬʱ���Ϊ����˵��λͼ�����������ݱ�ʾ��ͼ������½ǵ����Ͻǣ������Ϊ��˵������
    ushort biPlanes;    //2�ֽڣ�ΪĿ���豸˵����ɫƽ�������ܱ�����Ϊ1
    ushort biBitCount;  //2�ֽڣ�˵��������/��������8-�Ҷ�ͼ��24-���ɫ
    ulong biCompression;    //4�ֽڣ�˵��ͼ���ѹ�����ͣ���õľ���0��BI_RGB������ʾ��ѹ��
    ulong biSizeImage;      //4�ֽڣ�˵��λͼ���ݵĴ�С������BI_RGB��ʽʱ����������Ϊ0
    long biXPelsPerMeter;  //��ʾˮƽ�ֱ��ʣ���λ������/�ף��з�������
    long biYPelsPerMeter;  //��ʾ��ֱ�ֱ��ʣ���λ������/�ף��з�������
    ulong biClrUsed;    //˵��λͼʹ�õĵ�ɫ���е���ɫ��������Ϊ0˵��ʹ������
    ulong biClrImportant;  //˵����ͼ����ʾ����ҪӰ�����ɫ��������Ϊ0˵������Ҫ
};

struct Palette //��ɫ��
{
    uchar b;
    uchar g;
    uchar r;
    uchar alpha; //Ԥ����λ
};

class BmpImage {
public:
    BmpImage() = default;

    int getHeight() const {
        return height;
    }

    int getWidth() const {
        return width;
    }

    int getChannels() const {
        return channels;
    }

    bool is_loaded() const {
        return loaded;
    }

    bool LoadImage(const string &path);

    bool SaveImage(const string &path);

    bool SaveImage() {
        string path = basePath.substr(0, basePath.find(".bmp"));
        path += "_save.bmp";
        return SaveImage(path);

    }

    void ShowBMPInfo();

private:
    string basePath;
    bool loaded = false;
    BmpFileHeader bmpFileHeader{};
    BmpInfoHeader bmpInfoHeader{};
    int width; //����
    int height; //���
    int channels; //ͨ����
    vector<vector<uchar>> data; //ͼƬ����
    vector<Palette> palette; //��ɫ������
};

bool BmpImage::LoadImage(const string &path) {
    basePath = path;
    ifstream fileReader(path, ios::binary);
    if (!fileReader.is_open()) {
        cerr << "Reader: Failed to open the file" << endl;
        return false;
    }
    fileReader.read(reinterpret_cast<char *>(&bmpFileHeader), sizeof(bmpFileHeader));
    if (bmpFileHeader.bfType != 0x4D42) {
        cerr << "Not Bmp file" << endl;
        return false;
    }
    fileReader.read(reinterpret_cast<char *>(&bmpInfoHeader), sizeof(bmpInfoHeader));

    //cout << bmpInfoHeader.biBitCount << " bit " << "image" << endl;
    //cout << "data offset: " << bmpFileHeader.bfOffBits << endl;
    int dataOffset = bmpFileHeader.bfOffBits;
    channels = bmpInfoHeader.biBitCount / 8;
    if (channels != 1 && channels != 3) {
        cerr << "Not Supported Bmp file" << endl;
        return false;
    }
    width = bmpInfoHeader.biWidth;
    height = bmpInfoHeader.biHeight;
    if (channels == 1 && dataOffset == 1078) {
        while (fileReader.tellg() < dataOffset) {
            Palette temp{};
            fileReader.read(reinterpret_cast<char *>(&temp), sizeof(temp));
            palette.push_back(temp);
        }
    }
    int widthBytes = width * channels;
    int offset = widthBytes % 4;
    if (offset != 0) {
        offset = 4 - offset;
        widthBytes += offset;
    }
    for (int i = 0; i < height; ++i) {
        vector<uchar> dataValues;
        for (int j = 0; j < widthBytes; ++j) {
            uchar dataValue;
            fileReader.read(reinterpret_cast<char *>(&dataValue), sizeof(dataValue));
            dataValues.push_back(dataValue);
        }
        for (int k = 0; k < offset; ++k) {
            dataValues.pop_back();
        }
        data.push_back(dataValues);
    }
    //cout << fileReader.tellg() << " " << bmpFileHeader.bfSize;
    fileReader.close();
    loaded = true;
    return true;
}

bool BmpImage::SaveImage(const string &path) {
    if (!is_loaded()) {
        cerr << "Bmp File Not Loaded, Cannot Save!" << endl;
        return false;
    }
    //bmpInfoHeader.biHeight = -bmpInfoHeader.biHeight;
    ofstream fileWriter;
    fileWriter.open(path, ios::out | ios::binary | ios::trunc);
    if (!fileWriter.is_open()) {
        cerr << "Writer: Failed to open the file" << endl;
        return false;
    }
    fileWriter.write(reinterpret_cast<char *>(&bmpFileHeader), sizeof(bmpFileHeader));
    fileWriter.write(reinterpret_cast<char *>(&bmpInfoHeader), sizeof(bmpInfoHeader));
    if (!palette.empty()) {
        for (auto value:palette) {
            fileWriter.write(reinterpret_cast<char *>(&value), sizeof(value));
        }
    }
    for (const auto &dataValues:data) {
        for (auto dataValue:dataValues) {
            fileWriter.write(reinterpret_cast<char *>(&dataValue), sizeof(dataValue));
        }
    }
    fileWriter.close();
    return true;
}

void BmpImage::ShowBMPInfo() {
    if (!is_loaded()) {
        cerr << "Bmp File Not Loaded, Cannot Tell The Information!" << endl;
        return;
    }
    cout << "�ļ�����: " << hex << bmpFileHeader.bfType << endl;
    cout << "�ļ���С: " << dec << bmpFileHeader.bfSize << endl;
    cout << "������1: " << dec << bmpFileHeader.bfReserved1 << endl;
    cout << "������2: " << dec << bmpFileHeader.bfReserved2 << endl;
    cout << "����ƫ��: " << dec << bmpFileHeader.bfOffBits << endl;
    cout << "��Ϣͷ��С: " << dec << bmpInfoHeader.biSize << endl;
    cout << "ͼƬ���: " << dec << bmpInfoHeader.biWidth << endl;
    cout << "ͼƬ�߶�: " << dec << bmpInfoHeader.biHeight << endl;
    cout << "ͼƬ��ɫƽ����: " << dec << bmpInfoHeader.biPlanes << endl;
    cout << "ͼƬλ���: " << dec << bmpInfoHeader.biBitCount << endl;
    cout << "ͼƬѹ������: " << dec << bmpInfoHeader.biCompression << endl;
    cout << "λͼ���ݴ�С: " << dec << bmpInfoHeader.biSizeImage << endl;
    cout << "ˮƽ�ֱ���: " << dec << bmpInfoHeader.biXPelsPerMeter << endl;
    cout << "��ֱ�ֱ���: " << dec << bmpInfoHeader.biYPelsPerMeter << endl;
    cout << "��ɫ����ɫ������: " << dec << bmpInfoHeader.biClrUsed << endl;
    cout << "��Ҫ��ɫ������: " << dec << bmpInfoHeader.biClrImportant << endl;

}

#endif //SIMPLEPHOTOSHOP_SAVELOAD_H
