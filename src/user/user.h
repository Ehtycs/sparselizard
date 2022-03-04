
#include "vec.h"
#include "mat.h"

typedef std::tuple<std::vector<int>,std::vector<int>,std::vector<double>> csr_data;


std::vector<double> getvecdata(vec &slvector);
void setvecdata(vec &slvector, std::vector<double> &values);

csr_data getcsrdata(mat &slmat);
void setcsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals);
