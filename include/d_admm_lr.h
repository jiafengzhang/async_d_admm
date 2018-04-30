/*************************************************************************
    > File Name: ssp_admm_lr.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#ifndef D_ADMM_LR_H
#define D_ADMM_LR_H

#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END

#include<mpi.h>
#include<iostream>
#include<Eigen/Dense>
#include<cmath>
#include<fstream>
#include<time.h>
#include<stdio.h>
#include"instanceData.h"
#include"conf_util.h"

using namespace Eigen;
using namespace std;

class ADMM
{
public:
    ADMM(args_t *args,InstanceData *ins);
    ~ADMM();
    void x_update();
    void alpha_update();
    bool is_stop();
    void train();
private:
    int32_t dataNum, dim;
    int32_t myid, procnum;
    VectorXd alpha, x_i, L, L_pre, L_, Atb;
    MatrixXd lemmo, lemmo1;
    double *x, *x_sum;
    double rho;
    double ABSTOL;
    double RELTOL;
    InstanceData *ins;
    char *outfile;
    ofstream of;
    clock_t time_begin, time_now;
    double wait_time;
};


#endif
