/*
* Copyright 2014 Google Inc. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#ifndef MATHFU_VECTOR_H_
#define MATHFU_VECTOR_H_

#include "mathfu/utilities.h"

#include <math.h>

/// @file mathfu/vector.h Vector
/// @brief Vector class and functions.
/// @addtogroup mathfu_vector

// Disable spurious warnings generated by MATHFU_VECTOR_OPERATION().
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127)  // conditional expression is constant
#if _MSC_VER >= 1900             // MSVC 2015
#pragma warning(disable : 4456)  // allow shadowing in unrolled loops
#endif                           // _MSC_VER >= 1900
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"
#endif

/// @cond MATHFU_INTERNAL
#define MATHFU_VECTOR_OPERATION(OP) MATHFU_UNROLLED_LOOP(i, d, OP)
/// @endcond

/// @cond MATHFU_INTERNAL
#define MATHFU_VECTOR_OPERATOR(OP)           \
  {                                          \
    Vector<T, d> result;                     \
    MATHFU_VECTOR_OPERATION(result[i] = OP); \
    return result;                           \
  }
/// @endcond

/// @cond MATHFU_INTERNAL
#define MATHFU_VECTOR_SELF_OPERATOR(OP) \
  {                                     \
    MATHFU_VECTOR_OPERATION(OP);        \
    return *this;                       \
  }
/// @endcond

namespace mathfu {

template <class T, int d>
class Vector;

/// @cond MATHFU_INTERNAL
template <class T, int d>
static inline T DotProductHelper(const Vector<T, d>& v1,
                                 const Vector<T, d>& v2);
template <class T>
static inline T DotProductHelper(const Vector<T, 2>& v1,
                                 const Vector<T, 2>& v2);
template <class T>
static inline T DotProductHelper(const Vector<T, 3>& v1,
                                 const Vector<T, 3>& v2);
template <class T>
static inline T DotProductHelper(const Vector<T, 4>& v1,
                                 const Vector<T, 4>& v2);
/// @endcond

/// @addtogroup mathfu_vector
/// @{

/// @class VectorPacked "mathfu/vector.h"
/// @brief Packed N-dimensional vector.
///
/// Some Vector classes are padded so that it's possible to use the data
/// structures with SIMD instructions.  This structure can be used in
/// conjunction with unpacked Vector classes to pack data
/// into flat arrays suitable for sending to a GPU (e.g vertex buffers).
///
/// <p>
/// For example, to pack (store) an unpacked to packed vector:<br>
/// <blockquote><code><pre>
/// VectorPacked<float, 3> packed;
/// Vector<float, 3> vector(3, 2, 1);
/// vector.Pack(&packed);
/// </pre></code></blockquote>
/// or<br>
/// <blockquote><code><pre>
/// Vector<float, 3> vector(3, 2, 1);
/// VectorPacked<float, 3> packed = vector;
/// </pre></code></blockquote>
/// </p>
///
/// <p>
/// To initialize a vector from a packed vector:<br>
/// <blockquote><code><pre>
/// VectorPacked<float, 3> packed = { 3, 2, 1 };
/// Vector<float, 3> vector(packed);
/// </pre></code></blockquote>
///
/// @tparam T type of VectorPacked elements.
/// @tparam d dimensions (number of elements) in the VectorPacked structure.
template <class T, int d>
struct VectorPacked {
  /// Create an uninitialized VectorPacked.
  VectorPacked() {}

  /// Create a VectorPacked from a Vector.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to create the VectorPacked from.
  explicit VectorPacked(const Vector<T, d>& vector) { vector.Pack(this); }

  /// Copy a Vector to a VectorPacked.
  ///
  /// Both VectorPacked and Vector must have the same number of dimensions.
  /// @param vector Vector to copy to the VectorPacked.
  /// @returns A reference to this VectorPacked.
  VectorPacked& operator=(const Vector<T, d>& vector) {
    vector.Pack(this);
    return *this;
  }

  /// Elements of the packed vector one per dimension.
  T data[d];
};
/// @}

/// @addtogroup mathfu_vector
/// @{
/// @class Vector "mathfu/vector.h"
/// @brief Vector of d elements with type T
///
/// Vector stores <b>d</b> elements of type <b>T</b> and provides a set
/// functions to perform operations on the set of elements.
///
/// @tparam T type of Vector elements.
/// @tparam d dimensions (number of elements) in the Vector structure.
template <class T, int d>
class Vector {
 public:
  /// @brief Element type to enable reference by other classes.
  typedef T Scalar;

  /// @brief Create an uninitialized Vector.
  inline Vector() {}

  /// @brief Create a vector from another vector copying each element.
  ///
  /// @param v Vector that the data will be copied from.
  inline Vector(const Vector<T, d>& v) {
    MATHFU_VECTOR_OPERATION(data_[i] = v.data_[i]);
  }

  /// @brief Create a vector from another vector of a different type.
  ///
  /// This copies each element of a Vector which makes it possible to between
  /// vectors of different types, for example
  /// <code>float/double/int</code> vectors.
  /// @param v Vector that the data will be copied from.
  /// @tparam U type of Vector elements to copy.
  template <typename U>
  explicit inline Vector(const Vector<U, d>& v) {
    MATHFU_VECTOR_OPERATION(data_[i] = static_cast<T>(v[i]));
  }

  /// @brief Create a vector from a single float.
  ///
  /// Each elements is set to be equal to the value given.
  /// @param s Scalar value that the vector will be initialized to.
  explicit inline Vector(const T& s) { MATHFU_VECTOR_OPERATION(data_[i] = s); }

  /// @brief Create a vector form the first d elements of an array.
  ///
  /// @param a Array of values that the vector will be iniitlized to.
  explicit inline Vector(const T* a) {
    MATHFU_VECTOR_OPERATION(data_[i] = a[i]);
  }

  /// @brief Create a vector from two values.
  ///
  /// @note This method only works when the vector is of size two.
  ///
  /// @param s1 Scalar value for the first element of the vector.
  /// @param s2 Scalar value for the second element of the vector.
  inline Vector(const T& s1, const T& s2) {
    MATHFU_STATIC_ASSERT(d == 2);
    data_[0] = s1;
    data_[1] = s2;
  }

  /// @brief Create a vector from three values.
  ///
  /// @note This method only works when the vector is of size three.
  ///
  /// @param s1 Scalar value for the first element of the vector.
  /// @param s2 Scalar value for the second element of the vector.
  /// @param s3 Scalar value for the third element of the vector.
  inline Vector(const T& s1, const T& s2, const T& s3) {
    MATHFU_STATIC_ASSERT(d == 3);
    data_[0] = s1;
    data_[1] = s2;
    data_[2] = s3;
  }

  /// @brief Create a vector from a 2 component vector and a third value.
  ///
  /// @note This method only works when the vector is of size three.
  ///
  /// @param v12 Vector containing the first 2 values.
  /// @param s3 Scalar value for the third element of the vector.
  inline Vector(const Vector<T, 2>& v12, const T& s3) {
    MATHFU_STATIC_ASSERT(d == 3);
    data_[0] = v12.x();
    data_[1] = v12.y();
    data_[2] = s3;
  }

  /// @brief Create a vector from four values.
  ///
  /// @note This method only works when the vector is of size four.
  ///
  /// @param s1 Scalar value for the first element of the vector.
  /// @param s2 Scalar value for the second element of the vector.
  /// @param s3 Scalar value for the third element of the vector.
  /// @param s4 Scalar value for the forth element of the vector.
  inline Vector(const T& s1, const T& s2, const T& s3, const T& s4) {
    MATHFU_STATIC_ASSERT(d == 4);
    data_[0] = s1;
    data_[1] = s2;
    data_[2] = s3;
    data_[3] = s4;
  }

  /// @brief Create a 4-dimensional vector from a Vector<T, 3>.
  ///
  /// The last element is initialized to the specified value.
  /// @note This method only works with 4 element vectors.
  ///
  /// @param vector3 Vector used to initialize the first 3 elements.
  /// @param value Value used to set the last element of the vector.
  inline Vector(const Vector<T, 3>& vector3, const T& value) {
    MATHFU_STATIC_ASSERT(d == 4);
    data_[0] = vector3[0];
    data_[1] = vector3[1];
    data_[2] = vector3[2];
    data_[3] = value;
  }

  /// @brief Create a vector from two 2 component vectors.
  ///
  /// @note This method only works when the vector is of size four.
  ///
  /// @param v12 Vector containing the first 2 values.
  /// @param v34 Vector containing the last 2 values.
  inline Vector(const Vector<T, 2>& v12, const Vector<T, 2>& v34) {
    MATHFU_STATIC_ASSERT(d == 4);
    data_[0] = v12.x();
    data_[1] = v12.y();
    data_[2] = v34.x();
    data_[3] = v34.y();
  }

  /// @brief Create a vector from packed vector (VectorPacked).
  ///
  /// @param vector Packed vector used to initialize an unpacked.
  explicit inline Vector(const VectorPacked<T, d>& vector) {
    MATHFU_VECTOR_OPERATION(data_[i] = vector.data[i]);
  }

  /// @brief Access an element of the vector.
  ///
  /// @param i Index of the element to access.
  /// @return A reference to the accessed data that can be modified by the
  /// caller.
  inline T& operator()(const int i) { return data_[i]; }

  /// @brief Access an element of the vector.
  ///
  /// @param i Index of the element to access.
  /// @return A reference to the accessed data.
  inline const T& operator()(const int i) const { return data_[i]; }

  /// @brief Access an element of the vector.
  ///
  /// @param i Index of the element to access.
  /// @return A reference to the accessed data that can be modified by the
  /// caller.
  inline T& operator[](const int i) { return data_[i]; }

  /// @brief Access an element of the vector.
  ///
  /// @param i Index of the element to access.
  /// @return A const reference to the accessed.
  inline const T& operator[](const int i) const { return data_[i]; }

  /// Get the first element (X axis) of the Vector.
  inline T& x() {
    MATHFU_STATIC_ASSERT(d > 0);
    return data_[0];
  }
  /// Get the second element (Y axis) of the Vector.
  inline T& y() {
    MATHFU_STATIC_ASSERT(d > 1);
    return data_[1];
  }
  /// Get the third element (Z axis) of the Vector.
  inline T& z() {
    MATHFU_STATIC_ASSERT(d > 2);
    return data_[2];
  }
  /// Get the fourth element (W axis) of the Vector.
  inline T& w() {
    MATHFU_STATIC_ASSERT(d > 3);
    return data_[3];
  }

  /// Get the first element (X axis) of the Vector.
  inline const T& x() const {
    MATHFU_STATIC_ASSERT(d > 0);
    return data_[0];
  }
  /// Get the second element (Y axis) of the Vector.
  inline const T& y() const {
    MATHFU_STATIC_ASSERT(d > 1);
    return data_[1];
  }
  /// Get the third element (Z axis) of the Vector.
  inline const T& z() const {
    MATHFU_STATIC_ASSERT(d > 2);
    return data_[2];
  }
  /// Get the fourth element (W axis) of the Vector.
  inline const T& w() const {
    MATHFU_STATIC_ASSERT(d > 3);
    return data_[3];
  }

  /// @brief GLSL style 3 element accessor.
  ///
  /// This only works with vectors that contain more than 3 elements.
  /// @returns A 3-dimensional Vector containing the first 3 elements of
  // this Vector.
  inline Vector<T, 3> xyz() {
    MATHFU_STATIC_ASSERT(d > 3);
    return Vector<T, 3>(x(), y(), z());
  }

  /// @brief GLSL style 3 element accessor.
  ///
  /// This only works with vectors that contain more than 3 elements.
  /// @returns A 3-dimensional Vector containing the first 3 elements of
  // this Vector.
  inline const Vector<T, 3> xyz() const {
    MATHFU_STATIC_ASSERT(d > 3);
    return Vector<T, 3>(x(), y(), z());
  }

  /// @brief GLSL style 2 element accessor.
  ///
  /// This only works with vectors that contain more than 2 elements.
  /// @returns A 2-dimensional Vector with the first 2 elements of this Vector.
  inline Vector<T, 2> xy() {
    MATHFU_STATIC_ASSERT(d > 2);
    return Vector<T, 2>(x(), y());
  }

  /// @brief GLSL style 2 element accessor.
  ///
  /// This only works with vectors that contain more than 2 elements.
  /// @returns A 2-dimensional Vector with the first 2 elements of this Vector.
  inline const Vector<T, 2> xy() const {
    MATHFU_STATIC_ASSERT(d > 2);
    return Vector<T, 2>(x(), y());
  }

  /// @brief GLSL style 2 element accessor.
  ///
  /// This only works with vectors that contain 4 elements.
  /// @returns A 2-dimensional Vector with the last 2 elements of this Vector.
  inline Vector<T, 2> zw() {
    MATHFU_STATIC_ASSERT(d == 4);
    return Vector<T, 2>(z(), w());
  }

  /// @brief GLSL style 2 element accessor.
  ///
  /// This only works with vectors that contain 4 elements.
  /// @returns A 2-dimensional Vector with the last 2 elements of this Vector.
  inline const Vector<T, 2> zw() const {
    MATHFU_STATIC_ASSERT(d == 4);
    return Vector<T, 2>(z(), w());
  }

  /// @brief Pack a Vector to a packed "d" element vector structure.
  ///
  /// @param vector Packed "d" element vector to write to.
  inline void Pack(VectorPacked<T, d>* const vector) const {
    MATHFU_VECTOR_OPERATION(vector->data[i] = data_[i]);
  }

  /// @brief Negate all elements of the Vector.
  ///
  /// @return A new Vector containing the result.
  inline Vector<T, d> operator-() const { MATHFU_VECTOR_OPERATOR(-data_[i]); }

  /// @brief Multiply this Vector by another Vector.
  ///
  /// In line with GLSL, this performs component-wise multiplication.
  /// @param v A Vector to mulitply this Vector with.
  /// @return A new Vector containing the result.
  inline Vector<T, d> operator*(const Vector<T, d>& v) const {
    return HadamardProduct(*this, v);
  }

  /// @brief Divide this Vector by another Vector.
  ///
  /// In line with GLSL, this performs component-wise division.
  /// @param v A Vector to divide this Vector by.
  /// @return A new Vector containing the result.
  inline Vector<T, d> operator/(const Vector<T, d>& v) const {
    MATHFU_VECTOR_OPERATOR(data_[i] / v[i]);
  }

  /// @brief Add this Vector with another Vector.
  ///
  /// @param v A vector to add this vector with.
  /// @return A new vector containing the result.
  inline Vector<T, d> operator+(const Vector<T, d>& v) const {
    MATHFU_VECTOR_OPERATOR(data_[i] + v[i]);
  }

  /// @brief Add this Vector with another Vector.
  ///
  /// @param v A vector to subtract from this vector.
  /// @return A new vector containing the result.
  inline Vector<T, d> operator-(const Vector<T, d>& v) const {
    MATHFU_VECTOR_OPERATOR(data_[i] - v[i]);
  }

  /// @brief Multiply this Vector with a scalar.
  ///
  /// @param s A scalar to multiply this vector with.
  /// @return A new vector containing the result.
  inline Vector<T, d> operator*(const T& s) const {
    MATHFU_VECTOR_OPERATOR(data_[i] * s);
  }

  /// @brief Divide this Vector by a scalar.
  ///
  /// @param s A scalar to divide this vector with.
  /// @return A new vector containing the result.
  inline Vector<T, d> operator/(const T& s) const {
    MATHFU_VECTOR_OPERATOR(data_[i] / s);
  }

  /// @brief Add a scalar to all elements of this Vector.
  ///
  /// @param s A scalar to add to this vector.
  /// @return A new vector containing the result.
  inline Vector<T, d> operator+(const T& s) const {
    MATHFU_VECTOR_OPERATOR(data_[i] + s);
  }

  /// @brief Subtract a scalar from all elements of this vector.
  ///
  /// @param s A scalar to subtract from this vector.
  /// @return A new vector that stores the result.
  inline Vector<T, d> operator-(const T& s) const {
    MATHFU_VECTOR_OPERATOR(data_[i] - s);
  }

  /// @brief Multiply (in-place) this Vector with another Vector.
  ///
  /// In line with GLSL, this performs component-wise multiplication.
  /// @param v A vector to multiply this vector with.
  /// @return A reference to this class.
  inline Vector<T, d>& operator*=(const Vector<T, d>& v) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] *= v[i]);
  }

  /// @brief Divide (in-place) this Vector by another Vector.
  ///
  /// In line with GLSL, this performs component-wise division.
  /// @param v A vector to divide this vector by.
  /// @return A reference to this class.
  inline Vector<T, d>& operator/=(const Vector<T, d>& v) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] /= v[i]);
  }

  /// @brief Add (in-place) this Vector with another Vector.
  ///
  /// @param v A vector to add this vector with.
  /// @return A reference to this class.
  inline Vector<T, d>& operator+=(const Vector<T, d>& v) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] += v[i]);
  }

  /// @brief Subtract (in-place) another Vector from this Vector.
  ///
  /// @param v A vector to subtract this vector by.
  /// @return A reference to this class.
  inline Vector<T, d>& operator-=(const Vector<T, d>& v) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] -= v[i]);
  }

  /// @brief Multiply (in-place) each element of this Vector with a scalar.
  ///
  /// @param s A scalar to mulitply this vector with.
  /// @return A reference to this class.
  inline Vector<T, d>& operator*=(const T& s) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] *= s);
  }

  /// @brief Divide (in-place) each element of this Vector by a scalar.
  ///
  /// @param s A scalar to divide this vector by.
  /// @return A reference to this class.
  inline Vector<T, d>& operator/=(const T& s) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] /= s);
  }

  /// @brief Add (in-place) a scalar to each element of this Vector.
  ///
  /// @param s A scalar to add this vector to.
  /// @return A reference to this class.
  inline Vector<T, d>& operator+=(const T& s) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] += s);
  }

  /// @brief Subtract (in-place) a scalar from each element of this Vector.
  ///
  /// @param s A scalar to subtract from this vector.
  /// @return A reference to this class.
  inline Vector<T, d>& operator-=(const T& s) {
    MATHFU_VECTOR_SELF_OPERATOR(data_[i] -= s);
  }

  /// @brief Calculate the squared length of this vector.
  ///
  /// @return The length of this vector squared.
  inline T LengthSquared() const { return DotProduct(*this, *this); }

  /// @brief Calculate the length of this vector.
  ///
  /// @return The length of this vector.
  inline T Length() const { return sqrt(LengthSquared()); }

  /// @brief Normalize this vector in-place.
  ///
  /// @return The length of this vector.
  inline T Normalize() {
    const T length = Length();
    *this = *this * (1 / length);
    return length;
  }

  /// @brief Calculate the normalized version of this vector.
  ///
  /// @return The normalized vector.
  inline Vector<T, d> Normalized() const { return *this * (1 / Length()); }

  /// @brief Calculate the dot product of two vectors.
  ///
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @return The dot product of v1 and v2.
  static inline T DotProduct(const Vector<T, d>& v1, const Vector<T, d>& v2) {
    return DotProductHelper(v1, v2);
  }

  /// @brief Calculate the hadamard or componentwise product of two vectors.
  ///
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @return The hadamard product of v1 and v2.
  static inline Vector<T, d> HadamardProduct(const Vector<T, d>& v1,
                                             const Vector<T, d>& v2) {
    MATHFU_VECTOR_OPERATOR(v1[i] * v2[i]);
  }

  /// @brief Calculate the cross product of two vectors.
  ///
  /// Note that this function is only defined for 3-dimensional Vectors.
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @return The cross product of v1 and v2.
  static inline Vector<T, 3> CrossProduct(const Vector<T, 3>& v1,
                                          const Vector<T, 3>& v2) {
    return Vector<T, 3>(v1[1] * v2[2] - v1[2] * v2[1],
                        v1[2] * v2[0] - v1[0] * v2[2],
                        v1[0] * v2[1] - v1[1] * v2[0]);
  }

  /// @brief Linearly interpolate two vectors.
  ///
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @param percent Percentage from v1 to v2 in range 0.0...1.0.
  /// @return The hadamard product of v1 and v2.
  static inline Vector<T, d> Lerp(const Vector<T, d>& v1,
                                  const Vector<T, d>& v2, const T percent) {
    const T one_minus_percent = static_cast<T>(1.0) - percent;
    MATHFU_VECTOR_OPERATOR(one_minus_percent * v1[i] + percent * v2[i]);
  }

  /// @brief Generates a random vector.
  ///
  /// The range of each component is bounded by min and max.
  /// @param min Minimum value of the vector.
  /// @param max Maximum value of the vector.
  static inline Vector<T, d> RandomInRange(const Vector<T, d>& min,
                                           const Vector<T, d>& max) {
    Vector<T, d> result;
    MATHFU_VECTOR_OPERATION(result[i] =
                                mathfu::RandomInRange<T>(min[i], max[i]));
    return result;
  }

  /// @brief Compare each component and returns max values.
  ///
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @return Max value of v1 and v2.
  static inline Vector<T, d> Max(const Vector<T, d>& v1,
                                 const Vector<T, d>& v2) {
    Vector<T, d> result;
    MATHFU_VECTOR_OPERATION(result[i] = std::max(v1[i], v2[i]));
    return result;
  }

  /// @brief Compare each component and returns min values.
  ///
  /// @param v1 First vector.
  /// @param v2 Second vector.
  /// @return Min value of v1 and v2.
  static inline Vector<T, d> Min(const Vector<T, d>& v1,
                                 const Vector<T, d>& v2) {
    Vector<T, d> result;
    MATHFU_VECTOR_OPERATION(result[i] = std::min(v1[i], v2[i]));
    return result;
  }

  MATHFU_DEFINE_CLASS_SIMD_AWARE_NEW_DELETE

 private:
  /// Elements of the vector.
  T data_[d];
};
/// @}

/// @addtogroup mathfu_vector
/// @{

/// @brief Multiply a Vector by a scalar.
///
/// Multiplies each component of the specified Vector with a scalar.
///
/// @param s scalar to multiply.
/// @param v Vector to multiply.
/// @return Vector containing the result.
/// @related Vector
template <class T, int d>
inline Vector<T, d> operator*(const T& s, const Vector<T, d>& v) {
  return v * s;
}

/// @brief Divide a Vector by a scalar.
///
/// Divides each component of the specified Vector by a scalar.
///
/// @param v Vector to be divided.
/// @param s scalar to divide the vector by.
/// @return Vector containing the result.
/// @related Vector
template <class T, int d>
inline Vector<T, d> operator/(const Vector<T, d>& v, const T& s) {
  return v / s;
}

/// @brief Add a scalar to each element of a Vector.
///
/// @param s scalar to add to each element of a Vector.
/// @param v Vector to add the scalar to.
/// @return Vector containing the result.
/// @related Vector
template <class T, int d>
inline Vector<T, d> operator+(const T& s, const Vector<T, d>& v) {
  return v + s;
}

/// @brief Subtract a scalar from each element of a Vector.
///
/// @param s scalar to subtract from each element of a Vector.
/// @param v Vector to subtract the scalar from.
/// @return Vector containing the result.
/// @related Vector
template <class T, int d>
inline Vector<T, d> operator-(const T& s, const Vector<T, d>& v) {
  return v - s;
}

/// @brief Check if val is within [range_start..range_end), denoting a
/// rectangular area.
///
/// @param val 2D vector to be tested.
/// @param range_start Starting point of the range (inclusive).
/// @param range_end Ending point of the range (non-inclusive).
/// @return Bool indicating success.
///
/// @tparam T Type of vector components to test.
template <class T>
bool InRange2D(const mathfu::Vector<T, 2>& val,
               const mathfu::Vector<T, 2>& range_start,
               const mathfu::Vector<T, 2>& range_end) {
  return InRange(val.x(), range_start.x(), range_end.x()) &&
         InRange(val.y(), range_start.y(), range_end.y());
}

/// @cond MATHFU_INTERNAL
/// @brief Calculate the dot product of two vectors.
///
/// @param v1 First vector.
/// @param v2 Second vector.
/// @return The dot product of v1 and v2.
/// @related Vector
template <class T, int d>
static inline T DotProductHelper(const Vector<T, d>& v1,
                                 const Vector<T, d>& v2) {
  T result = 0;
  MATHFU_VECTOR_OPERATION(result += v1[i] * v2[i]);
  return result;
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline T DotProductHelper(const Vector<T, 2>& v1,
                                 const Vector<T, 2>& v2) {
  return v1[0] * v2[0] + v1[1] * v2[1];
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline T DotProductHelper(const Vector<T, 3>& v1,
                                 const Vector<T, 3>& v2) {
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
/// @endcond

/// @cond MATHFU_INTERNAL
template <class T>
static inline T DotProductHelper(const Vector<T, 4>& v1,
                                 const Vector<T, 4>& v2) {
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}
/// @endcond

/// @}

}  // namespace mathfu

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif  // MATHFU_VECTOR_H_