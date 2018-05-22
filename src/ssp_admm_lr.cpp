/*************************************************************************
    > File Name: ssp_admm_lr.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/

#include<iostream>

#include"ssp_admm_lr.h"

using namespace std;

ADMM::ADMM(args_t *args, InstanceData *ins1)
{
    ins = ins1;
    dataNum = ins->getDataNum(); dim = ins->getDim();
    myid = args->myid; procnum = args->procnum; dgree = args->dgree;
    s = args->s; delta = args->delta; stepsize = args->stepsize;
    rho = args->rho; ABSTOL = args->epsilon; adpstep = args->adpstep;
	if(!adpstep)
		stepsize = 0;
    alpha.setZero(dim); x_i.setZero(dim);
    outfile = new char[100];
    sprintf(outfile,"../out/admmresult_%d_%d_%d_rho.dat",s,delta,adpstep);
    if(myid ==0)
        of.open(outfile);
	of<<"stepsize:"<<stepsize<<endl;
    msgbuf = new double*[dgree]; al = new double*[dgree];
    step = new double[dgree];
    delay = new int[dgree];
    
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
        step[i] = 1;delay[i] = 1;
    }
    recvRequests = new MPI_Request[dgree];
    sendRequests = new MPI_Request[dgree];
    statuses = new MPI_Status[dgree];
    sendstatus = new MPI_Status[dgree];
    indices = new int[dgree*delta]; a = new int[dgree*delta]; index = new int[dgree*delta];
    neighbor = new int[dgree];
    for(int i = 0; i < dgree / 2; i++)
    {
        neighbor[2*i] = (myid+i+1) % procnum;
        neighbor[2*i+1]  = (myid - (i+1) + procnum) % procnum;
    }
    if(dgree % 2)
        neighbor[dgree-1] = (myid + procnum/2) % procnum;
    for (int i = 0; i < dgree; i++)
    {
        MPI_Recv_init(msgbuf[i], dim+1, MPI_DOUBLE, neighbor[i] ,1, MPI_COMM_WORLD, &recvRequests[i]);
        MPI_Send_init(al[i], dim+1, MPI_DOUBLE, neighbor[i] ,1, MPI_COMM_WORLD, &sendRequests[i]);
    }
    if(myid == 0)
    {
        cout<<"neighborhood ";
        for(int i = 0; i < dgree; i++)
            cout<<neighbor[i]<<"  ";
        cout<<endl;
    }
    commtime = 0;

}
ADMM::~ADMM()
{
	for(int32_t i = 0; i < dgree; i++)
   		{
			delete []msgbuf[i];
			delete []al[i];
    	}
    delete []msgbuf;
    delete []al;
    delete []delay; delete []step; delete []outfile; delete []recvRequests;delete []sendstatus;
    delete []sendRequests; delete []statuses; delete []indices; delete []a; delete []index;
    alpha.setZero(dim); x_i.setZero(dim);
}
int ADMM::miniter(double ** msgbuf, int num)
{
    int minclk = (int)msgbuf[0][dim];
    for(int i = 0; i < num; i++)
    {
        if((int)msgbuf[i][dim] < minclk)
            minclk = (int)msgbuf[i][dim];
    }
    return minclk;
}

void ADMM::x_update()
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

}
void ADMM::alpha_update()
{
    if(!adpstep)
    {
        for(int32_t j = 0; j < dim; j++)
        {
            for(int32_t i = 0; i < sum; i++)
            {
                al[index[i]][j] += -1*(0.5*(al[index[i]][j] + msgbuf[index[i]][j]) + rho * x_i(j));
            }
        }
    }
    else
    {
        //int mindelay = miniter(msgbuf, procnum);
        for(int32_t i = 0; i < sum; i++)
        {
            //int delay = msgbuf[a[i]][dim] - msgbuf[myid][dim];
            //double step = (double)(delta + delay)/delta;
            for(int32_t j = 0; j < dim; j++)
            {
                al[index[i]][j] += -step[index[i]] * (0.5*(al[index[i]][j] + msgbuf[index[i]][j]) + rho * x_i(j));
            }
        }
        for(int32_t i = 0; i < dgree; i++)
        {
	    //double tmpsize = (0.75*s)/procnum;
            step[i] += pow(stepsize,delay[i]);
			//if(step[i] >= 2)
				//step[i] = 2;
            delay[i]++;
        }
        for(int32_t i = 0; i < sum; i++)
        {
            step[index[i]] = 1;
            delay[index[i]] = 1;
        }
    }

}

bool ADMM::is_stop()
{
    VectorXd tmp = x_i - ins->solution;
    double eps = tmp.norm() / ins->solution.norm();
    time_now = clock();
    double sum_time = (double)(time_now-time_begin)/CLOCKS_PER_SEC;
    if(myid == 0)
    {
        of <<eps<<"\t"<< sum_time<<"\t"<<wait_time<<endl;
        printf("%10.4f  %.10f\t", x_i.norm(), eps);
        for(int i = 0; i < sum; i++)
        {
            cout<<a[i]<<"\t";
        }
		cout<<endl;
    }
    if(eps < ABSTOL)
        return true;
    else
        return false;
}
void ADMM::stop()
{
    //waitall
    MPI_Waitall(dgree, recvRequests, statuses);
    //sendall
    MPI_Waitall(dgree, sendRequests, sendstatus);
    MPI_Startall(dgree, sendRequests);
    //waitall(send)
    //MPI_Waitall(dgree, sendRequests, sendstatus);
    return ;
}
void ADMM::train()
{
    int32_t k = 0;int number_of_completed_operation = 0;
    int32_t t = 0;
    clock_t  time_bwait,time_ewait;
	MPI_Barrier(MPI_COMM_WORLD);
    time_begin = clock();

    MPI_Startall(dgree, recvRequests);
    //x_update();
    MPI_Startall(dgree, sendRequests);
	time_bwait = clock();
    //MPI_Waitall(dgree, sendRequests, sendstatus);
    if(myid == 0)
        printf("%3s %10s\n","#", "eps");
    wait_time = 0;
    int minclk = -1;
    while(k <= 5000)
    {
        sum = 0;
        if(myid ==0)
        {
            of << k <<"\t";
            printf("%3d",k);
        }
        while(sum < s || (k - minclk) >= delta)
        {


            MPI_Waitsome(dgree, recvRequests, &number_of_completed_operation, indices, statuses);
            minclk = miniter(msgbuf,dgree);
            if(minclk == -2)
            {
                //cout<<myid<<" break"<<endl;
                for(int i = 0; i < dgree; i++)
                {
                    al[i][dim] = -2;
                }
                stop();
                return;
            }
            if(number_of_completed_operation > 0)
            {
                for(int i = 0; i < number_of_completed_operation; i++)
                {
                    a[sum] = statuses[i].MPI_SOURCE;
                    //MPI_Start(&recvRequests[indices[i]]);
                    index[sum] = indices[i];
                    sum++;
                }

            }

        }
        time_ewait = clock();
        wait_time += (double)(time_ewait-time_bwait)/CLOCKS_PER_SEC;
        commtime += sum;
        x_update(); //x_update();
        alpha_update();

        if(myid == 0)
        {
            if(is_stop())
            {
                for(int i = 0; i < dgree; i++)
                {
                    al[i][dim] = -2;
                }
                stop();
                return;
            }
        }

		time_bwait = clock();
        for(int i = 0; i < sum; i++)
        {

            MPI_Wait(&sendRequests[index[i]], &sendstatus[index[i]]);
            al[index[i]][dim] = k+1;
            MPI_Start(&recvRequests[index[i]]);
            MPI_Start(&sendRequests[index[i]]);
            //MPI_Send(al[a[i]], dim+1, MPI_DOUBLE, a[i], 1, MPI_COMM_WORLD);
        }
        k++;
    }
    stop();
}
