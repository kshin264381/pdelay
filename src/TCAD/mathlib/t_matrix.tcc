/**
 * t_matrix.cc
 *
 * 	Implementation for t_matrix class
 * 	defined at t_matrix.h...
 *
 *  Created on: Mar 22, 2016
 *      Author: Taylor Shin
**/

// use uBlas if cygwin...
#if defined(__CYGWIN__) && !defined(_WIN32)

/**
 * t_matrix: private stuff
 *
**/
// Matrix inversion --> Adopted from
// http://www.crystalclearsoftware.com/cgi-bin/boost_wiki/wiki.pl?LU_Matrix_Inversion
template <class T>
matrix<T> t_matrix<T>::matrix_inverse()
{
    if (!this->isSquare()) {
        std::cerr << \
            "t_matrix: Cannot perform matrix inversion \
            on non square matrix" << \
            std::endl;
        return matrix<T>();
        exit(-1);
    }
    else {
        matrix<T> inv_matrix = \
            identity_matrix<T>(this->matrix_data->size1());
        matrix<T> L;
        matrix<T> U;
        this->LU(L, U);
        lu_substitute(L, U, inv_matrix);

        return inv_matrix;
    }
}

/**
 * t_matrix: public stuff
 *
**/
/**
 * Operator overloading stuff
 *
**/
template <class T>
t_matrix<T>& t_matrix<T>::operator=(const t_matrix<T>& rhs)
{
    return t_matrix<T>(rhs.GetMatrix());
}

template <class T>
t_matrix<T>& t_matrix<T>::operator=(const matrix<T>& rhs)
{
    return t_matrix<T>(rhs);
}

