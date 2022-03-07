
#include "vec.h"
#include "mat.h"

typedef std::tuple<std::vector<double>,std::vector<int>,std::vector<int>> csr_data;


std::vector<double> getvecdata(vec &slvector);
void setvecdata(vec &slvector, std::vector<double> &values);

csr_data getacsrdata(mat &slmat);
void setacsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals);

csr_data getdcsrdata(mat &slmat);
void setdcsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals);
