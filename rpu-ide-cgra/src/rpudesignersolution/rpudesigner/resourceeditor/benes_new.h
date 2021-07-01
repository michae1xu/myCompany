#ifndef BENES_NEW_H
#define BENES_NEW_H

#include <vector>

using namespace std;

class benes_new
{
public:
    benes_new();

    void change_by_cfg(int in[], bool cfg, int out[]);
    void sub_benes128x128(int in[128], bool cfg[64], int out[128]);

    void benes128x128(int in[128], const vector<bool>& cfg, int out[128]);

    int cal(int level);
    void exchange(int last_out[], int next_in[], int level);
};

#endif // BENES_NEW_H
