#include "user.h"

/*
 * Local functions
 */

/** 
 *  Extract the CSR data of the matrix to transfer it to python and 
 *  construct a scipy.csr_matrix
 *  
 * "Inspired" by PETSc4py function getValuesCSR:
 *  https://gitlab.com/petsc/petsc/-/blob/main/src/binding/petsc4py/src/PETSc/Mat.pyx#L911
 */
csr_data _getcsrdata(Mat pmat) {

    // row ownership
    int rstart = 0, rend=0, nrows=0;
    MatGetOwnershipRange(pmat, &rstart, &rend);
    nrows = rend - rstart;
    // first pass: row pointer array
    std::vector<int> ai(nrows+1);
    int irow = 0, ncols = 0;
    ai.at(0) = 0;
    for (int irow = 0; irow < nrows; irow++) {
        MatGetRow(pmat, irow+rstart, &ncols, NULL, NULL);
        ai.at(irow+1) = ai.at(irow) + ncols;
        MatRestoreRow(pmat, irow+rstart, &ncols, NULL, NULL);
    }
    // second pass: column indices and values
    std::vector<int> aj(ai.at(nrows));
    std::vector<double> av(ai.at(nrows));

    // yolo!
    int* paj = aj.data();
    double* pav = av.data();

    const int *cols = NULL;
    const double *vals = NULL;

    for (int irow = 0; irow < nrows; irow++) {
        MatGetRow(pmat, irow+rstart, &ncols, &cols, &vals);
        int air = ai.at(irow);
        // PetscMemcpy(paj+air, cols, <size_t>ncols*sizeof(int));
        // PetscMemcpy(pav+air, vals, <size_t>ncols*sizeof(double));
        PetscMemcpy(paj+air, cols, ncols*sizeof(int));
        PetscMemcpy(pav+air, vals, ncols*sizeof(double));
        MatRestoreRow(pmat, irow+rstart, &ncols, &cols, &vals);
    }

    // return as (data, indices, indptr)
    return {av, aj, ai};

}

/*
 *  Set data to PETSc matrix such that pmat[rowi[i], coli[i]] = vals[i]
 *  Inputs:
 *     rowi - row indices
 *     coli - column indices
 *     vals - values
 */
void _setcsrdata(Mat pmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals) {
    
    if(rowi.size() != coli.size() || coli.size() != vals.size()) {
        std::stringstream tmp;
        tmp << "The sizes of the row and column indices and the "
            << "value vector must match"; 
        throw std::runtime_error(tmp.str());
    }
        
    for(int i = 0; i < vals.size(); i++) {
        MatSetValue(pmat, rowi[i], coli[i], vals[i], INSERT_VALUES);
    }

    MatAssemblyBegin(pmat, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(pmat, MAT_FINAL_ASSEMBLY);
}


/** 
 * Get a copy of the data in the underlying PETSC vector of 
 * a sl::vec.  E.g. for python world.
 * FIXME: seems that this is not the right way to go for multiple 
 * processors but will do for now.
 */
std::vector<double> getvecdata(vec &slvector) {

    double *p = NULL;
    // Just read only access, no VecRestoreArray needed
    VecGetArray(slvector.getpetsc(), &p);
    std::vector<double> out(p, p+slvector.size());
    return out;
}

void setvecdata(vec &slvector, std::vector<double> &values) {
    if(slvector.size() != values.size()) {
        std::stringstream tmp;
        tmp << "The size of the sl::vec " << slvector.size() 
            << " and the size of the values " << values.size()
            << " don't match.";
        throw std::runtime_error(tmp.str());
    }
    double* p = NULL;
    VecGetArray(slvector.getpetsc(), &p);
    for (int i = 0; i < values.size(); i++) {
        p[i] = values[i];
    }
    VecRestoreArray(slvector.getpetsc(), &p);
}

csr_data getacsrdata(mat &slmat) {
    return _getcsrdata(slmat.getapetsc());
}

csr_data getdcsrdata(mat &slmat) {
    return _getcsrdata(slmat.getdpetsc());
}

void setacsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals) {
    _setcsrdata(slmat.getapetsc(), rowi, coli, vals);
}

void setdcsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals) {
    _setcsrdata(slmat.getdpetsc(), rowi, coli, vals);
}
