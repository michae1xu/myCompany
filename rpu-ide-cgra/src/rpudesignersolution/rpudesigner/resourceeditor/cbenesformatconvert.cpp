#include "common/debug.h"
#include "cbenesformatconvert.h"

CBenesFormatConvert::CBenesFormatConvert()
{
    x = new int*[14];
    config = new int*[13];
    for (int i = 0; i < 14; i++)
    {
        if (i == 0)
        {
            x[i] = new int[128] {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
                37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,
                71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,
                104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127};
        }
        else
        {
            x[i] = new int[128];
            memset(x[i], 0, sizeof(int) * 128);
        }
        if (i != 13)
        {
            config[i] = new int[64];
            memset(config[i], 0, sizeof(int) * 64);
        }
    }

}

CBenesFormatConvert::~CBenesFormatConvert()
{
    for (int i = 0; i < 14; i++)
    {
        delete[] x[i];
        if (i != 13)
        {
            delete[] config[i];
        }
    }
    delete[] x;
    delete[] config;
}

void CBenesFormatConvert::setValue(QList<uint> *valueList)
{
    for(int i = 0; i < valueList->count(); ++i)
    {
        x[13][i] = valueList->at(i);
    }
}

void CBenesFormatConvert::convert128To26(int level, int n)
{
    int i = 0, j = 0, k = 0;
    int m1 = 0, m2 = 0;
    int temp = 0, t1 = 0, t2 = 0;
    int b1 = 0, b2 = 0;
    int *p1 = new int[128] {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1
};

    int *p2 = new int[128] {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1
};

    switch (n) {
    case 128: {
        t1 = 1;
        t2 = 12;
        b1 = 0;
        b2 = 12;
        break;
    }
    case 64: {
        t1 = 2;
        t2 = 11;
        b1 = 1;
        b2 = 11;
        break;
    }
    case 32: {
        t1 = 3;
        t2 = 10;
        b1 = 2;
        b2 = 10;
        break;
    }
    case 16: {
        t1 = 4;
        t2 = 9;
        b1 = 3;
        b2 = 9;
        break;
    }
    case 8: {
        t1 = 5;
        t2 = 8;
        b1 = 4;
        b2 = 8;
        break;
    }
    case 4: {
        t1 = 6;
        t2 = 7;
        b1 = 5;
        b2 = 7;
        break;
    }
    case 2: {
//        b1 = 6;
        b2 = 6;
        if ((x[6][0 + level*n] == x[7][0 + level*n]) && (x[6][1 + level*n] == x[7][1 + level*n]))
        {
//            benes[0 + level * n / 2][b2] = 0;
//            config[b2][0 + level*n/2] = 0;
        }
        else if ((x[6][0 + level*n] == x[7][1 + level*n]) && (x[6][1 + level*n] == x[7][0 + level*n]))
        {
//            benes[0 + level * n / 2][b2] = 1;
            config[b2][0 + level*n/2] = 1;
        }
        delete[] p1;
        p1 = nullptr;
        delete[] p2;
        p2 = nullptr;
        return;
    }
    }

    x[t1][0 + level*n] = x[t1 - 1][0 + level*n];
    x[t1][n/2 + level*n] = x[t1 - 1][1 + level*n];

    //benes[0 + level * n / 2][b1] = 0;
    config[b1][0 + level*n/2] = 0;

    for (j = 0; j < n; j++)
        if (x[t2 + 1][j + level*n] == x[t1 - 1][0 + level*n]) {
            if (j%2 == 0) {
                x[t2][j/2 + level*n] = x[t2 + 1][j + level*n];
                x[t2][j/2 + level*n + n/2] = x[t2 + 1][j + 1 + level*n];
                temp = x[t2 + 1][j + 1 + level*n];
                //benes[j / 2 + level * n / 2][b2] = 0;
                config[b2][j/2 + level*n/2] = 0;
            }
            else {
                x[t2][(j - 1)/2 + level*n] = x[t2 + 1][j + level*n];
                x[t2][(j - 1)/2 + level*n + n/2] = x[t2 + 1][j - 1 + level*n];
                temp = x[t2 + 1][j - 1 + level*n];
                //benes[(j - 1) / 2 + level * n / 2][b2] = 1;
                config[b2][(j - 1)/2 + level*n/2] = 1;
            }
        }


    for (k = 0; k < (n/2 - 1); k++) {
        for (i = 0; i < n; i++) {
            p1[i] = x[t1][i + level*n];
            p2[i] = x[t2][i + level*n];
        }

        sort(p1, n);
        sort(p2, n);

        m1 = 0;

        for (i = 0; i < n; i++)
            if (p1[i] == p2[i])
                m1++;

        if (m1 == n) {
            for (i = 0; i < n; i++) {
                m2 = 0;
                for (j = 0; j < n; j++) {
                    if (x[t1 - 1][i + level*n] != x[t1][j + level*n])
                        m2++;
                }
                if (m2 == n)
                    break;
            }

            x[t1][i/2 + level*n] = x[t1 - 1][i + level*n];
            x[t1][i/2 + level*n + n/2] = x[t1 - 1][i + 1 + level*n];
            temp = x[t1 - 1][i + level*n];
            //benes[i / 2 + level * n / 2][b1] = 0;
            config[b1][i/2 + level*n/2] = 0;
        }
        else
            for (i = 0; i < n; i++) {
                if (x[t1 - 1][i + level*n] == temp) {
                    if (i%2 == 0) {
                        x[t1][i/2 + level*n + n/2] = x[t1 - 1][i + level*n];
                        x[t1][i/2 + level*n] = x[t1 - 1][i + 1 + level*n];
                        temp = x[t1 - 1][i + 1 + level*n];
                        //benes[i / 2 + level * n / 2][b1] = 1;
                        config[b1][i/2 + level*n/2] = 1;
                        break;
                    }
                    else {
                        x[t1][(i - 1)/2 + level*n + n/2] = x[t1 - 1][i + level*n];
                        x[t1][(i - 1)/2 + level*n] = x[t1 - 1][i - 1 + level*n];
                        temp = x[t1 - 1][i - 1 + level*n];
                        //benes[(i - 1) / 2 + level * n / 2][b1] = 0;
                        config[b1][(i - 1)/2 + level*n/2] = 0;
                        break;
                    }
                }
            }
        //printf("temp=%d\n",temp);

        for (j = 0; j < n; j++) {
            if (x[t2 + 1][j + level*n] == temp) {
                if (j%2 == 0) {
                    x[t2][j/2 + level*n] = x[t2 + 1][j + level*n];
                    x[t2][j/2 + level*n + n/2] = x[t2 + 1][j + 1 + level*n];
                    temp = x[t2 + 1][j + 1 + level*n];
                    //benes[j / 2 + level * n / 2][b2] = 0;
                    config[b2][j/2 + level*n/2] = 0;
                    break;
                }
                else {
                    x[t2][(j - 1)/2 + level*n] = x[t2 + 1][j + level*n];
                    x[t2][(j - 1)/2 + level*n + n/2] = x[t2 + 1][j - 1 + level*n];
                    temp = x[t2 + 1][j - 1 + level*n];
                    //benes[(j - 1) / 2 + level * n / 2][b2] = 1;
                    config[b2][(j - 1)/2 + level*n/2] = 1;
                    break;
                }
            }
        }
        //printf("temp=%d\n",temp);
    }

    convert128To26(2*level + 0, n/2);
    convert128To26(2*level + 1, n/2);

    delete[] p1;
    p1 = nullptr;
    delete[] p2;
    p2 = nullptr;
}

void CBenesFormatConvert::convert26To128()
{

}

void CBenesFormatConvert::sort(int *a, int n)
{
    int i, j, temp;

    for (j = 0; j <= n - 1; j++)
    {
        for (i = 1; i < n - j; i++)
        {
            if (a[i - 1] > a[i])
            {
                temp = a[i - 1];
                a[i - 1] = a[i];
                a[i] = temp;
            }
        }
    }
}
