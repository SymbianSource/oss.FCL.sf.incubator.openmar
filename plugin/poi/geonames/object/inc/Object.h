/*
 * Name        : Object.h
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

#ifndef OBJECT_H_
#define OBJECT_H_

#include "POIObject.h"

namespace Geonames {

/**
 * @brief Implements basic rendering for geonames.org landmarks
 */
class CObject : public OpenMAR::CPOIObject
{
public:
    static CObject* NewL(OpenMAR::CPOIProvider* aProvider);
    ~CObject();

protected:
    CObject(OpenMAR::CPOIProvider* aProvider);
    void ConstructL();

public:
    void Render() const;

private:
    TUint iTextureId;
};

}

#endif
