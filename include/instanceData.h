/*************************************************************************
    > File Name: instanceData.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include<iostream>
#include<Eigen/Dense>
#include<vector>
#include<cmath>
#include<fstream>
#include<sstream>
using namespace std;
using namespace Eigen;

class InstanceData{
	public:
		InstanceData(const char *filename, const char *filenameb);
	//	~InstanceData();
	    void init();
	    void loadData();
		int32_t getDataNum();
		int32_t getDim();
		MatrixXd dataMat;
		VectorXd label;
		VectorXd solution;
    private:
        uint32_t dataNum;
		uint32_t dim;
		const char *filenameA, *filenameb;
};


#endif
