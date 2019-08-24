/*************************************************************************
    > File Name: randomized_admm.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2019-08-21
 ************************************************************************/

#ifndef RANDOMIZEDADMM_H
#define RANDOMIZEDADMM_H

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

class RandomizedADMM{
public:
    RandomizedADMM(args_t *args,InstanceData *ins);
    ~RandomizedADMM();
    void x_update();
    void alpha_update(int index);
    bool is_stop();
    void stop();
    int miniter(double **, int);
    int maxiter(double **, int);
    int getdelay();
    void update(int32_t i, int32_t j);
    void train();
    ofstream of;
    int commtime;
private:
    int32_t dataNum, dim;
    int32_t myid, procnum, neighborId;
    int32_t s, delta, dgree;
    bool adpstep;
    VectorXd alpha, x_i, L, L_pre, L_;
    MatrixXd lemmo, lemmo1, Atb;
    double **msgbuf, **al, *step;
    int * neighbor, *edge;
    int ** graph;
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