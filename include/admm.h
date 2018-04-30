/*************************************************************************
    > File Name: admm.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-12-13
 ************************************************************************/
#ifndef ADMM_H
#define ADMM_H

#include<iostream>
#include<mpi.h>
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
    void y_update();
    void z_update();
    bool is_stop();
    void train();
private:
    int32_t dataNum, dim;
    int32_t myid, procnum;
    double *x, *y, *z, *w;
    VectorXd alpha, x_i, L, L_pre, L_;
    MatrixXd lemmo, lemmo1;
    double rho = 2;
    double ABSTOL = 10e-10;
    InstanceData *ins;
    char *outfile;
    ofstream of;
    clock_t time_begin, time_now;
    double wait_time;
};


#endif // ADMM_H


