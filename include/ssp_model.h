/*************************************************************************
    > File Name: ssp_model.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-27
 ************************************************************************/
#ifndef SSP_MODEL_H
#define SSP_MODEL_H

#include"conf_util.h"
#include"instanceData.h"
#include<iostream>

using namespace std;

class sspmodel
{
public:
    sspmodel(args_t *args, InstanceData *ins);
    init();
    waitsome();
    waitall();
    send(int k);



};


#endif
