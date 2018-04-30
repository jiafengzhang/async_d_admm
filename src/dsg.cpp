#include"dsg.h"

DSG::DSG(args_t *args,InstanceData *ins1)
{
    ins = ins1;
    dataNum = ins->getDataNum(); dim = ins->getDim(); dgree = args->dgree;
    myid = args->myid; procnum = args->procnum;
    x_i.setZero(dim); wx.setZero(dim);
	rho = args->rho; ABSTOL = args->epsilon;
    outfile = new char[100];
    sprintf(outfile,"../out/dsgresult_%f.dat",rho);
    
    if(myid ==0)
        of.open(outfile);
    msgbuf = new double*[dgree];x = new double[dim]; x_sum = new double[dim];
    lemmo = ins->dataMat.transpose() * ins->dataMat;
    Atb = ins->dataMat.transpose() * ins->label;
    cout<<"Atb norm"<<Atb.norm()<<endl;
    for(int i = 0; i < dim; i++)
        x[i] = 0;
    for(int32_t i = 0; i < dgree; i++)
    {
        msgbuf[i] = new double[dim];
        for(int32_t j = 0; j < dim; j++)
        {
            msgbuf[i][j] = 0;
        }
    }
    recvRequests = new MPI_Request[dgree];
    sendRequests = new MPI_Request[dgree];
    statuses = new MPI_Status[dgree];
    sendstatus = new MPI_Status[dgree];
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
        MPI_Recv_init(msgbuf[i], dim, MPI_DOUBLE, neighbor[i] ,1, MPI_COMM_WORLD, &recvRequests[i]);
        MPI_Send_init(x, dim, MPI_DOUBLE, neighbor[i] ,1, MPI_COMM_WORLD, &sendRequests[i]);
    }

}
DSG::~DSG()
{
    for(int32_t i = 0; i < procnum; i++)
   		{
			delete []msgbuf[i];
    	}
    delete []msgbuf;  delete []x;
}

void DSG::x_update(int k)
{

    //cout<<"AtAx "<<(lemmo * x_i - Atb).norm()<<endl;

    //MPI_Allreduce(x,x_sum,dim,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);
    wx.setZero(dim);
    for(int i = 0; i < dgree; i++)
    {
        for(int j = 0; j < dim; j++)
            wx(j) += msgbuf[i][j];
    }
    wx += x_i;
    wx /= dgree+1;
    double val_rho =rho * 0.01 / pow(k,1.0/3);
    VectorXd grade = lemmo * x_i - Atb;
    for(int i = 0; i < dim; i++)
    {
        grade(i) *= val_rho;
        //cout<<grade(i)<<endl;
    }
    //cout<<"grade norm"<<grade.norm();
    x_i = wx - grade;
    for(int i = 0; i < dim; i++)
        x[i] = x_i(i);
}

bool DSG::is_stop()
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

void DSG::train()
{
    int32_t k = 1;
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Startall(dgree, recvRequests);
    MPI_Startall(dgree, sendRequests);
    time_begin = clock();
    wait_time = 0;
    while(k < 5000)
    {
        if(myid ==0)
        {
            printf("%3d",k);
            of << k <<"\t";
        }
        MPI_Waitall(dgree, recvRequests, statuses);
        x_update(k);
        MPI_Waitall(dgree, sendRequests, sendstatus);
        MPI_Startall(dgree, recvRequests);
        MPI_Startall(dgree, sendRequests);
        if(myid == 0)
            if(is_stop())
            {
                cout<<myid<<" return"<<endl;
                return;
            }

        k++;
    }
}




















