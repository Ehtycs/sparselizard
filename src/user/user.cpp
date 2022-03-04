#include "user.h"

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

/** 
 *  Extract the CSR data of the matrix to transfer it to python and 
 *  construct a scipy.csr_matrix
 *  
 * "Inspired" by PETSc4py function getValuesCSR:
 *  https://gitlab.com/petsc/petsc/-/blob/main/src/binding/petsc4py/src/PETSc/Mat.pyx#L911
 */
csr_data getcsrdata(mat &slmat) {

    Mat pmat = slmat.getapetsc();

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

    return {ai, aj, av};

}

void setcsrdata(mat &slmat, std::vector<int> &rowi, 
                std::vector<int> &coli, std::vector<double> &vals) {

    std::cout << "I'm here" << std::endl;

}
