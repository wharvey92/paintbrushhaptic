//
//  RealBrushController.cpp
//  GEL
//
//  Created by Diego Canales on 5/26/14.
//
//

#include "RealBrushController.h"

RealBrushController::RealBrushController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice)  : UtensilController(newWorld, newCanvas, resourceRoot, newHapticDevice) {
    
    
    cylinder = new cShapeCylinder(0.1, 0.1, 1.4);
    cylinder->rotateAboutGlobalAxisDeg(cVector3d(0,1,0), 90);
    newWorld->addChild(cylinder);
    
    box = new cShapeBox(0.15, .45, 0.15);
    newWorld->addChild(box);
    
    naturalSpringLength = sphereRadius / 2.2;
    
    sphereCenter = cVector3d(-.05,0, 0);

    frictionOn = false;
    double distBetweenSpheres = sphereRadius;
    
    
    startingYDist = distBetweenSpheres * y_dimension / 2 - distBetweenSpheres / 2;
    startingZDist = distBetweenSpheres * z_dimension / 2 - distBetweenSpheres / 2;
    
    
    surroundingObject = new cMultiMesh();
    for (int i = 0; i < x_dimension; i++) {
        for (int j = 0; j < y_dimension; j++) {
            for (int k = 0; k < z_dimension; k++) {
                cMesh *sphere = new cMesh();
                cVector3d sphereOrigin = cVector3d(0 + i * distBetweenSpheres, -startingYDist + j * distBetweenSpheres, startingZDist - k * distBetweenSpheres );
                cCreateSphere(sphere, sphereRadius, 32, 32, sphereOrigin);

                
                spheresArray[i][j][k] = sphere;
                // world->addChild(sphere);
                velArray[i][j][k] = cVector3d(0,0,0);
                accArray[i][j][k] = cVector3d(0,0,0);
                originArray[i][j][k] = sphereOrigin;
                surroundingObject->addMesh(sphere);
                //  sphere->createAABBCollisionDetector(sphereRadius);
            }
        }
    }
    
    //  surroundingObject->setShowBoundaryBox(true);
    surroundingObject->m_material->setRed();
    newWorld->addChild(surroundingObject);
    
    
    planePos = canvas->getLocalPos();
    //    plane = new cMesh();
    //    cCreatePlane(plane, planeWidth, planeHeight);
    //    planePos = cVector3d(-.01,0,0);
    //    plane->setLocalPos(planePos);
    //    plane->m_material->setRed();
    //    plane->setLocalRot(cMatrix3d(cDegToRad(0), cDegToRad(90), cDegToRad(0), C_EULER_ORDER_XYZ));
    //    world->addChild(plane);
    
    
    
    
    
}

cVector3d RealBrushController::calculateForces(cVector3d currSpherePos, cVector3d fixedSphere, double sprCons, cVector3d velocity, double sprLength, bool lengthOn) {
    cVector3d totalForce(0,0,0);
    cVector3d vec_p0_q0 = currSpherePos - fixedSphere;
    cVector3d uVec_p0_q0 = vec_p0_q0 / vec_p0_q0.length();
    
    double differenceInLength = vec_p0_q0.length() - sprLength;
    if (lengthOn) {
        if (differenceInLength < 0) return cVector3d(0,0,0);
    }
    cVector3d gravDir = cVector3d(-1, 0, 0);
    
    cVector3d gravity = mass * 9.8 * gravDir;
    totalForce += gravity;
    
    cVector3d force0 = -sprCons * differenceInLength * uVec_p0_q0 / 2;
    totalForce += force0;
    double dotProd = velocity.get(0) * uVec_p0_q0.get(0) + velocity.get(1) * uVec_p0_q0.get(1) + velocity.get(2) * uVec_p0_q0.get(2);
    
    cVector3d damping = -damperConstant  * dotProd * uVec_p0_q0 / 2;
    totalForce += damping;
    return totalForce;
}

