/*************************************************************************
    > File Name: train.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-10-27
 ************************************************************************/

#include<iostream>
#include"instanceData.h"
#include"ssp_admm_lr.h"
#include"conf_util.h"
#include<mpi.h>
#include<stdio.h>
#include<string>
using namespace std;

int main(int argc, char ** argv)
{
    int myid, procnum;
    clock_t start_time, end_time;
    //if(argc < 2)
    //{
//	cout<<"useage: ./a.out s"<<endl;
//	//return 0;
   // }
//    int s = atoi(argv[1]);
    start_time = clock();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &procnum);
    args_t *args = new args_t(myid, procnum);
    args->get_args();
    if(args->dgree >  procnum - 1)
    {
        cerr<<"dgree configure error"<<endl;
        return 0;
    }
    if(args->dgree%2 && procnum % 2)
    {
        cerr<<"cannot create network"<<endl;
        return 0;
    }
    if(myid == 0)
        args->print_args();
    char filenameA[20], filenameb[20];
    sprintf(filenameA, "/lineardataA_%d.dat", myid);
    sprintf(filenameb, "/lineardatab_%d.dat", myid);
    string fileA, fileb;
    fileA = args->data_dir + filenameA;
    fileb = args->data_dir + filenameb;
    InstanceData *ins = new InstanceData(fileA.c_str(),fileb.c_str());
    //args->s = s;
    ADMM admm(args, ins);
    admm.train();
    MPI_Barrier(MPI_COMM_WORLD);
    int sum_times = 0;
    MPI_Allreduce(&admm.commtime,&sum_times,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
    admm.of<<"Communication times: "<<sum_times<<endl;
    MPI_Finalize();
    end_time = clock();
    if(myid == 0)
        cout<<"run time: "<<(double)(end_time-start_time)/CLOCKS_PER_SEC<<endl;
}
