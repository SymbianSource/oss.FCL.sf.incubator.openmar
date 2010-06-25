/*
 * Name        : Loader.h
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

#ifndef LOADER_H_
#define LOADER_H_

#include <e32base.h>

class CPosLandmark;
class TCoordinate;

class MLandmarkLoaderObserver
{
public:
    virtual void LandmarkLoaderOpenedL(TInt aError) = 0;
    virtual void LandmarkLoaderItemCreatedL(const CPosLandmark& aLandmark) = 0;
    virtual void LandmarkLoaderItemUpdatedL(const CPosLandmark& aLandmark) = 0;
    virtual void LandmarkLoaderItemDeletedL(const CPosLandmark& aLandmark) = 0;
};

/**
 * @brief Interface for landmark loader
 */
class MLandmarkLoader
{
public:
    virtual ~MLandmarkLoader() = 0;

    virtual void RequestL(const TCoordinate& aCoordinate, TReal32 aRadius) = 0;
};

inline MLandmarkLoader::~MLandmarkLoader()
{}

#endif
