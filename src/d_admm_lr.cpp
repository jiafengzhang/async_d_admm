/*************************************************************************
    > File Name: ssp_admm_lr.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#include <iostream>
#include "d_admm_lr.h"

using namespace std;

ADMM::ADMM(args_t *args, InstanceData *ins1)
{
    ins = ins1;
    dataNum = ins->getDataNum(); dim = ins->getDim();
    myid = args->myid; procnum = args->procnum;
    rho = args->rho * 0.5; ABSTOL = args->epsilon;
    alpha.setZero(dim); x_i.setZero(dim);
    L.setZero(dim); L_pre.setZero(dim); L_.setZero(dim);
    outfile = new char[40];
    sprintf(outfile,"../out/d_admm_result.dat");
    if(myid ==0)
        of.open(outfile);
    MatrixXd eye;
    eye.setIdentity(dim,dim);   //单位矩阵
    lemmo = (ins->dataMat.transpose() * ins->dataMat + 2*(procnum-1)*rho*eye).inverse();
    Atb = ins->dataMat.transpose() * ins->label;
    x = new double[dim]; x_sum = new double[dim];
    for(int i = 0; i < dim; i++)
    {
        x[i] = 0; x_sum[i] = 0;
    }
}
ADMM::~ADMM()
{
    delete []x;
}

void ADMM::x_update()
{
    //Atb = ins->dataMat.transpose() * ins->label;
    x_i = lemmo *(Atb+rho*L-alpha);
    for(int i = 0; i < dim; i++)
    {
        x[i] = x_i(i);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    clock_t  time_bwait,time_ewait;
    time_bwait = clock();
    MPI_Allreduce(x,x_sum,dim,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    time_ewait = clock();
    wait_time += (double)(time_ewait-time_bwait)/CLOCKS_PER_SEC;
    for(int i = 0; i < dim; i++)
    {
        L(i) = (procnum-2)*x_i(i) + x_sum[i];
        L_(i) = procnum * x_i(i) - x_sum[i];
    }
}

void ADMM::alpha_update()
{

    alpha += rho*L_;
}

bool ADMM::is_stop()
{
    VectorXd tmp = x_i - ins->solution;
    double eps = tmp.norm() / ins->solution.norm();
    time_now = clock();
    double sum_time = (double)(time_now-time_begin)/CLOCKS_PER_SEC;
    if(myid == 0)
    {
        printf("%10.4f  %.10f\n", x_i.norm(), eps);
        of <<eps<<"\t"<< sum_time<<"\t"<<wait_time<<endl;
    }
    if(eps < ABSTOL)
        return true;
    else
        return false;
}
void ADMM::train()
{
    int32_t k = 0;
	MPI_Barrier(MPI_COMM_WORLD);
    time_begin = clock();
    wait_time = 0;
    while(k < 10000)
    {
        if(myid ==0)
        {
            printf("%3d",k);
            of << k <<"\t";
        }
        x_update();
        alpha_update();
        if(myid == 0)
            if(is_stop())
            {
                cout<<myid<<" return"<<endl;
                return;
            }

        k++;
    }
}
