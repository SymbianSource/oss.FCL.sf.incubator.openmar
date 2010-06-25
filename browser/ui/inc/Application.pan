/*
 * Name        : Application.pan
 * Description : Panic definitions
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

#ifndef __APPLICATION_PAN__
#define __APPLICATION_PAN__

enum TApplicationPanic
{

};

inline void Panic(TApplicationPanic aReason)
{
    _LIT(appName, "OpenMAR");
    User::Panic(appName, aReason);
}

#endif // __APPLICATION_PAN__
