#include "mathop.h"


int mathop::regionunion(const std::vector<int> physregs)
{
    return (universe::mymesh->getphysicalregions())->createunion(physregs);
}

int mathop::regionintersection(const std::vector<int> physregs)
{
    return (universe::mymesh->getphysicalregions())->createintersection(physregs);
}

expression mathop::normal(int surfphysreg)
{
    int problemdimension = universe::mymesh->getmeshdimension();
    int elementdimension = universe::mymesh->getphysicalregions()->get(surfphysreg)->getelementdimension();
    
    if (problemdimension-1 != elementdimension || problemdimension == 1)
    {
        std::cout << "Error in 'mathop' namespace: can only compute the normal to a surface in 3D and to a line in 2D" << std::endl;
        abort();
    }
    
    expression expr;
    if (problemdimension == 2)
    {
        expression mynorm = sqrt(expr.invjac(0,1)*expr.invjac(0,1)+expr.invjac(1,1)*expr.invjac(1,1));
        mynorm.reuseit();
        return array2x1(expr.invjac(0,1), expr.invjac(1,1))/mynorm;
    }
    if (problemdimension == 3)
    {
        expression mynorm = sqrt(expr.invjac(0,2)*expr.invjac(0,2)+expr.invjac(1,2)*expr.invjac(1,2)+expr.invjac(2,2)*expr.invjac(2,2));
        mynorm.reuseit();
        return array3x1(expr.invjac(0,2), expr.invjac(1,2), expr.invjac(2,2))/mynorm;
    }
}
    
    
void mathop::setfundamentalfrequency(double f) { universe::fundamentalfrequency = f; }

expression mathop::t(void) { expression exp; return exp.time(); }

expression mathop::dx(expression input) { return input.spacederivative(1); }
expression mathop::dy(expression input) { return input.spacederivative(2); }
expression mathop::dz(expression input) { return input.spacederivative(3); }

expression mathop::dt(expression input) { return input.timederivative(1); }
expression mathop::dtdt(expression input) { return input.timederivative(2); }

expression mathop::sin(expression input) { return input.sin(); }
expression mathop::cos(expression input) { return input.cos(); }
expression mathop::abs(expression input) { return input.abs(); }
expression mathop::sqrt(expression input) { return pow(input, 0.5); }
expression mathop::log10(expression input) { return input.log10(); }
expression mathop::pow(expression base, expression exponent) { return base.pow(exponent); }

expression mathop::comp(int selectedcomp, expression input) 
{ 
    std::vector<expression> mycomp(input.countcolumns());
    for (int i = 0; i < input.countcolumns(); i++)
        mycomp[i] = input.getarrayentry(selectedcomp,i);
    return expression(1, input.countcolumns(), mycomp);
}

expression mathop::compx(expression input) { return comp(0,input); }
expression mathop::compy(expression input) { return comp(1,input); }
expression mathop::compz(expression input) { return comp(2,input); }

expression mathop::entry(int row, int col, expression input) { return input.getarrayentry(row,col); }

expression mathop::transpose(expression input) { return input.transpose(); }
expression mathop::inverse(expression input) { return input.invert(); }
expression mathop::determinant(expression input) { return input.determinant(); }

expression mathop::grad(expression input)
{
    if (input.countcolumns() != 1 || input.countrows() > 3)
    {
        std::cout << "Error in 'mathop' namespace: can only take the gradient of scalars and column vectors" << std::endl;
        abort();
    }

    int problemdimension = universe::mymesh->getmeshdimension();

    std::vector<expression> myexprs = {};
    for (int i = 0; i < problemdimension; i++)
    {
        for (int comp = 0; comp < input.countrows(); comp++)
            myexprs.push_back(input.spacederivative(i+1).getarrayentry(comp,0));
    }
    
    return expression(problemdimension, input.countrows(), myexprs);
}

expression mathop::curl(expression input)
{
    if (input.countcolumns() > 1)
    {
        std::cout << "Error in 'mathop' namespace: can only take the curl of a column vector" << std::endl;
        abort();
    }

    switch (input.countrows())
    {
        case 1:
            return expression(3,1,{0, 0, 0});
        case 2:
            return expression(3,1,{0, 0, compy(dx(input))-compx(dy(input))});
        case 3:
            return expression(3,1,{compz(dy(input))-compy(dz(input)), compx(dz(input))-compz(dx(input)), compy(dx(input))-compx(dy(input))});
    }
}

