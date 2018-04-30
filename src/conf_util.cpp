/*************************************************************************
    > File Name: conf_util.cpp
    > Author: Zhang Jiafeng
    > Mail: 761369516@qq.com
    > Created Time: 2017-11-21
 ************************************************************************/
#include"conf_util.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;

conf_util::conf_util()
{
    string conf_file = "../conf/admm.conf";
    parse(conf_file);
}
void conf_util::parse(const string &conf_file)
{
    ifstream confIn(conf_file.c_str());
    string line;
    vector<string> vitems;
    while(getline(confIn, line))
    {
        vitems.clear();
        if(line.empty() || line[0] == '#')
            continue;
		const int len = line.length();
		char s[len+1];
		strcpy(s,line.c_str());
		//char *delim = "=";
        char *pos = strtok(s," =");
        int32_t k = 0;
        while(pos != NULL)
        {
            vitems.push_back(pos);
            pos = strtok(NULL, "=");
            k++;
        }
        if(k!=2)
        {
            cout<<"args conf error!"<<endl;
            exit(0) ;
        }
        conf_items.insert({vitems[0],vitems[1]});
    }
}
template<class T> T conf_util::getItem(const std::string &item_name)
{
    stringstream sitem;
    T result;
    sitem << conf_items[item_name];
    sitem >> result;
    return result;
}

args_t::args_t(int rank, int size)
{
    myid = rank;
    procnum = size;
    data_dir = "../data";
    out_dir = "../data";
    s = 0;
    delta = 0;
    rho = 0.0;
    epsilon = 0.0;
    adpstep = false;
}

void args_t::get_args()
{
    conf_util admm_conf;
    data_dir = admm_conf.getItem<string>("data_dir");
    out_dir = admm_conf.getItem<string>("out_dir");
    dgree = admm_conf.getItem<int>("dgree");
    s = admm_conf.getItem<int>("s");
    delta = admm_conf.getItem<int>("delta");
    rho = admm_conf.getItem<double>("rho");
    epsilon = admm_conf.getItem<double>("epsilon");
    string sstep = admm_conf.getItem<string>("adpstep");
    if(sstep=="true")
        adpstep = true;
	stepsize = admm_conf.getItem<double>("stepsize");
}
void args_t::print_args()
{
    cout << "#************************configuration***********************";
    cout<<endl;
    cout << "#Number of processors: "<<procnum<<endl;
    cout << "#Data dirent: "<<data_dir<<endl;
    cout << "#Out dirent: "<<out_dir<<endl;
    cout << "#s: "<<s<<endl;
    cout << "#delta: "<<delta<<endl;
    cout << "#dgree: "<<dgree<<endl;
    cout << "#rho: "<<rho<<endl;
    cout << "#epsilon: "<<epsilon<<endl;
    cout << "#adpstep: "<<adpstep<<endl;
    cout << "#************************************************************";
    cout<<endl;
}
