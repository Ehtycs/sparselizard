#include "gppyramid.h"

int gppyramid::count(int integrationorder)
{
    std::stringstream tmp;
    tmp  << "Error in 'gppyramid' namespace: Gauss points have not been defined yet for pyramids" << std::endl;
    throw std::runtime_error(tmp.str());
}

void gppyramid::set(int integrationorder, std::vector<double>& coordinates, std::vector<double>& weights)
{
    std::stringstream tmp;
    tmp  << "Error in 'gppyramid' namespace: Gauss points have not been defined yet for pyramids" << std::endl;
    throw std::runtime_error(tmp.str());
}




