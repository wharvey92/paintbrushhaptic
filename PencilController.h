

#ifndef __GEL__PencilController__
#define __GEL__PencilController__

#include <iostream>


#include "chai3d.h"
#include "UtensilController.h"
#include "GEL3D.h"


using namespace chai3d;
using namespace std;

class PencilController : public UtensilController {

    cShapeCylinder *cylinder;
    cShapeCylinder *tip;
    float tipLength = 0.3;
    
    cVector3d computeForceOnPalette(const cVector3d& a_spherePos,
                                    double a_radius,
                                    double a_stiffness);
    cVector3d getNormalAtPosition(cVector3d pos);
    
    
    double radius;
    
    // stiffness properties between the haptic device tool and the model (GEM)
    double stiffness;


public:
    PencilController(cWorld *newWorld, cMesh *newCanvas, string newResourceRoot, shared_ptr<cGenericHapticDevice> newHapticDevice);
    virtual void updateHaptics(double time, cVector3d position, double deviceForceScale);
    virtual void updateGraphics();
    virtual void removeFromWorld();
    
};

#endif /* defined(__GEL__PencilController__) */