void RealBrushController::updateHaptics(double time, cVector3d position, double deviceForceScale) {
    double d = earthRadius;
    
    cVector3d positionOfBoxAndCylinder = cVector3d(position.x() - .13, position.y() - .1, position.z());
    
    cylinder->setLocalPos(positionOfBoxAndCylinder);
    box->setLocalPos(positionOfBoxAndCylinder);
    
    bool contact = false;
    
        cVector3d force = cVector3d(0,0,0);
    
    //Draw paint for spheres in contact
    double numInContact = 0;
    for (int a = 0; a < x_dimension; a++) {
        for (int b = 0; b < y_dimension; b++) {
            for (int i = 0; i < z_dimension; i++) {
                cVector3d spherePos = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i];
                if (spherePos.x() < planePos.x()) {
                    contact = true;
                    numInContact++;
                  //  cout << "velocity " << velArray[a][b][i] << endl;
                    if (frictionOn) {
                        cVector3d tempVel = cVector3d(0, velArray[a][b][i].get(1), velArray[a][b][i].get(2));
                        double distIn = spherePos.x() - planePos.x();
                        if (tempVel.length() > .2) {
                            force += (-abs(distIn) / 2) * tempVel;
                            
                        } else {
                            force += -abs(distIn) * 2 * tempVel;
                        }
                    }
                    
//                    cVector3d currForce = accArray[a][b][i] * mass;
//                    cout << "Calling 1 " << endl;
//                    cout << "drawing at pos "<< spherePos << endl;
//                    drawAtPoint(spherePos, currForce.length(), time, true, true);
                }
            }
        }
    }
    
    double k = 700;
    

    
    
    double massMultiplierCons = 1;
    for (int x = 0; x < x_dimension; ++x) {
        for (int y = 0; y < y_dimension; ++y) {
            for (int z = 0; z < z_dimension; ++z) {
                cVector3d currSpherePos = spheresArray[x][y][z]->getLocalPos() + originArray[x][y][z];
                cVector3d previousForce = cVector3d(0,0,0);
                int surroundingSphereRadius = 1;
                double sprLength = naturalSpringLength;
                
                //Calculate "grid" forces
                for (int k = -surroundingSphereRadius; k < surroundingSphereRadius + 1; ++k) {
                    for (int l = -surroundingSphereRadius; l < surroundingSphereRadius + 1; ++l) {
                        for (int m = -surroundingSphereRadius; m < surroundingSphereRadius + 1; ++m) {
                            
                            //These "continue" statements ensure that we don't calculate diagonal springs, that we don't calculate a spring from a ball to itself, and that we don't exceed the bounds in the z direction
                            if (k == 0 && l == 0 && m == 0) continue;
                            if (k * l * m != 0) continue;
                            if (x + k < 0 || x + k > x_dimension - 1) continue;
                            cVector3d fixedSphere;
                            double sprCons = springConstant;
                            bool len = false;
                            if (k == 0) {
                                //Grid forces
                                
                                cVector3d pos = position + cVector3d(0, -startingYDist + y * sphereRadius * (x_dimension - x) / 4, startingZDist - z * sphereRadius);
                                fixedSphere = cVector3d(pos.x() - .002 * double(k) - sphereRadius * (x_dimension - x + 1), pos.y() + .002 * (double)l, pos.z()  + .002 * (double)m);
                                // sprLength = .004;
                                sprCons /= (3 * (x_dimension - x) * (x_dimension - x));
                                // bool len = true;
                                
                            }
                            else {
                                //Spring forces between spheres
                                
                                fixedSphere = spheresArray[x + k][y][z]->getLocalPos() + originArray[x + k][y][z];
                            }
                            
                            //Apply spring forces
                            // cout << "x is  " << x << " and spr cons is " << sprCons << endl;
                            previousForce += calculateForces(currSpherePos, fixedSphere, sprCons, velArray[x][y][z], sprLength, len);
                            
                        }
                    }
                }
                
                //Add forces to the acc array.  Also update velocity and position
                cVector3d f = mass * accArray[x][y][z];
                cVector3d airDamping = -10 * velArray[x][y][z];
                f += airDamping;
                accArray[x][y][z] += f / mass;
                accArray[x][y][z] += previousForce / mass;
                velArray[x][y][z] = velArray[x][y][z] + accArray[x][y][z] * timeStep;
                accArray[x][y][z] = cVector3d(0,0,0);
                cVector3d newPos = currSpherePos + velArray[x][y][z] * timeStep;
                spheresArray[x][y][z]->setLocalPos(newPos - originArray[x][y][z]);
                
                
                
            }
        }
    }
    
    
    //    cVector3d middleBall = spheresArray[0][y_dimension / 2][0]->getLocalPos() + originArray[0][y_dimension / 2][0];
    //    force += calculateForces(position, middleBall, (double)springConstant, 0, naturalSpringLength);
    //
    
    
    /////////////////////////////////////////////////////////////////////
    // COMPUTE FORCES
    /////////////////////////////////////////////////////////////////////
    

    //  cout << "num is " << numInContact << endl;
    //Add forces of spheres to cursor
    for (int a = 0; a < x_dimension; a++) {
        for (int b = 0; b < y_dimension; b++) {
            for (int c = 0; c < z_dimension; c++) {
                cVector3d pos = spheresArray[a][b][c]->getLocalPos() + originArray[a][b][c];
                //Sphere force
                cVector3d canvasForce(0,0,0);
                if (pos.x() < planePos.x()) {
                    
                    
                    cVector3d g = getNormalAtPosition(pos);
                    canvasForce += -k * (pos.x() - planePos.x()) * g;

//                    cout << "Calling 2" << endl;
//                    cout << "position is " << pos << endl;
                    
                     if (canvasForce.length() > 0) drawAtPoint(pos, canvasForce.length(), time, (b == 0), (b == y_dimension - 1));
                    
                    
                    cVector3d acc = canvasForce / mass;
                    accArray[a][b][c] += acc;
                    //force += canvasForce * (double)(((x + 1) / 7.0) * 1.2);// * 1.0 / pow(x_dimension, paintBrushWeakness));
                    
                    cVector3d tempVel = cVector3d(0, velArray[a][b][c].get(1), velArray[a][b][c].get(2));
                    force += canvasForce * ((double)(a + 1) * 1.0 / pow(x_dimension, paintBrushWeakness * 1.6)) * 3 ;

                }

            }
        }
    }


    
    surroundingObject->updateBoundaryBox();
    
    
    
  //  force = accArray[x_dimension - 1][1][0] * mass;
