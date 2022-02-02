#include "vectorfieldselect.h"


void vectorfieldselect::setdata(int physreg, field myfield, std::string op)
{
    universe::getrawmesh()->getphysicalregions()->errorundefined({physreg});
    
    if (op != "set" && op != "add")
    {
        std::stringstream tmp;
        tmp  << "Error in 'vectorfieldselect' object: operation " << op << " is unknown in .setdata (use 'set' or 'add')" << std::endl;
        throw std::runtime_error(tmp.str());
    }
    
    (myfield.getpointer())->transferdata(physreg, vectorfieldselect(myrawvec, myrawfield), op);
}
