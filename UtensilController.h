//
//  UtensilController.h
//  GEL
//
//  Created by Will Harvey on 5/22/14.
//
//

#ifndef __GEL__UtensilController__
#define __GEL__UtensilController__

#include <iostream>

#include "chai3d.h"

using namespace chai3d;
using namespace std;

class UtensilController
{
    
protected: shared_ptr<cGenericHapticDevice> hapticDevice;
protected: cMesh* canvas;
protected: cWorld *world;
protected: cColorb paintColor;
protected: double canvasSize;
    
public:
    UtensilController(cWorld *newWorld, cMesh *newCanvas, string newResourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice);
    virtual void updateHaptics(double time, cVector3d pos, double deviceForceScale);
    virtual void updateGraphics();
    void setCanvasSize(double size);
        
protected:
    void drawAtPoint(const cVector3d texCoord, double force, double timeInterval, bool left, bool right);
};


#endif /* defined(__GEL__UtensilController__) */