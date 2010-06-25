/*
 * Name        : Vector4d.h
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

#ifndef VECTOR4D_H_
#define VECTOR4D_H_

#include "Scalar.h"

/**
 * @brief Provides needed 4d-vector functionality and operations
 */
class Vector4d
{
public:
    Vector4d(Scalar x = 0, Scalar y = 0, Scalar z = 0, Scalar w = 0);

//private:
    Scalar mX;
    Scalar mY;
    Scalar mZ;
    Scalar mW;
};

inline Vector4d::Vector4d(Scalar x, Scalar y, Scalar z, Scalar w)
    : mX(x), mY(y), mZ(z), mW(w)
{}

#endif
