/*************************************************************************
    > File Name: test.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com 
    > Created Time: 2017-11-21
 ************************************************************************/

#include<iostream>
#include"../include/conf_util.h"
using namespace std;
int main()
{
    args_t args(0,4);
    args.get_args();
    args.print_args();
}

