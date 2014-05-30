//
//  PencilController.cpp
//  GEL
//
//  Created by Will Harvey on 5/29/14.
//
//

#include "PencilController.h"

#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

cVector3d PencilController::computeForceOnPalette(const cVector3d& a_spherePos,
                                                     double a_radius,
                                                     double a_stiffness) {
    // compute the reaction forces between the tool and the ith sphere.
    cVector3d force;
    force.zero();
    
    double xDist = abs(a_spherePos.x() - canvas->getLocalPos().x());
    
    // check if both objects are intersecting
    if (xDist < 0.0000001)
    {
        return (force);
    }
    
    if (xDist > (a_radius))
    {
        return (force);
    }
    
    // compute penetration distance between tool and surface of sphere
    double penetrationDistance = a_radius - xDist;
    
    // cVector3d forceDirection = cVector3d(-1, 0, 0);
    cVector3d forceDirection = getNormalAtPosition(a_spherePos);
    force = cMul( penetrationDistance * a_stiffness, forceDirection);
    
    // return result
    
    return (force);
    
}

void PencilController::updateGraphics() {
    
}


PencilController::PencilController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice)  : UtensilController(newWorld, newCanvas, resourceRoot, newHapticDevice) {
    
    
    radius = 0.5;
    stiffness = 3000;
    
    
    tip = new cShapeCylinder(0.001, 0.1, tipLength);
    tip->rotateAboutGlobalAxisDeg(cVector3d(0,1,0), 90);
    tip->m_material->setGrayDarkSlate();
    world->addChild(tip);
    
    cylinder = new cShapeCylinder(0.1, 0.1, 1.4);
    cylinder->rotateAboutGlobalAxisDeg(cVector3d(0,1,0), 90);
    cylinder->m_material->setYellowLemonChiffon();
    world->addChild(cylinder);
}


cVector3d PencilController::getNormalAtPosition(cVector3d pos) {
    int px, py;
    cVector3d newCoord;
    
    newCoord.x((pos.y() - canvas->getLocalPos().y() + canvasSize/2)/canvasSize);
    newCoord.y((pos.z() - canvas->getLocalPos().z() + canvasSize/2)/canvasSize);
    
    newCoord.z(0);
    canvas->m_normalMap->m_image->getPixelLocation(newCoord, px, py);
    
    
    cColorb grad;
    canvas->m_normalMap->m_image->getPixelColor(px, py, grad);
    
    
    cVector3d g = cVector3d((double)grad.getR(), (double)grad.getG(), (double)grad.getB());
    //                    cout << "Gradient is " << cNormalize(g) << endl;
    const double SCALE = (1.0/255.0);
    double fX00 = SCALE * (grad.getR() - 128);
    double fY00 =-SCALE * (grad.getG() - 128);
    double fZ00 = SCALE * (grad.getB() - 128);
    
    double pi = 3.14159;
    cMatrix3d rotAboutY = cMatrix3d(cos(pi / 2), 0, sin(pi / 2),0 , 1, 0, -sin(pi / 2), 0, cos(pi / 2));
    cMatrix3d rotAboutX = cMatrix3d(1, 0, 0, 0 , cos(pi / 2), -sin(pi / 2), 0, sin(pi / 2), cos(pi / 2));
    
    g.set(fX00, fY00, fZ00);
    g = rotAboutY * g;
    g = rotAboutX * g;
    g = cNormalize(g);
    // cout << "g is " << g << endl;
    return g;
}


void PencilController::updateHaptics(double time, cVector3d pos, double deviceForceScale) {
    // clear all external forces
    
    cVector3d tipPos = pos;
    tipPos.x(tipPos.x() - tipLength/2 - 0.5);
    
    
    cVector3d cylPos = pos;
    cylPos.x(cylPos.x() + tipLength/2 - 0.5);
    
    
    cVector3d actualPos = pos;
    actualPos.x(actualPos.x() - .16);
    
    tip->setLocalPos(tipPos);
    cylinder->setLocalPos(cylPos);
    cVector3d f = computeForceOnPalette(actualPos, radius, stiffness);
    
    if (f.length() > 0) {
        thinDrawAtPoint(cylPos, f.length(), time, false, false);
    }
    
    canvas->m_texture->markForUpdate();

    f.mul(deviceForceScale);
    hapticDevice->setForce(f);
    
    
//
//    moveNodesToCursor(pos, true);
//    
//    
//    cVector3d force(0.0, 0.0, 0.0);
//    for (int y=0; y<numYNodes; y++)
//    {
//        for (int x=0; x<numXNodes; x++)
//        {
//            cVector3d nodePos = nodes[x][y]->m_pos;
//            
//            cVector3d f = computeForceOnPalette(nodePos, radius, stiffness);
//            //Got rid of the -1 multiplication here (used to be cVector3d tmpfrc = -1 * f)
//            cVector3d tmpfrc = f;
//            
//            force += tmpfrc * ((double) 3 * x * 1.0 / pow(numXNodes, 1));
//            
//            //if (x == numXNodes/2+1 && f.length() > 0) {
//            
//            if (f.length() > 0) {
//                
//                if (y != 0) {
//                    
//                    
//                    drawBetweenPoints(nodePos, nodes[x][y - 1]->m_pos, f.length(), time);
//                    
//                    
//                    //drawAtPoint(nodePos, f.length(), time, (y == 0), (y == numYNodes - 1));
//                } else {
//                    //drawBetweenPoints(nodePos, nodes[x][y - 1]->m_pos, f.length(), time, (y == 0), (y == numYNodes - 1));
//                }
//                
//            }
//            nodes[x][y]->setExternalForce(tmpfrc);
////        }
////    }
//    
//    // update texture
//    canvas->m_texture->markForUpdate();
//    
//    //Integrate dynamics
//    defWorld->updateDynamics(time);
//    
//    //Send force back to haptic device
//    force.mul(deviceForceScale);
//    
//    hapticDevice->setForce(force);
}

void PencilController::removeFromWorld() {
    world->removeChild(cylinder);
}
