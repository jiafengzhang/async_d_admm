/*************************************************************************
    > File Name: instanceData.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/

#include<iostream>
#include"instanceData.h"

using namespace std;

InstanceData::InstanceData(const char *filenameA,const char *filenameb)
{
    this->filenameA = filenameA; this->filenameb = filenameb;
    init();
    dataMat.resize(dataNum,dim);
    label.resize(dataNum);
    solution.resize(dim);
    loadData();
}
void InstanceData::init()
{
    stringstream ss;
    double val;
    ifstream in(filenameA);
    if(in)
    {
        string line;
        int m = 0, n = 0;
        while(getline(in,line))
        {
            if(m == 0)
            {
                ss.clear();
                ss << line;
                while(ss >> val)
                    n++;
            }
            m++;
        }
        dataNum = m;
        dim = n;
    }
    else
        cerr << filenameA << "doesn't exist!";
}
void InstanceData::loadData()
{
    stringstream ss;
    double val;
    ifstream matIn(filenameA);
    ifstream bIn(filenameb);
    ifstream solIn("../data/solution.dat");
    int m = 0, n;
    if(matIn && bIn && solIn)
    {
        string line;
        while(getline(matIn,line))
        {
            n = 0;
            ss.clear();
            ss << line;
            while(ss >> val)
            {
                dataMat(m,n) = val;
                if(m == 0)
                    solIn >> solution(n);
                n++;
            }
            bIn >> label(m);
            m++;
        }
    }
    else
        cerr << "file:lineardataA.dat, lineardatab.dat, solution.dat doesn't exist!";
}

int32_t InstanceData::getDataNum()
{
    return dataNum;
}
int32_t InstanceData::getDim()
{
    return dim;
}
