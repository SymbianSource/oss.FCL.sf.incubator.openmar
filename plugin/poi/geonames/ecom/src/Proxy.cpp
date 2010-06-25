/*
 * Name        : Proxy.cpp
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

#include <e32base.h>

#include <ecom/implementationproxy.h>

#include "ImplementationUid.hrh"
#include "Provider.h"
#include "Object.h"

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KProviderImplementationUidValue, CProvider::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KObjectImplementationUidValue, Geonames::CObject::NewL),
};

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}
