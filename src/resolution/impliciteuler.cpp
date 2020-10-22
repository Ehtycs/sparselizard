#include "impliciteuler.h"

impliciteuler::impliciteuler(formulation formul, vec dtxinit, int verbosity, std::vector<bool> isrhskcconstant)
{
    myverbosity = verbosity;

    myformulation = formul;
    
    dtx = dtxinit;
    
    if (isrhskcconstant.size() == 0)
        isconstant = {false,false,false};
    else
        isconstant = isrhskcconstant;
    if (isconstant.size() != 3)
    {
        std::cout << "Error in 'impliciteuler' object: expected a length 3 or empty vector as third argument" << std::endl;
        abort();  
    }
}

void impliciteuler::settimederivative(vec sol)
{
    dtx = sol;
}

void impliciteuler::presolve(std::vector<formulation> formuls) { tosolvebefore = formuls; }
void impliciteuler::postsolve(std::vector<formulation> formuls) { tosolveafter = formuls; }

void impliciteuler::runlinear(double timestep)
{
    run(true, timestep, -1);
}

int impliciteuler::runnonlinear(double timestep, int maxnumnlit)
{
    return run(false, timestep, maxnumnlit);
}

int impliciteuler::run(bool islinear, double timestep, int maxnumnlit)
{
    dt = timestep;
    // Update and print the time:
    universe::currenttimestep += dt;
    char spacer = ':';
    if (islinear || myverbosity < 2)
        spacer = ' ';
    if (myverbosity > 0)
        std::cout << "@" << universe::currenttimestep << "s" << spacer << std::flush;
    
    // Make all time derivatives available in the universe:
    universe::xdtxdtdtx = {{},{dtx},{}};
        
    // Get the data from all fields to create the x vector:
    vec x(myformulation);
    x.setdata();
    
    // Nonlinear loop:
    double relchange = 1; int nlit = 0;
    vec xnext = x, dtxnext = dtx;
    while (relchange > nltol && (maxnumnlit <= 0 || nlit < maxnumnlit))
    {
        // Solve all formulations that must be solved at the beginning of the nonlinear loop:
        mathop::solve(tosolvebefore);
        
        vec xtolcalc = xnext;
        
        // Reassemble only the non-constant matrices:
        bool isfirstcall = not(K.isdefined());
        if (isconstant[1] == false || isfirstcall)
        {
            myformulation.generatestiffnessmatrix();
            K = myformulation.K(false, false);
        }
        if (isconstant[2] == false || isfirstcall)
        {
            myformulation.generatedampingmatrix();
            C = myformulation.C(false, true);
        }
        if (isconstant[0] == false || isfirstcall)
        {
            myformulation.generaterhs();
            rhs = myformulation.rhs();
        }
        else
            rhs.updateconstraints();
        
        // Reuse matrices when possible (including the factorization):
        if (isconstant[1] == false || isconstant[2] == false || isfirstcall || defdt != dt)
        {
            leftmat = C + dt*K;
            leftmat.reusefactorization();
            
            defdt = dt;
        }
        
        // Update the solution xnext.
        xnext = relaxationfactor * mathop::solve(leftmat, C*x+dt*rhs) + (1.0-relaxationfactor)*xnext;
        
        dtxnext = 1.0/dt*(xnext-x);
        
        // Update all fields in the formulation:
        mathop::setdata(xnext);
        
        relchange = (xnext-xtolcalc).norm()/xnext.norm();
        
        if (islinear == false && myverbosity > 1)
            std::cout << " " << relchange << std::flush;

        nlit++; 
        
        // Solve all formulations that must be solved at the end of the nonlinear loop:
        mathop::solve(tosolveafter);
        
        // Make all time derivatives available in the universe:
        universe::xdtxdtdtx = {{},{dtxnext},{}};
        
        if (islinear)
            break;
    }
    
    dtx = dtxnext;
    
    if (myverbosity > 1 && islinear == false)
        std::cout << " (" << nlit << "NL it) " << std::flush;
    
    return nlit;
}


