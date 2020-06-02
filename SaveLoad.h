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
struct BmpFileHeader //文件头, 14字节
{
    ushort bfType;  //2字节，文件类型，标识该文件为bmp文件,判断文件是否为bmp文件，即用该值与"0x4d42"比较是否相等即可，0x4d42 = 19778
    ulong bfSize;   //4字节，文件大小
    ushort bfReserved1; //2字节，预保留位,必须设置为0
    ushort bfReserved2; //2字节，预保留位,必须设置为0
    ulong bfOffBits;    //4字节，从头到位图数据的偏移，即图像数据区的起始位置
};

struct BmpInfoHeader //信息头, 40字节
{
    ulong biSize;    //4字节，信息头的大小，即40
    long biWidth;    //4字节，以像素为单位说明图像的宽度
    long biHeight;    //4字节，以像素为单位说明图像的高度，同时如果为正，说明位图倒立（即数据表示从图像的左下角到右上角），如果为负说明正向
    ushort biPlanes;    //2字节，为目标设备说明颜色平面数，总被设置为1
    ushort biBitCount;  //2字节，说明比特数/像素数，8-灰度图；24-真彩色
    ulong biCompression;    //4字节，说明图像的压缩类型，最常用的就是0（BI_RGB），表示不压缩
    ulong biSizeImage;      //4字节，说明位图数据的大小，当用BI_RGB格式时，可以设置为0
    long biXPelsPerMeter;  //表示水平分辨率，单位是像素/米，有符号整数
    long biYPelsPerMeter;  //表示垂直分辨率，单位是像素/米，有符号整数
    ulong biClrUsed;    //说明位图使用的调色板中的颜色索引数，为0说明使用所有
    ulong biClrImportant;  //说明对图像显示有重要影响的颜色索引数，为0说明都重要
};

struct Palette //调色板
{
    uchar b;
    uchar g;
    uchar r;
    uchar alpha; //预保留位
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
    int width; //长度
    int height; //宽度
    int channels; //通道数
    vector<vector<uchar>> data; //图片数据
    vector<Palette> palette; //调色板数据
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
    cout << "文件类型: " << hex << bmpFileHeader.bfType << endl;
    cout << "文件大小: " << dec << bmpFileHeader.bfSize << endl;
    cout << "保留字1: " << dec << bmpFileHeader.bfReserved1 << endl;
    cout << "保留字2: " << dec << bmpFileHeader.bfReserved2 << endl;
    cout << "数据偏移: " << dec << bmpFileHeader.bfOffBits << endl;
    cout << "信息头大小: " << dec << bmpInfoHeader.biSize << endl;
    cout << "图片宽度: " << dec << bmpInfoHeader.biWidth << endl;
    cout << "图片高度: " << dec << bmpInfoHeader.biHeight << endl;
    cout << "图片颜色平面数: " << dec << bmpInfoHeader.biPlanes << endl;
    cout << "图片位深度: " << dec << bmpInfoHeader.biBitCount << endl;
    cout << "图片压缩类型: " << dec << bmpInfoHeader.biCompression << endl;
    cout << "位图数据大小: " << dec << bmpInfoHeader.biSizeImage << endl;
    cout << "水平分辨率: " << dec << bmpInfoHeader.biXPelsPerMeter << endl;
    cout << "垂直分辨率: " << dec << bmpInfoHeader.biYPelsPerMeter << endl;
    cout << "调色板颜色索引数: " << dec << bmpInfoHeader.biClrUsed << endl;
    cout << "重要颜色索引数: " << dec << bmpInfoHeader.biClrImportant << endl;

}

#endif //SIMPLEPHOTOSHOP_SAVELOAD_H
