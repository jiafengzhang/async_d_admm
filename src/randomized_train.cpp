/*************************************************************************
    > File Name: randomized_admm.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2019-08-21
 ************************************************************************/
#include"instanceData.h"
#include"conf_util.h"
#include"randomized_admm.h"
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
    char filenameA[100], filenameb[100];
    sprintf(filenameA, "../data/lineardataA_%d.dat", myid);
    sprintf(filenameb, "../data/lineardatab_%d.dat", myid);
/*    string fileA, fileb;
    fileA = args->data_dir + filenameA;
    fileb = args->data_dir + filenameb;
*/
    InstanceData *ins = new InstanceData(filenameA,filenameb);
    RandomizedADMM radmm(args, ins);
    radmm.train();
    //cout<<myid<<" finish"<<endl;
    //MPI_Barrier(MPI_COMM_WORLD);
    //MPI_Finalize();
    end_time = clock();
    if(myid == 0)
        cout<<"run time: "<<(double)(end_time-start_time)/CLOCKS_PER_SEC<<endl;
    return 0;
}

