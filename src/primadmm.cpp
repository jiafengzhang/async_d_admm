/*************************************************************************
    > File Name: primadmm.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-12-20
 ************************************************************************/

#include<iostream>
#include"instanceData.h"
#include"admm.h"
#include"conf_util.h"
#include<mpi.h>
#include<stdio.h>
#include<string>
using namespace std;

int main(int argc, char ** argv)
{
    int myid, procnum;
    clock_t start_time, end_time;
    start_time = clock();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &procnum);
    args_t *args = new args_t(myid, procnum);
    args->get_args();
    if(myid == 0)
        args->print_args();
    char filenameA[20], filenameb[20];
    sprintf(filenameA, "/lineardataA_%d.dat", myid);
    sprintf(filenameb, "/lineardatab_%d.dat", myid);
    string fileA, fileb;
    fileA = args->data_dir + filenameA;
    fileb = args->data_dir + filenameb;
    InstanceData *ins = new InstanceData(fileA.c_str(),fileb.c_str());
    ADMM admm(args, ins);
    admm.train();
    MPI_Finalize();
    end_time = clock();
    cout<<"run time: "<<(double)(end_time-start_time)/CLOCKS_PER_SEC<<endl;
}