integration mathop::integral(int physreg, expression tointegrate, int integrationorderdelta, int blocknumber)
{
    return integration(physreg, tointegrate, integrationorderdelta, blocknumber);
}

integration mathop::integral(int physreg, expression meshdeform, expression tointegrate, int integrationorderdelta, int blocknumber)
{
    return integration(physreg, meshdeform, tointegrate, integrationorderdelta, blocknumber);
}

integration mathop::integral(int physreg, int numcoefharms, expression tointegrate, int integrationorderdelta, int blocknumber)
{
    return integration(physreg, numcoefharms, tointegrate, integrationorderdelta, blocknumber);
}

integration mathop::integral(int physreg, int numcoefharms, expression meshdeform, expression tointegrate, int integrationorderdelta, int blocknumber)
{
    return integration(physreg, numcoefharms, meshdeform, tointegrate, integrationorderdelta, blocknumber);
}

expression mathop::dof(expression input, int physreg) { return input.dof(physreg); }
expression mathop::tf(expression input, int physreg) { return input.tf(physreg); }



expression mathop::array1x1(expression term11) 
{
    return expression(1,1, {term11});
}

expression mathop::array1x2(expression term11, expression term12) 
{
    return expression(1,2, {term11, term12});
}

expression mathop::array1x3(expression term11, expression term12, expression term13) 
{
    return expression(1,3, {term11, term12, term13});
}

expression mathop::array2x1(expression term11, expression term21) 
{
    return expression(2,1, {term11, term21});
}

expression mathop::array2x2(expression term11, expression term12, expression term21, expression term22) 
{
    return expression(2,2, {term11, term12, term21, term22});
}

expression mathop::array2x3(expression term11, expression term12, expression term13, expression term21, expression term22, expression term23) 
{
    return expression(2,3, {term11, term12, term13, term21, term22, term23});
}

expression mathop::array3x1(expression term11, expression term21, expression term31) 
{
    return expression(3,1, {term11, term21, term31});
}

expression mathop::array3x2(expression term11, expression term12, expression term21, expression term22, expression term31, expression term32) 
{
    return expression(3,2, {term11, term12, term21, term22, term31, term32});
}

expression mathop::array3x3(expression term11, expression term12, expression term13, expression term21, expression term22, expression term23, expression term31, expression term32, expression term33) 
{
    return expression(3,3, {term11, term12, term13, term21, term22, term23, term31, term32, term33});
}
 
vec mathop::solve(mat A, vec b)
{
    if (A.getpointer() == NULL || b.getpointer() == NULL)
    {
        std::cout << "Error in 'mathop' namespace: direct solve of Ax = b failed (A or b is undefined)" << std::endl;
        abort();
    }

    Vec bpetsc = b.getpetsc();
    Mat Apetsc = A.getpetsc();

    vec sol = b;
    Vec solpetsc = sol.getpetsc();

////////// USE PETSC COMPILED WITHOUT DEBUG OPTION--> FASTER!!!!!!!!!!!!!!!!!! AND PETSC COMPILE WITH LOND INT TYPE FOR LARGE NNZ NUM!!!!!!!!
    
    KSP* ksp = A.getpointer()->getksp();
    
    if (A.getpointer()->isludefined() == false)
    {
        PC pc;
        KSPCreate(PETSC_COMM_WORLD, ksp);
        KSPSetOperators(*ksp, Apetsc, Apetsc);
        KSPSetFromOptions(*ksp);

        KSPGetPC(*ksp,&pc);
        PCSetType(pc,PCLU);
        PCFactorSetMatSolverPackage(pc,MATSOLVERMUMPS);
    }
    
    KSPSolve(*ksp, bpetsc, solpetsc);
   
    A.getpointer()->isludefined(true);
    
    if (A.getpointer()->islutobereused() == false)
    {
        KSPDestroy(ksp);
        A.getpointer()->isludefined(false);
    }
    
    return sol;
}



////////// PREDEFINED OPERATORS

