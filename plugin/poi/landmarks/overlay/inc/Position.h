/*
 * Name        : Position.h
 * Description : Position helper class
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

#ifndef POSITION_H_
#define POSITION_H_

#include <e32base.h>
#include <Lbs.h>
#include <LbsPositionInfo.h>

class MPositionObserver
{
public:
    virtual void PositionUpdateL(TInt aError, const TPosition& aPosition) = 0;
};

/**
 * @brief 
 * 
 */
class CPosition : public CActive
{
public:
    static CPosition* NewL(MPositionObserver& aObserver);
    ~CPosition();

protected:
    CPosition(MPositionObserver& aObserver);
    void ConstructL();

public:
    void Request();

protected:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    MPositionObserver& iObserver;

    RPositionServer iServer;
    RPositioner iPositioner;
    TPositionInfo iPositionInfo;

    enum TState {
        EUnknown,
        ECache,
        EGps
    } iState;
};

#endif  // POSITION_H_
