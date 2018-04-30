/*************************************************************************
    > File Name: admm.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-12-13
 ************************************************************************/

#include"admm.h"

using namespace std;

ADMM::ADMM(args_t *args, InstanceData *ins1)
{
    ins = ins1;
    dataNum = ins->getDataNum(); dim = ins->getDim();
    myid = args->myid; procnum = args->procnum;
    rho = args->rho; ABSTOL = args->epsilon;
    x_i.setZero(dim);
    x = new double[dim]; y = new double[dim];
    z = new double[dim]; w = new double[dim];
    outfile = new char[40];
    sprintf(outfile,"../out/admmresult.txt");
    if(myid ==0)
        of.open(outfile);
    MatrixXd eye;
    eye.setIdentity(dim,dim);   //单位矩阵
    lemmo = (ins->dataMat.transpose() * ins->dataMat + rho*eye).inverse();
    for(int32_t i = 0; i < dim; i++)
    {
        x[i] = 0; y[i] = 0;
        z[i] = 0; w[i] = 0;
    }
}
ADMM::~ADMM()
{
	delete []x; delete []y;
	delete []z; delete []w;
	delete []outfile;
}

void ADMM::x_update()
{
    VectorXd Atb = ins->dataMat.transpose() * ins->label;
    VectorXd tmp(dim);
    for(int32_t i = 0; i < dim; i++)
    {
        tmp(i) = rho * z[i] - y[i];
    }
    x_i = lemmo * (Atb + tmp);
    for(int32_t i = 0; i < dim; i++)
    {
        x[i] = x_i(i);
    }
}
void ADMM::y_update()
{
    for(int32_t i = 0; i < dim; i++)
    {
        y[i] += rho * (x[i] - z[i]);
    }
}
void ADMM::z_update()
{
    for(size_t i = 0; i < dim; i++)
    {
        w[i] = (1.0/procnum) *(x[i] + (1.0/rho) *y[i]);//rho*x[i] + y[i]
    }
    MPI_Allreduce(w,z, dim, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
}
bool ADMM::is_stop()
{
    VectorXd tmp = x_i - ins->solution;
    double eps = tmp.norm() / ins->solution.norm();
    time_now = clock();
    double sum_time = (double)(time_now-time_begin)/CLOCKS_PER_SEC;
    if(myid == 0)
    {
        of <<eps<<"\t"<<endl;
        printf("%10.4f  %.10f\t", x_i.norm(), eps);
		cout<<endl;
    }
    if(eps < ABSTOL)
        return true;
    else
        return false;
}
void ADMM::train()
{
    int32_t k = 0;
    if(myid == 0)
        printf("%3s %10s\n","#", "eps");
    while(k <= 1000)
    {
        if (myid == 0)
        {
            of << k <<"\t";
            printf("%3d ",k);
        }
        x_update();
        z_update();
        y_update();
        if(is_stop())
            break;
        k++;
    }
}
