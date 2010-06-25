/*
 * Name        : POIProvider.inl
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

inline CPOIProvider* CPOIProvider::NewL(const TDesC8& aMatch, const TDesC8& aProviderUri)
{
    const TUid KCPOIProviderInterfaceUid = { KProviderInterfaceUidValue };

    TDesC8& param = const_cast<TDesC8&>(aProviderUri);

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatch);
    resolverParams.SetWildcardMatch(ETrue);

    TAny* interface = REComSession::CreateImplementationL(KCPOIProviderInterfaceUid,
        _FOFF(CPOIProvider, iDtor_ID_Key), &param, resolverParams);

    return reinterpret_cast <CPOIProvider*>(interface);
}

inline CPOIProvider* CPOIProvider::NewL(const TUid aImplementationUid, const TDesC8& aProviderUri)
{
    TDesC8& param = const_cast<TDesC8&>(aProviderUri);

    TAny* interface = REComSession::CreateImplementationL(aImplementationUid, 
        _FOFF(CPOIProvider, iDtor_ID_Key), &param);

    return reinterpret_cast <CPOIProvider*>(interface);
}

inline CPOIProvider::~CPOIProvider()
{
    REComSession::DestroyedImplementation(iDtor_ID_Key);
}

inline void CPOIProvider::SetObserver(MPOIProviderObserver* aObserver)
{
    iObserver = aObserver;
}
