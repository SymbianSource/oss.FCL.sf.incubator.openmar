/*
 * Name        : POIObject.inl
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

inline CPOIObject* CPOIObject::NewL(const TDesC8& aMatch, const CPOIProvider& aProvider)
{
    const TUid KCPOIRendererInterfaceUid = { KObjectInterfaceUidValue };

    TAny* param = const_cast<CPOIProvider*>(&aProvider);

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(ETrue);

    TAny* interface = REComSession::CreateImplementationL(KCPOIRendererInterfaceUid,
        _FOFF(CPOIObject, iDtor_ID_Key), param, resolverParams);

    return reinterpret_cast <CPOIObject*>(interface);
}

inline CPOIObject* CPOIObject::NewL(const TUid aImplementationUid, const CPOIProvider& aProvider)
{
    TAny* param = const_cast<CPOIProvider*>(&aProvider);

    TAny* interface = REComSession::CreateImplementationL(aImplementationUid, 
        _FOFF(CPOIObject, iDtor_ID_Key), param);

    return reinterpret_cast <CPOIObject*>(interface);
}

inline CPOIObject::~CPOIObject()
{
    REComSession::DestroyedImplementation(iDtor_ID_Key);
}

inline CPOIObject::CPOIObject(const CPOIProvider& aProvider)
    : iProvider(aProvider)
{}

inline const TCoordinate& CPOIObject::GetCoordinate() const
{
    return iCoordinate;
}

inline void CPOIObject::SetCoordinate(const TCoordinate& aCoordinate)
{
    iCoordinate = aCoordinate;
}

inline const Vector3d& CPOIObject::GetPosition() const
{
    return iPosition;
}

inline void CPOIObject::SetPosition(const Vector3d& aPosition)
{
    iPosition = aPosition;
}

inline TUint32 CPOIObject::GetIdentifier() const
{
    return iIdentifier;
}

inline void CPOIObject::SetIdentifier(TUint32 aIdentifier)
{
    iIdentifier = aIdentifier;
}

inline const TDesC& CPOIObject::GetName() const
{
    return iName? *iName : KNullDesC();
}

inline void CPOIObject::SetNameL(const TDesC& aName)
{
    delete iName;
    iName = 0;

    iName = aName.AllocL();
}

inline TBool CPOIObject::IdentityRelation(const CPOIObject& aLeft, const CPOIObject& aRight)
{
    return aLeft.GetIdentifier() == aRight.GetIdentifier();
}
