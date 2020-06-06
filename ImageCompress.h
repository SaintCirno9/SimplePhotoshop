#pragma   once

#include "SaveLoad.h"
#include "Bitmap.h"
#include "huffman.h"


using namespace std;

class ImageCompress {
public:
    ImageCompress() = default;

    explicit ImageCompress(BmpImage &bmpImage, int channels) {
        if (!bmpImage.getData().empty() && (channels == 1 || channels == 3)) {
            this->bmpImage = bmpImage;
            this->channels = channels;
            this->bmpData = bmpImage.getData();
            cout << channels << endl;
        } else {
            exit(1);
        }
    }

    void doOperation();

private:
    BmpImage bmpImage;
    vector<vector<uchar>> bmpData;
    int channels;

    void TryUniformQuantization(string postFix);

    void getPrecodeImg(string postFix);
};

void ImageCompress::doOperation() {
    cout << "Please choose an operation:\n1.Uniform Quantization\n2.Precode" << endl;
    int choice = 0;
    cin >> choice;
    while (choice < 1 || choice > 2) {
        cout << "Wrong input! Please type a num between 1-2. " << endl;
        cin >> choice;
    }
    if (choice == 1) {
        TryUniformQuantization("_Uniq.bmp");
    } else {
        getPrecodeImg("_Precode.bmp");
    }
}

//r=0 igs量化 r=2 2倍量化 r=4 4倍量化
void ImageCompress::TryUniformQuantization(string postFix) {
    int r = 0;
    cout << "Please input the Quantization Multiple:";
    cin >> r;
    ///输入项目
    //图像宽度
    int width = bmpImage.getWidth();
    //图像高度
    int height = bmpImage.getHeight();
    //创建向量容器
    vector<vector<int>> p;

    //初始化向量
    p.resize(width);
    for (int k = 0; k < width; k++) {
        p[k].resize(height);
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            p[x][y] = (int) bmpData[x][y];
        }
    }

    //压缩比固定为2，4;igs写为0实质为2;
    vector<vector<int>> mp;

    //初始化向量
    mp.resize(width);
    for (int k = 0; k < width; k++) {
        mp[k].resize(height);
    }
    //量化
    if (r == 0) {
        int sum = 0;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (p[x][y] <= 240) {
                    mp[x][y] = (p[x][y] + sum) / 16;
                    sum = mp[x][y] % 16;
                } else {
                    mp[x][y] = (p[x][y]) / 16;
                }
            }
        }
    } else if (r == 2) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                mp[x][y] = p[x][y] / 16;
            }
        }
    } else if (r == 4) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                mp[x][y] = p[x][y] / 64;
            }
        }
    }


    //反量化
    //创建向量容器
    vector<vector<int>> np;

    //初始化向量
    np.resize(height);
    for (int k = 0; k < width; k++) {
        np[k].resize(height);
    }

    //反量化
    if (r == 0 || r == 2) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                np[x][y] = mp[x][y] * 16;
            }
        }
    } else if (r == 4) {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                np[x][y] = mp[x][y] * 64;
            }
        }
    }

    //写进图片
    vector<vector<uchar>> out;
    out.resize(height);
    for (int k = 0; k < width; k++) {
        out[k].resize(height);
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            out[x][y] = (uchar) np[x][y];
        }
    }
    bmpImage.setData(out);
    bmpImage.SaveImage(postFix);
}


//预测器阶数int order;
//预测器系数int e0;
void ImageCompress::getPrecodeImg(string postFix) {
    int order = 0;
    int e0 = 0;
    cout << "Please input the Predictor Order:";
    cin >> order;
    cout << "Please input the Predictor Coefficient";
    cin >> e0;
    //宽度
    int width = bmpImage.getWidth();
    //高度
    int height = bmpImage.getHeight();

    //创建向量并初始化
    vector<vector<int>> f;
    vector<vector<int>> e;
    e.resize(width);
    f.resize(width);
    for (int k = 0; k < width; k++) {
        f[k].resize(height);
        e[k].resize(height);
    }
    //写入图像
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            f[x][y] = (int) (bmpData[x][y]);
        }
    }

    //一阶预测器
    if (order == 1) {
        vector<vector<int>> ff;
        ff.resize(width);
        for (int k = 0; k < width; k++) {
            ff[k].resize(height);
        }
        //for ++ ;maxx是宽度
        for (int x = 0; x < width; x++)
            e[x][0] = e0;
        for (int x = 1; x < width; x++) {
            for (int y = 0; y < height; y++) {
                ff[x][y] = f[x - 1][y];
                e[x][y] = f[x][y] - f[x - 1][y];
            }
        }
    }
        //二阶预测器
    else if (order == 2) {
        vector<vector<int>> ff;
        vector<vector<int>> dh;
        vector<vector<int>> dv;

        //初始化向量大小
        ff.resize(width);
        dh.resize(width);
        dv.resize(width);

        for (int k = 0; k < width; k++) {
            ff[k].resize(height);
            dh[k].resize(height);
            dv[k].resize(height);
        }

        for (int x = 0; x < width; x++)
            dh[x][0] = e0;
        for (int y = 0; y < height; y++)
            dv[0][y] = e0;
        for (int x = 1; x < width; x++) {
            for (int y = 1; y < height; y++) {
                dh[x - 1][y] = f[x - 1][y] - f[x - 1][y - 1];
                dv[x][y - 1] = f[x][y - 1] - f[x - 1][y - 1];
            }
        }
        for (int x = 1; x < width; x++) {
            for (int y = 1; y < height; y++) {
                if (dh[x][y] <= dv[x][y])
                    ff[x][y] = f[x][y - 1];
                else
                    ff[x][y] = f[x - 1][y];
            }
        }
        //for ++
        for (int x = 1; x < width; x++) {
            for (int y = 1; y < height; y++) {
                ff[x][y] = f[x - 1][y];
                e[x][y] = f[x][y] - f[x - 1][y];
            }
        }
    }

    //对e进行hfm编码并输出文件

    //huffman编码

    int n = 0, i;
    int weightId[SIZE], w[SIZE];
    memset(w, 0, sizeof(w));
    for (i = 0; i < SIZE; i++) {
        weightId[i] = -1;
    }
    //计算权重
    int weight[SIZE] = {0};
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            weight[((e[x][y] + 256) / 2)]++;
        }
    }
    //建树
    HuffmanTree huffmantree;
    n = GetWeightId(weightId, w);
    HuffmanCode hc;
    CHT(&huffmantree, w, weightId, n);
    CHC(&huffmantree, &hc, w, weightId, n);

    //输出文件
    /*FILE *fp;
    stringstream ss;
    string s;
    fp = fopen(R"(C:\Users\Cirno\Desktop\code.hfm)", "w");
    char ch;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            ch = ((e[x][y] + 256) / 2);
            fputc((*hc)[(int) ch], fp);
        }
    }
    fclose(fp);
    printf("1");*/

    vector<vector<uchar>> out;
    out.resize(height);
    for (int k = 0; k < width; k++) {
        out[k].resize(height);
    }
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            out[x][y] = (uchar) ((e[x][y] - e0 + 256) / 2);
        }
    }
    bmpImage.setData(out);
    auto bitMap = Bitmap(bmpImage, channels);
    bitMap.GrayHistogramEqualization(postFix);
}