//    
//    cVector3d mid = spheresArray[x_dimension - 1][1][0]->getLocalPos() + originArray[x_dimension - 1][1][0];
//    cVector3d p;
//    cVector3d v;
//    hapticDevice->getPosition(p);
//    hapticDevice->getLinearVelocity(v);
//    cVector3d f = calculateForces(p, mid, springConstant, v, naturalSpringLength * 4, true);
    
    // update texture
    
//    if (numInContact > 0) {
//        if (frictionOn) {
//            cVector3d tempVel;
//            hapticDevice->getLinearVelocity(tempVel);
//            cVector3d pos = spheresArray[0][0][0]->getLocalPos() + originArray[0][0][0];
//            double distIn = pos.x() - planePos.x();
//            cVector3d canvasForce = -k * (pos.x() - planePos.x()) * cVector3d(1, 0, 0);
//            if (tempVel.length() > .03) {
//                force += (-canvasForce.length() * cNormalize(tempVel) * .06);
//            } else {
//                force += (-canvasForce.length() * tempVel * .3);
//            }
//        }
//    }
    canvas->m_texture->markForUpdate();
    
    //Integrate dynamics    
    //Send force back to haptic device
    force.mul(deviceForceScale);
    
    hapticDevice->setForce(force);
}


void RealBrushController::updateGraphics() {
    
}

void RealBrushController::removeFromWorld() {
    world->removeChild(surroundingObject);
    world->removeChild(box);
    world->removeChild(cylinder);
}