template <class T>
t_matrix<T>& t_matrix<T>::operator+(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator+(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator+(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator+=(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator+=(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator+=(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix + rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-=(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-=(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator-=(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix - rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*=(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs.GetMatrix();
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*=(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator*=(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix * rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator/(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix / rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
t_matrix<T>& t_matrix<T>::operator/=(const T& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    this_matrix = this_matrix / rhs;
    return t_matrix<T>(this_matrix);
}
template <class T>
bool t_matrix<T>::operator==(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    matrix<T> other_matrix = rhs.GetMatrix();
    return (this_matrix == other_matrix);
}
template <class T>
bool t_matrix<T>::operator==(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    return (this_matrix == rhs);
}
template <class T>
bool t_matrix<T>::operator!=(const t_matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    matrix<T> other_matrix = rhs.GetMatrix();
    return (this_matrix != other_matrix);
}
template <class T>
bool t_matrix<T>::operator!=(const matrix<T>& rhs)
{
    matrix<T> this_matrix = this->GetMatrix();
    return (this_matrix != rhs);
}


/**
 * Mathematical operations
 *
**/
// Inverse it!!
template <class T>
void t_matrix<T>::Inv()
{
    this->matrix_data = \
        std::make_unique(this->matrix_inverse());
}

// Inverse and return it
template <class T>
t_matrix<T> t_matrix<T>::GetInv()
{
    return this->matrix_inverse();
}

// Transpose self
template <class T>
void t_matrix<T>::Tran()
{
    this->matrix_data = \
        std::make_unique(trans((*this->matrix_data)));
}

// Transpose and return
template <class T>
t_matrix<T> t_matrix<T>::GetTran()
{
    return t_matrix<T>(trans((*this->matrix_data)));
}

// Calculate determinent
template <class T>
T t_matrix<T>::Det()
{
    matrix<T> L;
    matrix<T> U;
    this->LU(L, U);

    T det = static_cast<T>(1);
    #pragma omp parallel
    {
        auto pivot_size = U.Size().x;

        #pragma omp for
        for (auto i=0; i<pivot_size; i++) {
            auto det_tmp = static_cast<T>(1);
            if (!fp_equal(U(i), static_cast<T>(i))) {
                det_tmp *= static_cast<T>(-1);
            }
            det_tmp *= L(i,i);

            #pragma omp critical
            {
                det *= det_tmp;
            }
        }
    }
    return det;
}


// LU factorization
template <class T>
void t_matrix<T>::LU(t_matrix<T>& L, t_matrix<T>& U)
{
    auto orig_matrix = (*this->matrix_data);
    permutation_matrix<std::size_t> permu_matrix(orig_matrix.size1());

    auto res = lu_factorize(orig_matrix, permu_matrix);
    if (!res) {
        std::cerr << \
            "t_matrix: LU decomposition failed!! residual: " << \
            res << " spotted!!" << std::endl;
        exit(-1);
    }
    else {
        L.SetMatrix(orig_matrix);
        U.SetMatrix(permu_matrix);
    }
}

/**
 * Utility operations
 *
**/
// Returns matrix size as a tuple
//
// tuple.x --> row size
// tuple.y --> column size
//
template <class T>
t_m_size t_matrix<T>::Size()
{
    auto rows = \
        static_cast<uint64_t>(this->matrix_data->size1());
    auto columns = \
        static_cast<uint64_t>(this->matrix_data->size2());
    return t_m_size{ rows, columns };
}

// Returns this->matrix_data to the matrix datatype.
template <class T>
matrix<T> t_matrix<T>::GetMatrix()
{
    return *this->matrix_data;
}

// Check if current matrix is square or not
template <class T>
bool t_matrix<T>::isSquare()
{
    return (this->matrix_data->size1() == this->matrix_data->size2());
}

// Manually assign matrix_data
template <class T>
void t_matrix<T>::SetMatrix(const matrix<T>& input_matrix)
{
    this->matrix_data = std::make_unique(input_matrix);
}

/**
 * Constructors and Destructors
 *
**/
// Dummy constructor
template <class T>
t_matrix<T>::t_matrix()
{
    this->matrix_data = \
        std::unique_ptr<T>(matrix<T>(1,1));
}

// row and col constructor
template <class T>
t_matrix<T>::t_matrix(const uint64_t& row, const uint64_t& col)
{
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(
            zero_matrix<T>(row, col));
}

// Square constructor
template <class T>
t_matrix<T>::t_matrix(const uint64_t& N)
{
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(
            zero_matrix<T>(N, N));
}

// Square unit matrix constructor with multiplier
template <class T>
t_matrix<T>::t_matrix(const uint64_t& N, const T& init_value)
{
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(
            identity_matrix<T>(N, N)*init_value);
}

// Initialize with the ublas::matrix
template <class T>
t_matrix<T>::t_matrix(const matrix<T>& Matrix)
{
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(Matrix);
}

// Initialize with 2D vector
template <class T>
t_matrix<T>::t_matrix(std::vector<std::vector<T>> two_D_vec)
{
    matrix<T> tempMatrix();
    // obtain size info
    auto row_size = two_D_vec.size();
    auto col_size = two_D_vec[0].size();
    tempMatrix.resize(row_size, col_size);

    // Now assign vector stuff to matrix.
    for (auto i=0; i<row_size; i++) {
        for (auto j=0; j<col_size; j++) {
            tempMatrix.insert_element(i+1, j+1, two_D_vec[i][j]);
        }
    }

    // Finally, assign the tempMatrix to the class
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(tempMatrix);
}

// Initialize with 2D array of type T
template <class T>
t_matrix<T>::t_matrix(const T* two_D_arr[], const uint64_t& rows, const uint64_t& cols)
{
    matrix<T> tempMatrix(rows, cols);
    tempMatrix = make_matrix_from_pointer(two_D_arr);
    this->matrix_data = \
        std::unique_ptr<matrix<T>>(tempMatrix);
}



#else
// rely on InteMKL for MSVC or pure linux environment.




#endif /* #if defined(__CYGWIN__) && !defined(_WIN32) */

