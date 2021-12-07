#include <QCoreApplication>
#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>

unsigned int a0[8];
unsigned int sbox[8][8]=
{{0x2a, 0x6c, 0xd7, 0xec, 0x2d, 0xaa, 0x04, 0x22},//B00,B01,B02
 {0xbc, 0xf0, 0x60, 0x60, 0x4d, 0xfd, 0x11, 0xed},
 {0xbf, 0xd5, 0xee, 0x52, 0x07, 0x46, 0x4e, 0x9a},
 {0x26, 0x8f, 0x52, 0x66, 0x54, 0x06, 0x5d, 0x34},
 {0xed, 0x40, 0x55, 0x17, 0xa8, 0x23, 0xce, 0xcd},
 {0x90, 0x61, 0xd5, 0xf4, 0x4f, 0x4b, 0x61, 0x3e},
 {0xcc, 0xef, 0x30, 0x69, 0x17, 0x76, 0x94, 0x6e},
 {0x36, 0x34, 0xf5, 0x0e, 0x4d, 0xe7, 0x06, 0x3a}};

int sjcf()
{
    FILE *fp=fopen("D:/a_in.txt","r");
    unsigned int x[2];
    int j;
    int i,k;
    if(fp==NULL)
    {
        return -1;
    }

    for(j=0;j<2;j=j+1)
    {
        fscanf(fp,"%x",&x[j]);
    }
    for(i=0;i<4;i++)
    {
        a0[i]=(x[0]>>(i*8))& 0xFF;
        a0[i+4]=(x[1]>>(i*8))& 0xFF;
    }
    fclose(fp);
    return 0;
}

unsigned char XTIME(unsigned char x) {
    return ((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00));
}
unsigned char multiply(unsigned char a, unsigned char b) {
    unsigned char temp[8] = {a};
    unsigned char tempmultiply = 0x00;
    int i = 0;
    for (i = 1; i < 8; i++) {
        temp[i] = XTIME(temp[i - 1]);
    }
    tempmultiply = (b & 0x01) * a;
    for (i = 1; i <= 7; i++) {
        tempmultiply ^= (((b >> i) & 0x01) * temp[i]);
    }
    return tempmultiply;
}


//unsigned int* s0(unsigned int*arr,unsigned int**brr)
//{
//    int i,j;
//    uint32_t A[4];
//    uint8_t x0,x1,x2,x3;
//    uint32_t *p;
//    for (i=0;i<4;i=i+1)
//    {
//        x0=multiply(*(arr+i),*((brr+i*8)+0));
//        printf("%x\n",x0);
//        x1=multiply(*(arr+i),*((brr+i*8)+1));
//        printf("%x\n",x1);
//        x2=multiply(*(arr+i),*((brr+i*8)+2));
//        printf("%x\n",x2);
//        x3=multiply(*(arr+i),*((brr+i*8)+3));
//        printf("%x\n",x3);
//        A[i]=(x3<<24)|(x2<<16)|(x1<<8)|x0;
//    }
//    p = A;

//    return p;
//}


unsigned int* s1(unsigned int arr[],unsigned int brr[][8])
{
    int i,j;
    uint32_t A[4];
    uint8_t x0,x1,x2,x3;
    uint32_t *p;
    for (i=4;i<8;i=i+1)
    {
        x0=multiply(arr[i],brr[i][0]);
        printf("%x\n",x0);
        x1=multiply(arr[i],brr[i][1]);
        printf("%x\n",x1);
        x2=multiply(arr[i],brr[i][2]);
        printf("%x\n",x2);
        x3=multiply(arr[i],brr[i][3]);
        printf("%x\n",x3);
        A[i]=(x3<<24)|(x2<<16)|(x1<<8)|x0;
    }
    p = A;

    return p;
}

unsigned int* s2(unsigned int arr[],unsigned int brr[][8])
{
    int i,j;
    uint32_t A[4];
    uint8_t x0,x1,x2,x3;
    uint32_t *p;
    for (i=4;i<8;i=i+1)
    {
        x0=multiply(arr[i],brr[i][4]);
        printf("%x\n",x0);
        x1=multiply(arr[i],brr[i][5]);
        printf("%x\n",x1);
        x2=multiply(arr[i],brr[i][6]);
        printf("%x\n",x2);
        x3=multiply(arr[i],brr[i][7]);
        printf("%x\n",x3);
        A[i]=(x3<<24)|(x2<<16)|(x1<<8)|x0;
    }
    p = A;

    return p;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    unsigned int *A_1;
    sjcf();
    A_1 = s2(a0,sbox);
    printf("%x\n",*(A_1+0));
    printf("%x\n",*(A_1+1));
    printf("%x\n",*(A_1+2));
    printf("%x\n",*(A_1+3));
    return a.exec();
}
