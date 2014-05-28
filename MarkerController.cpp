//
//  MarkerController.cpp
//  GEL
//
//  Created by Diego Canales on 5/26/14.
//
//

#include "MarkerController.h"

MarkerController::MarkerController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice)  : UtensilController(newWorld, newCanvas, resourceRoot, newHapticDevice) {
    
    naturalSpringLength = sphereRadius / 2.2;
    
    sphereCenter = cVector3d(-.05,0, 0);

    
    cout << "ADDED" << endl;
    
    double distBetweenSpheres = sphereRadius * 2;
    
    
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
    world->addChild(surroundingObject);
    
    
    planePos = canvas->getLocalPos();
    //    plane = new cMesh();
    //    cCreatePlane(plane, planeWidth, planeHeight);
    //    planePos = cVector3d(-.01,0,0);
    //    plane->setLocalPos(planePos);
    //    plane->m_material->setRed();
    //    plane->setLocalRot(cMatrix3d(cDegToRad(0), cDegToRad(90), cDegToRad(0), C_EULER_ORDER_XYZ));
    //    world->addChild(plane);
    
    
    
    
    
}
//
//double dotProduct(cVector3d& vec1, cVector3d& vec2) {
//    return vec1.get(0) * vec2.get(0) + vec1.get(1) * vec2.get(1) + vec1.get(2) * vec2.get(2);
//}

cVector3d MarkerController::calculateForces(cVector3d currSpherePos, cVector3d fixedSphere, double sprCons, cVector3d velocity) {
    cVector3d totalForce(0,0,0);
    cVector3d vec_p0_q0 = currSpherePos - fixedSphere;
    cVector3d uVec_p0_q0 = vec_p0_q0 / vec_p0_q0.length();
    
    double differenceInLength = vec_p0_q0.length() - naturalSpringLength;
    cVector3d gravDir = cVector3d(-1, 0, 0);
    
    cVector3d gravity = mass * 9.8 * gravDir;
    totalForce += gravity;
    
    cVector3d force0 = -sprCons * differenceInLength * uVec_p0_q0 / 2;
    totalForce += force0;
    
    double dotProd = velocity.get(0) * uVec_p0_q0.get(0) + velocity.get(1) * uVec_p0_q0.get(1) + velocity.get(2) * uVec_p0_q0.get(2);
    // cVector3d damping = -damperConstant  * (dotProduct(velocity, uVec_p0_q0)) * uVec_p0_q0 / 2;
    cVector3d damping = -damperConstant  * dotProd * uVec_p0_q0 / 2;
    totalForce += damping;
    return totalForce;
}

void MarkerController::updateHaptics(double time, cVector3d position, double deviceForceScale) {
    double d = earthRadius;
    double k = 9000;
    bool contact = false;
    
    for (int a = 0; a < x_dimension; a++) {
        for (int b = 0; b < y_dimension; b++) {
            for (int i = 0; i < z_dimension; i++) {
                cVector3d spherePos = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i];
                if (spherePos.x() < planePos.x()) {
                    contact = true;
                }
            }
        }
    }
    
    
    cVector3d force = cVector3d(0,0,0);
    double gridSpringSize = .002;
    double massMultiplierCons = 1;
    for (int x = 0; x < x_dimension; ++x) {
        for (int y = 0; y < y_dimension; ++y) {
            for (int z = 0; z < z_dimension; ++z) {
                cVector3d currSpherePos = spheresArray[x][y][z]->getLocalPos() + originArray[x][y][z];
                cVector3d previousForce = cVector3d(0,0,0);
                int surroundingSphereRadius = 1;
                
                //Calculate "grid" forces
                for (int k = -surroundingSphereRadius; k < surroundingSphereRadius + 1; ++k) {
                    for (int l = -surroundingSphereRadius; l < surroundingSphereRadius + 1; ++l) {
                        for (int m = -surroundingSphereRadius; m < surroundingSphereRadius + 1; ++m) {
                            
                            //These "continue" statements ensure that we don't calculate diagonal springs, that we don't calculate a spring from a ball to itself, and that we don't exceed the bounds in the z direction
                            if (k == 0 && l == 0 && m == 0) continue;
                            if (k * l * m != 0) continue;
                            cVector3d fixedSphere;
                            double sprCons = springConstant;
                            if (k == 0) {
                                //Grid forces
                                cVector3d pos = position + cVector3d(0, -startingYDist + y * sphereRadius, startingZDist - z * sphereRadius);
                                fixedSphere = cVector3d(pos.x() - .002 * double(k) - sphereRadius * (x_dimension - x + 1), pos.y() + .002 * (double)l, pos.z()  + .002 * (double)m);
                                

                            }
                            //Apply spring forces
                            previousForce += calculateForces(currSpherePos, fixedSphere, sprCons, velArray[x][y][z]);
                            
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
                
                cVector3d pos = spheresArray[x][y][z]->getLocalPos() + originArray[x][y][z];
                
                //Sphere force
                cVector3d canvasForce(0,0,0);
                if (pos.x() < planePos.x()) {
                    // cout << "this pos " << pos.x() << endl;
                    canvasForce += -k * (pos.x() - planePos.x()) * cVector3d(1, 0,0);
                    cVector3d acc = canvasForce / mass;
                    accArray[x][y][z] += acc;
                    force += canvasForce * ((double)x * 1.0 / pow(x_dimension, paintBrushWeakness));
                }
            }
        }
    }
    
    
    
    /////////////////////////////////////////////////////////////////////
    // COMPUTE FORCES
    /////////////////////////////////////////////////////////////////////
    
    
    
    cVector3d dist = position - sphereCenter;
    cVector3d normal = cNormalize(dist);
    
    
    
    //Cursor force
    if (position.x() < planePos.x()) {
        force += -k * (position.x() - planePos.x()) * cVector3d(1, 0, 0);
    }
    
    //    //Calculate forces on spheres
    //    for (int a = 0; a < x_dimension; a++) {
    //        for (int b = 0; b < y_dimension; b++) {
    //            for (int i = 0; i < z_dimension; i++) {
    //                cVector3d pos = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i];
    //                //Sphere force
    //                cVector3d f(0,0,0);
    //                if (pos.x() < planePos.x()) {
    //                    // cout << "this pos " << pos.x() << endl;
    //                    f += -k * (pos.x() - planePos.x()) * cVector3d(1, 0,0);
    //                    cVector3d acc = f / mass;
    //                    accArray[a][b][i] += acc;
    //                    force += f * ((double)a * 1.0 / pow(x_dimension, paintBrushWeakness));
    //                }
    //            }
    //        }
    //    }
    
//    for (int x = 0; x < x_dimension; x++) {
//        for (int x_2 = 0; x_2 < x_dimension;x_2++) {
//            if (x == x_2) continue;
//            cVector3d currSpherePos = spheresArray[x][0][0]->getLocalPos() + originArray[x][0][0];
//            cVector3d otherSpherePos = spheresArray[x_2][0][0]->getLocalPos() + originArray[x_2][0][0];
//            cVector3d vec_between = currSpherePos - otherSpherePos;
//            cVector3d f(0, 0, 0);
//            if (vec_between.length() < sphereRadius ) {
//                 cout << "CONTACT" << endl;
//                f = -k / 5 * (vec_between.length() - sphereRadius) * cNormalize(vec_between);
//                cVector3d acc = f / mass;
//                accArray[x][0][0] += acc;
//            }
//            
//        }
//    }
    
    surroundingObject->updateBoundaryBox();
    force.mul(deviceForceScale);
    
    hapticDevice->setForce(force);
}

void MarkerController::updateGraphics() {
    
}