//
//  BristleBrushController.h
//  GEL
//
//  Created by Diego Canales on 5/26/14.
//
//

#ifndef __GEL__BristleBrushController__
#define __GEL__BristleBrushController__

#include <iostream>


#include "chai3d.h"
#include "UtensilController.h"
#include "GEL3D.h"


using namespace chai3d;
using namespace std;

class BristleBrushController : public UtensilController
{

    
public:
    BristleBrushController(cWorld *newWorld, cMesh *newCanvas, string newResourceRoot, shared_ptr<cGenericHapticDevice> newHapticDevice);
    virtual void updateHaptics(double time, cVector3d pos, double deviceForceScale);
    virtual void updateGraphics();
    
};

#endif /* defined(__GEL__BristleBrushController__) */
