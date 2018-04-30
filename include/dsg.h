#ifndef DSG_H
#define DSG_H

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

class DSG
{
public:
    DSG(args_t *args,InstanceData *ins);
    ~DSG();
    void x_update(int k);
    void train();
    bool is_stop();
private:
    int myid, procnum;
    InstanceData *ins;
    int dataNum, dim, dgree;
    double rho;
    double ABSTOL ;
    double RELTOL ;
    MatrixXd lemmo,Atb;
    VectorXd x_i, wx;
    double **msgbuf;
    double *x, *x_sum;
    MPI_Request* recvRequests; MPI_Request* sendRequests;
    MPI_Status* statuses, *sendstatus;
    int * neighbor;
    clock_t time_begin, time_now;
    double wait_time;
    char * outfile;
    ofstream of;

};


#endif