expression mathop::m2d(expression input)
{
    if (input.countrows() != 2 || input.countcolumns() != 1)
    {
        std::cout << "Error in 'mathop' namespace: can only compute m2d of a 2x1 column vector" << std::endl;
        abort();
    }
    return expression(3,1,{dx(compx(input)), dy(compy(input)), dy(compx(input)) + dx(compy(input))});
}

expression mathop::m3dn(expression input)
{
    if (input.countrows() != 3 || input.countcolumns() != 1)
    {
        std::cout << "Error in 'mathop' namespace: can only compute m3dn of a 3x1 column vector" << std::endl;
        abort();
    }
    return expression(3,1,{dx(compx(input)), dy(compy(input)), dz(compz(input))});
}

expression mathop::m3ds(expression input)
{
    if (input.countrows() != 3 || input.countcolumns() != 1)
    {
        std::cout << "Error in 'mathop' namespace: can only compute m3ds of a 3x1 column vector" << std::endl;
        abort();
    }
    return expression(3,1,{dx(compy(input)) + dy(compx(input)), dz(compy(input)) + dy(compz(input)), dx(compz(input)) + dz(compx(input))});
}

////////// PREDEFINED FORMULATIONS

expression mathop::predefinedelasticity(expression u, expression E, expression nu)
{
    if (u.countrows() == 1)
    {
        std::cout << "Error in 'mathop' namespace: 'predefinedelasticity' is undefined in 1D" << std::endl;
        abort();
    }
    if (u.countrows() == 2)
        return - transpose(E/(1-pow(nu,2))*array3x3(1,nu,0,nu,1,0,0,0,0.5*(1-nu))*m2d(dof(u)))*m2d(tf(u));
    if (u.countrows() == 3)
        return - transpose(E/(1+nu)/(1-2*nu)*array3x3(1-nu,nu,nu,nu,1-nu,nu,nu,nu,1-nu)*m3dn(dof(u)))*m3dn(tf(u)) - transpose(E/(1+nu)/(1-2*nu)*array3x3(0.5*(1-2*nu),0,0,0,0.5*(1-2*nu),0,0,0,0.5*(1-2*nu))*m3ds(dof(u)))*m3ds(tf(u));
}

expression mathop::predefinedelectrostaticforce(expression gradtfu, expression gradv, expression epsilon)
{
    expression E = gradv;
    E.reuseit();
    
    if (gradtfu.countcolumns() == 1)
    {
        std::cout << "Error in 'mathop' namespace: 'predefinedelectrostaticforce' is undefined for 1D displacements" << std::endl;
        abort();
    }
    if (gradtfu.countcolumns() == 2)
        return -( epsilon*0.5 * (pow(compx(E),2) * entry(0,0,gradtfu) - pow(compy(E),2) * entry(0,0,gradtfu) + 2 * compx(E) * compy(E) * entry(1,0,gradtfu))      +epsilon*0.5 * (-pow(compx(E),2) * entry(1,1,gradtfu) + pow(compy(E),2) * entry(1,1,gradtfu) + 2 * compy(E) * compx(E) * entry(0,1,gradtfu)) );
    if (gradtfu.countcolumns() == 3)
        return -( epsilon*0.5 * (pow(compx(E),2) * entry(0,0,gradtfu) - pow(compy(E),2) * entry(0,0,gradtfu) - pow(compz(E),2) * entry(0,0,gradtfu) + 2 * compx(E) * compy(E) * entry(1,0,gradtfu) + 2 * compx(E) * compz(E) * entry(2,0,gradtfu))      +epsilon*0.5 * (-pow(compx(E),2) * entry(1,1,gradtfu) + pow(compy(E),2) * entry(1,1,gradtfu) - pow(compz(E),2) * entry(1,1,gradtfu) + 2 * compy(E) * compx(E) * entry(0,1,gradtfu) + 2 * compy(E) * compz(E) * entry(2,1,gradtfu))      +epsilon*0.5 * (-pow(compx(E),2) * entry(2,2,gradtfu) - pow(compy(E),2) * entry(2,2,gradtfu) + pow(compz(E),2) * entry(2,2,gradtfu) + 2 * compz(E) * compx(E) * entry(0,2,gradtfu) + 2 * compz(E) * compy(E) * entry(1,2,gradtfu)) );
}

