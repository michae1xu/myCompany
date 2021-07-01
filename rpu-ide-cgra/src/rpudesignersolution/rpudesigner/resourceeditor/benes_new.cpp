#include "benes_new.h"

benes_new::benes_new()
{

}

void benes_new::change_by_cfg(int in[], bool cfg, int out[])
{
    if(cfg) {
        out[0] = in[1];
        out[1] = in[0];
    } else {
        out[0] = in[0];
        out[1] = in[1];
    }
}

void benes_new::sub_benes128x128(int in[], bool cfg[], int out[])
{
    int _in[2];
    int _out[2];
    for(int i = 0; i < 64; i++) {
        _in[0] = in[i*2];
        _in[1] = in[i*2+1];
        change_by_cfg(_in, cfg[i], _out);
        out[i*2] = _out[0];
        out[i*2+1] = _out[1];
    }
}

void benes_new::benes128x128(int in[], const vector<bool> &cfg, int out[])
{
    int _tmp_in[128];
    for(int i = 0; i < 128; i++) _tmp_in[i] = in[i];
    int _tmp_out[128];
    bool _cfg[64];

    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 64; j++) _cfg[j] = cfg[i*64+j];
        sub_benes128x128(_tmp_in, _cfg, _tmp_out);
        exchange(_tmp_out, _tmp_in, i);
    }
    for(int j = 0; j < 128; j++) out[j] = _tmp_out[j];
}

int benes_new::cal(int level)
{
    int _ret = 1;
    for(int i = 0; i < level; i++) _ret *= 2;
    return _ret;
}

void benes_new::exchange(int last_out[], int next_in[], int level)
{
//    if(level != 6) {
//        int _count = cal(level+1);//拆分次数
//        int n = 2;
//        if(level > 6) n = cal(level-5);
//        int _min_count = _count / n;    //todo...
//        int _max_each = 128/_min_count;
//        int _each = 128/_count; //每次个数
//        int index = 0;
//        int _cur_start = 0;
//        while(index < 128) {

//            for(int i = _cur_start; i < _cur_start + _max_each; i++) {

//                for(int k = 0; k < _min_count; k++) {
//                    for(int j = 0; j < _each; j++) {
//                        next_in[j+k*_each+_cur_start] = last_out[j*n+k+_cur_start];
//                        index ++;
//                    }
//                }
//            }
//            _cur_start += _max_each;
//        }
//    } else {
//        exchange(last_out, next_in, 5);
//    }

    int _offset = 0;
    int _min_count = 0;
    int _max_count = 0;

    if(level < 6) {
        _offset = 2;
        _min_count = cal(6-level);
        _max_count = cal(7-level);
    } else if(level == 6){
        _offset = 2;
        _min_count = cal(1);
        _max_count = cal(2);
    } else {
        _offset = cal(level-5);
        _min_count = 2;
        _max_count = cal(level-4);
    }

    int _cur = 0;
    for(int i = 0; i < 128/_max_count; i++) {
        for(int j = 0; j < _max_count/_min_count; j++) {
            _cur = j;
            for(int k = 0; k < _min_count; k++) {
                next_in[k+j*_min_count+i*_max_count] = last_out[i*_max_count+_cur];
                _cur += _offset;
            }
        }
    }
}
