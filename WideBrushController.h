//
//  WideBrushController.h
//  GEL
//
//  Created by Will Harvey on 5/22/14.
//
//

#ifndef __GEL__WideBrushController__
#define __GEL__WideBrushController__

#include <iostream>


#include "chai3d.h"
#include "UtensilController.h"
#include "GEL3D.h"


using namespace chai3d;
using namespace std;

#define numY 8
#define numX 5


class WideBrushController : public UtensilController
{
    int numYNodes = numY;
    int numXNodes = numX;
    cGELSkeletonNode* nodes[numX][numY];
    int numFixedRows = 2;
    
    // deformable world
    cGELWorld* defWorld;
    
    // object mesh
    cGELMesh* defObject;
    
    // haptic device model
    cShapeSphere* device;
    double deviceRadius;
    
    // radius of the dynamic model sphere (GEM)
    double radius;
    
    // stiffness properties between the haptic device tool and the model (GEM)
    double stiffness;
    
    cShapeCylinder *cylinder;
    cVector3d getNormalAtPosition(cVector3d pos);
    cShapeBox *box;
    
    
public:
    WideBrushController(cWorld *newWorld, cMesh *newCanvas, string newResourceRoot, shared_ptr<cGenericHapticDevice> newHapticDevice);
    virtual void updateHaptics(double time, cVector3d pos, double deviceForceScale);
    virtual void updateGraphics();
    virtual void removeFromWorld();
    
private:
    void moveNodesToCursor(const cVector3d pos, bool onlyFixed);
    cVector3d computeForceOnPalette(const cVector3d& a_spherePos,
                                                         double a_radius,
                                                         double a_stiffness);
    
};

#endif /* defined(__GEL__WideBrushController__) */
