/*
 * Name        : POIObject.h
 * Description : ECOM interface definition
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

#ifndef POIOBJECT_H_
#define POIOBJECT_H_

#include "InterfaceUid.hrh"

#include <ecom/ecom.h>

#include <LbsPosition.h>

#include "Vector3d.h"

namespace OpenMAR {

class CPOIProvider;

/**
 * @brief POI object interface definition
 * 
 * 
 */
class CPOIObject : public CBase
{
public:
    static CPOIObject* NewL(const TDesC8& aMatch, const CPOIProvider& aProvider);
    static CPOIObject* NewL(const TUid aImplementationUid, const CPOIProvider& aProvider);
    ~CPOIObject();

protected:
    CPOIObject(const CPOIProvider& aProvider);

public:
    const TCoordinate& GetCoordinate() const;
    void SetCoordinate(const TCoordinate& aCoordinate);

    const Vector3d& GetPosition() const;
    void SetPosition(const Vector3d& aPosition);

    TUint32 GetIdentifier() const;
    void SetIdentifier(TUint32 aIdentifier);

    const TDesC& GetName() const;
    void SetNameL(const TDesC& aName);
/*
    const TDesC& GetDescription() const;
    void SetDescription(const TDesC& aName);

    TReal32 GetCoverageRadius() const;
    void SetCoverageRadius(TReal32 aCoverageRadius);
*/
    virtual void Render() const = 0;

public:
    static TBool IdentityRelation(const CPOIObject& aLeft, const CPOIObject& aRight);

protected:
    const CPOIProvider& iProvider;

    TCoordinate iCoordinate;
    Vector3d iPosition;
    TUint32 iIdentifier;
    HBufC* iName;

private:
    TUid iDtor_ID_Key;
};

#include "POIObject.inl"

}

#endif  // POIOBJECT_H_
