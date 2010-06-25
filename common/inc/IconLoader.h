/*
 * Name        : IconLoader.h
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

#ifndef ICONLOADER_H_
#define ICONLOADER_H_

#include <e32def.h>
#include <e32cmn.h>

class CGulIcon;
class TSize;

CGulIcon* CreateIconL(const TDesC& aIconFile, TInt aBitmapId, TInt aMaskId, const TSize& aSize = TSize(-1, -1));

#endif
