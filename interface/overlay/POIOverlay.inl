/*
 * Name        : POIOverlay.inl
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

inline CPOIOverlay* CPOIOverlay::NewL(const TDesC8& aMatch, SParameter& aParam)
{
    const TUid KCPOIOverlayInterfaceUid = { KOverlayInterfaceUidValue };

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(ETrue);

    TAny* interface = REComSession::CreateImplementationL(KCPOIOverlayInterfaceUid,
        _FOFF(CPOIOverlay, iDtor_ID_Key), &aParam, resolverParams);

    return reinterpret_cast <CPOIOverlay*>(interface);
}

inline CPOIOverlay* CPOIOverlay::NewL(const TUid aImplementationUid, SParameter& aParam)
{
    TAny* interface = REComSession::CreateImplementationL(aImplementationUid, 
        _FOFF(CPOIOverlay, iDtor_ID_Key), &aParam);

    return reinterpret_cast <CPOIOverlay*>(interface);
}

inline CPOIOverlay::~CPOIOverlay()
{
    REComSession::DestroyedImplementation(iDtor_ID_Key);
}
