#include "hcurlpyramid.h"

using namespace std;


int hcurlpyramid::count(int order)
{
    std::cout << "Error in 'hcurlpyramid' object: shape functions not defined for pyramids" << std::endl;
    throw std::runtime_error("");  
}

int hcurlpyramid::count(int order, int dim, int num)
{
    std::cout << "Error in 'hcurlpyramid' object: shape functions not defined for pyramids" << std::endl;
    throw std::runtime_error("");  
}



hierarchicalformfunctioncontainer hcurlpyramid::evalat(int maxorder) 
{    
    std::cout << "Error in 'hcurlpyramid' object: shape functions not defined for pyramids" << std::endl;
    throw std::runtime_error("");  
}
