#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <iostream>

template <typename T>
class Matrix {
private:
    size_t rows, cols;
    T *data;
public:
    Matrix(size_t rows, size_t cols);
    Matrix(const Matrix &m);
    ~Matrix();

    Matrix &operator=(const Matrix &m);
    T &get(size_t row, size_t col);
    size_t get_rows();
    size_t get_cols();
    void resize(const size_t num_rows, const size_t num_cols);
};

/**
 * @brief Default Constructor
 *
 * @tparam T The type of the values stored in Matrix
 * @param rows The number of rows in the Matrix
 * @param cols The number of columns in the Matrix
 * @return Matrix<T> The Matrix object
 */
template <typename T>
Matrix<T>::Matrix(size_t rows, size_t cols)
: rows(rows), cols(cols) {
    this->data = new T[rows * cols];
}

/**
 * @brief Copy Constructor
 *
 * @tparam T The type of the values stored in Matrix
 * @param m The Matrix to be copied
 * @return Matrix<T> The new Matrix object
 */
template <typename T>
Matrix<T>::Matrix(const Matrix &m)
: rows(rows), cols(cols) {
    std::memcpy(this->data, m.data, m.rows * m.cols * sizeof(T));
}

/**
 * @brief Destructor
 *
 * @tparam T The type of the values stored in Matrix
 */
template <typename T>
Matrix<T>::~Matrix() {
    delete[] this->data;
}

/**
 * @brief Overload operator=
 *
 * @tparam T The type of the values stored in Matrix
 */
template <typename T>
Matrix<T> &Matrix<T>::operator=(const Matrix &m) {
    this->rows = m.rows;
    this->cols = m.cols;
    std::memcpy(this->data, m.data, m.rows * m.cols * sizeof(T));
    return *this;
}

template <typename T>
T &Matrix<T>::get(size_t row, size_t col) {
    return this->data[row * this->cols + col];
}

template <typename T>
size_t Matrix<T>::get_rows() {
    return this->rows;
}

template <typename T>
size_t Matrix<T>::get_cols() {
    return this->cols;
}

template <typename T>
void
Matrix<T>::resize(const size_t num_rows, const size_t num_cols) {
    // Check if the new Matrix size is valid
    if (num_rows <= 0 || num_cols <= 0) {
        throw std::length_error("Matrix num_rows and num_cols must be positive.");
    }

    // Check if the Matrix already contains data or is uninitialized
    if (this->data == nullptr) {
        this->num_rows = num_rows;
        this->num_cols = num_cols;
        this->data = new T[num_rows * num_cols];
    }
    // Copy the values that can be fit into the new Matrix
    else {
        T *new_data = new T[num_rows * num_cols];
        for (size_t row = 0, end_row = std::min<size_t>(num_rows, this->num_rows); row < end_row; ++row) {
            std::memcpy(
                new_data + (row * num_cols),
                this->data + (row * this->num_rows),
                std::min<size_t>(num_cols, this->num_cols) * sizeof(T));
        }

        delete[] this->data;
        this->data = new_data;
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, Matrix<T> &mat) {
    for (size_t row = 0; row < mat.get_rows(); ++row) {
        for (size_t col = 0; col < mat.get_cols(); ++col) {
            stream << mat.get(row, col) << ' ';
        }
        stream << '\n';
    }
    return stream;
}

#endif
