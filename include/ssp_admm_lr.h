/*************************************************************************
    > File Name: ssp_admm_lr.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#ifndef SSP_ADMM_LR_H
#define SSP_ADMM_LR_H


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
    void alpha_update();
    bool is_stop();
    void stop();
    int miniter(double **, int);
    int maxiter(double **, int);
    int getdelay();
    void train();
    ofstream of;
    int commtime;
private:
    int32_t dataNum, dim;
    int32_t myid, procnum;
    int32_t s, delta, dgree;
    bool adpstep;
    VectorXd alpha, x_i, L, L_pre, L_;
    MatrixXd lemmo, lemmo1, Atb;
    double **msgbuf, **al, *step;
    int * neighbor;
    double rho = 2;
    double ABSTOL = 10e-10;
    double RELTOL = 10e-2;
	double stepsize;
    InstanceData *ins;
    MPI_Request* recvRequests; MPI_Request* sendRequests;
    MPI_Status* statuses, *sendstatus;
    int* indices, *a, *index, *delay, sum;
    char *outfile;

    clock_t time_begin, time_now;
    double wait_time;

};


#endif
