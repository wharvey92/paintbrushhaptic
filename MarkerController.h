//
//  MarkerController.h
//  GEL
//
//  Created by Diego Canales on 5/26/14.
//
//

#ifndef __GEL__MarkerController__
#define __GEL__MarkerController__

#include <iostream>


#include "chai3d.h"
#include "UtensilController.h"
#include "GEL3D.h"


using namespace chai3d;
using namespace std;

class MarkerController : public UtensilController
{
    cPrecisionClock *clock0;
    
    const int y_dimension = 1;
    const int x_dimension = 4;
    const int z_dimension = 1;
    cMultiMesh *surroundingObject;
    double box_radius = .08;
    double startingDist;
    int scene = 0;
    cMesh *spheresArray[4][1][1];
    cVector3d accArray[4][1][1];
    cVector3d velArray[4][1][1];
    cVector3d originArray[4][1][1];
    
    cMesh* object0;
    
    double earthRadius = .05;
    
    cVector3d planePos;
    
    
    cVector3d box_center;
    
    
    double startingYDist;
    double startingZDist;
    
    cMesh *plane;
    bool gravityInward = false;
    
    double planeHeight = .04;
    double planeWidth = .04;
    
    cVector3d sphereCenter;
    
    double paintBrushWeakness = 1.1;
    
    double sphereRadius = .05;
    double lineHeight;
    double naturalSpringLength;
    
    double cursorRadius = .01;
    
    
    double timeStep = .001;
    
    double springConstant = 11000;
    double damperConstant = 10;
    
    double mass = .08;
    
    cVector3d calculateForces(cVector3d currSpherePos, cVector3d fixedSphere, double sprCons, cVector3d velocity);
    
    
public:
    MarkerController(cWorld *newWorld, cMesh *newCanvas, string newResourceRoot, shared_ptr<cGenericHapticDevice> newHapticDevice);
    virtual void updateHaptics(double time, cVector3d position, double deviceForceScale);
    virtual void updateGraphics();
    
};

#endif /* defined(__GEL__MarkerController__) */
