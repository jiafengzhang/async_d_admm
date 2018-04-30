/*************************************************************************
    > File Name: conf_util.h
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#ifndef CONF_UTIL_H
#define CONF_UTIL_H
#include<iostream>
#include<map>
#include<string>
#include<string.h>

using namespace std;

class conf_util
{
public:
    conf_util();
    void parse(const string & conf_file);
    template<class T> T getItem(const string &item_name);
private:
    map<string, string> conf_items;
};

class args_t
{
public:
    args_t(int rank, int size);
    //process param
    int myid;
    int procnum;
    //dataset
    string data_dir;
    string out_dir;
    //async-model-param
    int s;
    int delta;
    int dgree;
    //admm
    double rho;
    double epsilon;
    bool adpstep;
	double stepsize;
    void get_args();
    void print_args();
};



#endif

