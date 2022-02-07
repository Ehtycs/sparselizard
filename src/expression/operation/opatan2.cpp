#include "opatan2.h"


std::vector<std::vector<densemat>> opatan2::interpolate(elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform)
{
    // Get the value from the universe if available and reuse is enabled:
    if (reuse && universe::isreuseallowed)
    {
        int precomputedindex = universe::getindexofprecomputedvalue(shared_from_this());
        if (precomputedindex >= 0) { return universe::getprecomputed(precomputedindex); }
    }
    
    std::vector<std::vector<densemat>> arg1mat = myargs.at(0)->interpolate(elemselect, evaluationcoordinates, meshdeform);
    std::vector<std::vector<densemat>> arg2mat = myargs.at(1)->interpolate(elemselect, evaluationcoordinates, meshdeform);
    
    if (arg1mat.size() == 2 && arg1mat[1].size() == 1)
    {
        arg1mat[1][0].atan2(arg2mat[1][0]);
        
        if (reuse && universe::isreuseallowed)
            universe::setprecomputed(shared_from_this(), arg1mat);
        
        return arg1mat;
    }

    std::stringstream tmp;

    tmp  << "Error in 'opatan2' object: without FFT atan() can only be computed for constant (harmonic 1) operations" << std::endl;

    throw std::runtime_error(tmp.str());
}

densemat opatan2::multiharmonicinterpolate(int numtimeevals, elementselector& elemselect, std::vector<double>& evaluationcoordinates, expression* meshdeform)
{
    // Get the value from the universe if available and reuse is enabled:
    if (reuse && universe::isreuseallowed)
    {
        int precomputedindex = universe::getindexofprecomputedvaluefft(shared_from_this());
        if (precomputedindex >= 0) { return universe::getprecomputedfft(precomputedindex); }
    }
    
    densemat output1 = myargs.at(0)->multiharmonicinterpolate(numtimeevals, elemselect, evaluationcoordinates, meshdeform);
    densemat output2 = myargs.at(1)->multiharmonicinterpolate(numtimeevals, elemselect, evaluationcoordinates, meshdeform);
    output1.atan2(output2);
            
    if (reuse && universe::isreuseallowed)
        universe::setprecomputedfft(shared_from_this(), output1);
    
    return output1;
}

std::shared_ptr<operation> opatan2::simplify(std::vector<int> disjregs)
{
    std::vector<std::shared_ptr<operation>> newmyargs;
    for(const auto& x: myargs) {
        newmyargs.push_back(x->simplify(disjregs));
    }

    myargs = newmyargs;

    if (myargs.at(0)->isconstant() && myargs.at(1)->isconstant())
        return std::shared_ptr<operation>(new opconstant(std::atan2(myargs.at(0)->getvalue(), myargs.at(1)->getvalue())));
    else
        return shared_from_this();
}

std::shared_ptr<operation> opatan2::copy(void)
{
    std::shared_ptr<opatan2> op(new opatan2(myargs));
    *op = *this;
    op->reuse = false;
    return op;
}

double opatan2::evaluate(void)
{
    return std::atan2(myargs.at(0)->evaluate(), myargs.at(1)->evaluate());
}

std::vector<double> opatan2::evaluate(std::vector<double>& xcoords, std::vector<double>& ycoords, std::vector<double>& zcoords)
{
    std::vector<double> evaluated1 = myargs.at(0)->evaluate(xcoords, ycoords, zcoords);
    std::vector<double> evaluated2 = myargs.at(1)->evaluate(xcoords, ycoords, zcoords);
    for (int i = 0; i < evaluated1.size(); i++)
        evaluated1[i] = std::atan2(evaluated1[i], evaluated2[i]);
    return evaluated1;
}

void opatan2::print(void)
{
    std::cout << "atan2(";
    myargs.at(0)->print();
    std::cout << ",";
    myargs.at(1)->print();
    std::cout << ")";
}
