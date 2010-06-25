/*
 * Name        : Matrix4d.h
 * Description : 
 * Project     : This file is part of OpenMAR, an Open Mobile Augmented Reality browser
 * Website     : http://OpenMAR.org
 *
 * Copyright (c) 2010 David Caabeiro
 *
 * All rights reserved. This program and the accompanying materials are made available 
 * under the terms of the Eclipse Public License v1.0 which accompanies this 
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef MATRIX4D_H_
#define MATRIX4D_H_

#include "Scalar.h"
#include "Vector3d.h"
#include "Vector4d.h"

/**
 * @brief Provides needed 4d-matrix functionality and operations
 */
class Matrix4d
{
public:
    Matrix4d();
    Matrix4d(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
             Scalar m10, Scalar m11, Scalar m12, Scalar m13,
             Scalar m20, Scalar m21, Scalar m22, Scalar m23,
             Scalar m30, Scalar m31, Scalar m32, Scalar m33);

    const Vector4d operator*(const Vector4d& v) const;
    const Matrix4d& operator*(Scalar s);

    void Load(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
            Scalar m10, Scalar m11, Scalar m12, Scalar m13,
            Scalar m20, Scalar m21, Scalar m22, Scalar m23,
            Scalar m30, Scalar m31, Scalar m32, Scalar m33);

    const Matrix4d Transpose() const;

//private:
    Scalar m[16];
};

inline Matrix4d::Matrix4d()
{
    m[0]  = 1; m[1]  = 0; m[2]  = 0; m[3]  = 0;
    m[4]  = 0; m[5]  = 1; m[6]  = 0; m[7]  = 0;
    m[8]  = 0; m[9]  = 0; m[10] = 1; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

inline Matrix4d::Matrix4d(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
                        Scalar m10, Scalar m11, Scalar m12, Scalar m13,
                        Scalar m20, Scalar m21, Scalar m22, Scalar m23,
                        Scalar m30, Scalar m31, Scalar m32, Scalar m33)
{
    Load(m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33);
}

inline const Vector4d Matrix4d::operator*(const Vector4d& v) const
{
    return Vector4d(
        m[0]  * v.mX + m[1]  * v.mY + m[2]  * v.mZ + m[3]  * v.mW,
        m[4]  * v.mX + m[5]  * v.mY + m[6]  * v.mZ + m[7]  * v.mW,
        m[8]  * v.mX + m[9]  * v.mY + m[10] * v.mZ + m[11] * v.mW,
        m[12] * v.mX + m[13] * v.mY + m[14] * v.mZ + m[15] * v.mW
    );
}

inline const Matrix4d& Matrix4d::operator*(Scalar s)
{
    for (TInt i = 0; i < 16; ++i)
            m[i] *= s;

    return *this;
}

inline void Matrix4d::Load(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
                        Scalar m10, Scalar m11, Scalar m12, Scalar m13,
                        Scalar m20, Scalar m21, Scalar m22, Scalar m23,
                        Scalar m30, Scalar m31, Scalar m32, Scalar m33)
{
    m[0]  = m00; m[1]  = m01; m[2]  = m02; m[3]  = m03;
    m[4]  = m10; m[5]  = m11; m[6]  = m12; m[7]  = m13;
    m[8]  = m20; m[9]  = m21; m[10] = m22; m[11] = m23;
    m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
}

inline const Matrix4d Matrix4d::Transpose() const
{
    return Matrix4d(
        m[0], m[4], m[8], m[12],
        m[1], m[5], m[9], m[13],
        m[2], m[6], m[10], m[14],
        m[3], m[7], m[11], m[15]
    );
}

#endif  // MATRIX4D_H_
