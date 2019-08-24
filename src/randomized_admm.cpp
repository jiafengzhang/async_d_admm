/*************************************************************************
    > File Name: randomized_admm.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2019-08-21
 ************************************************************************/
#include<iostream>

#include "randomized_admm.h"

using namespace std;

RandomizedADMM::RandomizedADMM(args_t *args,InstanceData *ins1){
    ins = ins1;
    dataNum = ins->getDataNum(); dim = ins->getDim();
    myid = args->myid; procnum = args->procnum; dgree = args->dgree;
    s = args->s; delta = args->delta; 
    rho = args->rho; ABSTOL = args->epsilon; 
    alpha.setZero(dim); x_i.setZero(dim);
    outfile = new char[100];
    sprintf(outfile,"../out/admmresult_%d_rho.dat",procnum);
    if(myid ==0)
        of.open(outfile);
    msgbuf = new double*[dgree]; al = new double*[dgree];
    graph = new int*[procnum];
    for(int32_t i = 0; i < dgree; i++)
    {
        msgbuf[i] = new double[dim + 1];
        al[i] = new double[dim + 1];
        for(int32_t j = 0; j < dim+1; j++)
        {
            msgbuf[i][j] = 0;
            al[i][j] = 0;
        }
        msgbuf[i][dim] = -1; al[i][dim] = 0;
    }
    neighbor = new int[dgree];
    edge = new int[2];
    for(int i = 0; i < procnum; i++)
    {
        int * tmp = new int[dgree];
        for(int j = 0; j < dgree / 2; j++)
        {
            tmp[2*j] = (i+j+1) % procnum;
            tmp[2*j+1]  = (i - (j+1) + procnum) % procnum;
        }
        if(dgree % 2)
            tmp[dgree-1] = (i + procnum/2) % procnum;
        graph[i] = tmp;
    }
    
    commtime = 0;
}

RandomizedADMM::~RandomizedADMM()
{
    for(int32_t i = 0; i < dgree; i++)
    {
        delete []msgbuf[i];
    }
    for(int i = 0; i < procnum; i++)
    {
        delete []graph[i];
    }
    delete []msgbuf;
    delete []graph;
    delete []al;
    delete []outfile;
    delete []neighbor;
    delete []edge;
    alpha.setZero(dim); x_i.setZero(dim);
}

void RandomizedADMM::x_update()
{
    //VectorXd Atb = ins->dataMat.transpose() * ins->label;
	MatrixXd eye;
    eye.setIdentity(dim,dim);   //单位矩阵
    lemmo = (ins->dataMat.transpose() * ins->dataMat + (dgree)*rho*eye).inverse();
    Atb = ins->dataMat.transpose() * ins->label;
    alpha.setZero(dim);
    for(int32_t i = 0; i < dgree; i++)
    {
	    for(int32_t j = 0; j < dim; j++)
	    {
		alpha(j) += msgbuf[i][j];
	    }
    }
    x_i = lemmo * (Atb - alpha);
    //cout<<"x_update myid:"<<myid<<endl;

}

void RandomizedADMM::alpha_update(int index)
{
    for(int32_t j = 0; j < dim; j++)
    {
        al[index][j] += -1*(0.5*(al[index][j] + msgbuf[index][j]) + rho * x_i(j));
    }
    //cout<<"alpha_update myid:"<<myid<<endl;
}

bool RandomizedADMM::is_stop()
{
    VectorXd tmp = x_i - ins->solution;
    double eps = tmp.norm() / ins->solution.norm();
    time_now = clock();
    double sum_time = (double)(time_now-time_begin)/CLOCKS_PER_SEC;
    if(myid == 0)
    {
        of <<eps<<"\t"<< sum_time<<"\t"<<wait_time<<endl;
        printf("%10.4f  %.10f\n", x_i.norm(), eps);
    }
    if(eps < ABSTOL)
        return true;
    else
        return false;
}

void RandomizedADMM::update(int32_t i, int32_t j)
{
    if(myid == i)
    {
        neighborId = j;
        int index_j;
        if((j-i+procnum)%procnum <= dgree/2)
        {
            index_j = ((j-i+procnum)%procnum-1)*2;
        }
        else if((i+procnum-j)%procnum <= dgree/2)
        {
            index_j = ((i+procnum-j)%procnum)*2-1;
        }
        else if(dgree%2 && (((i + procnum/2) % procnum) == j))
        {
            index_j = dgree-1;
        }
        else
        {
            cout<<"index_j error"<<endl;
        }
        
        //cout<<"index_j: "<<index_j<<" i,j: "<<i<<" "<<j<<endl;
        x_update();
        alpha_update(index_j);
        MPI_Status status;
        //send alpha to neighborhood
        //cout<<"before send"<<endl;
  
        MPI_Send(al[index_j], dim+1, MPI_DOUBLE, neighborId, 1, MPI_COMM_WORLD);
        //cout<<"after send"<<endl;
      
        MPI_Recv(msgbuf[index_j], dim+1, MPI_DOUBLE, neighborId, 1, MPI_COMM_WORLD,&status);  
        
    }
    else if(myid == j)
    {
        neighborId = i;
        int index_i;
        if((i-j+procnum)%procnum <= dgree/2)
        {
            index_i = ((i-j+procnum)%procnum-1)*2;
        }
        else if((j+procnum-i)%procnum <= dgree/2)
        {
            index_i = ((j+procnum-i)%procnum)*2-1;
        }
        else if(dgree%2 && (((j + procnum/2) % procnum) == i))
        {
            index_i = dgree-1;
        }
        else
        {
            cout<<"index_i error"<<endl;
        }
        
        //cout<<"index_i: "<<index_i<<" i,j: "<<i<<" "<<j<<endl;
        x_update();
        alpha_update(index_i);
        MPI_Status status;
        //send alpha to neighborhood
        //cout<<"before receive"<<endl;
        MPI_Recv(msgbuf[index_i], dim+1, MPI_DOUBLE, neighborId, 1, MPI_COMM_WORLD,&status);
    
        //cout<<"after receive"<<endl;  
    
        MPI_Send(al[index_i], dim+1, MPI_DOUBLE, neighborId, 1, MPI_COMM_WORLD);
       
       
    }
    else
    {
        return;
    }
    
}

void RandomizedADMM::train()
{
    int32_t k = 0;
    int32_t t = 0;
    clock_t  time_bwait,time_ewait;
	MPI_Barrier(MPI_COMM_WORLD);
    time_begin = clock();
    time_bwait = clock();
    if(myid == 0)
        printf("%3s %10s\n","#", "eps");
    wait_time = 0;
    while(k <= 5000)
    {
        if(myid == 0)
        {
            of << k <<"\t";
            printf("%3d",k);
            //random select (i,j)
            struct timespec tn;
            clock_gettime(CLOCK_REALTIME, &tn);

            srand(tn.tv_nsec);
            int i = rand() % procnum;
            int tmpj = rand() % dgree;
            int j = graph[i][tmpj];
            edge[0] = i; edge[1] = j;
        }
        MPI_Bcast(edge,2,MPI_INT,0,MPI_COMM_WORLD);
        update(edge[0],edge[1]);
        //cout<<"myid "<<myid<<" "<<"after update"<<endl;
        MPI_Barrier(MPI_COMM_WORLD);
        //cout<<"after barrier"<<endl;
        if(myid  == 0){
            if(is_stop())
            {
                break;
            }
        }
        k++;
    }
}



