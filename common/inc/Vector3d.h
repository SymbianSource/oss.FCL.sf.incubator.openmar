/*
 * Name        : Vector3d.h
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

#ifndef VECTOR3D_H_
#define VECTOR3D_H_

#include <e32math.h>

#include "Scalar.h"

/**
 * @brief Provides needed 3d-vector functionality and operations
 */
class Vector3d
{
public:
    Vector3d(Scalar x = 0, Scalar y = 0, Scalar z = 0);

    Scalar Norm() const;

    static const Vector3d Cross(const Vector3d& lhs, const Vector3d& rhs);

//private:
    Scalar mX;
    Scalar mY;
    Scalar mZ;
};

inline Vector3d::Vector3d(Scalar x, Scalar y, Scalar z)
    : mX(x), mY(y), mZ(z)
{}

inline Scalar Vector3d::Norm() const
{
    Scalar sm = mX * mX + mY * mY + mZ * mZ;

    TReal norm = 0;
    Math::Sqrt(norm, sm);

    return norm;
}

inline const Vector3d Vector3d::Cross(const Vector3d& lhs, const Vector3d& rhs)
{
    return Vector3d(lhs.mY * rhs.mZ - lhs.mZ * rhs.mY,
                    lhs.mZ * rhs.mX - lhs.mX * rhs.mZ,
                    lhs.mX * rhs.mY - lhs.mY * rhs.mX);
}

#endif
