//
//  BristleBrushController.cpp
//  GEL
//
//  Created by Diego Canales on 5/26/14.
//
//

#include "BristleBrushController.h"

BristleBrushController::BristleBrushController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice)  : UtensilController(newWorld, newCanvas, resourceRoot, newHapticDevice) {
    
    naturalSpringLength = sphereRadius / 1.5;
    
    sphereCenter = cVector3d(-.05,0, 0);
    
    //    earth = new cMesh();
    //    cCreateSphere(earth, earthRadius);
    //    earth->setLocalPos(sphereCenter);
    //   // world->addChild(earth);
    //
    //    // set graphic properties
    //    bool fileload;
    //    earth->m_texture = cTexture2d::create();
    //    fileload = earth->m_texture->loadFromFile(RESOURCE_PATH("resources/images/sand.jpg"));
    //    if (!fileload)
    //    {
    //#if defined(_MSVC)
    //        fileload = earth->m_texture->loadFromFile("../../../bin/resources/images/sand.jpg");
    //#endif
    //    }
    //    if (!fileload)
    //    {
    //        cout << "Error - Texture image failed to load correctly." << endl;
    //        close();
    //        return (-1);
    //    }
    //
    //    // enable texture mapping
    //    earth->setUseTexture(true);
    //    earth->m_material->setWhite();
    //
    //    // create normal map from texture data
    //    cNormalMapPtr normalMap0 = cNormalMap::create();
    //    normalMap0->createMap(earth->m_texture);
    //    earth->m_normalMap = normalMap0;
    //
    //    // set haptic properties
    //    earth->m_material->setStiffness(0.8 * 120);
    //    earth->m_material->setStaticFriction(0.3);
    //    earth->m_material->setDynamicFriction(0.2);
    //    earth->m_material->setTextureLevel(1.0);
    //    earth->m_material->setHapticTriangleSides(true, false);
    
    cout << "ADDED" << endl;
    
    double distBetweenSpheres = sphereRadius;
    
    
    double startingXDist = distBetweenSpheres * x_dimension / 2 - distBetweenSpheres / 2;
    double startingYDist = distBetweenSpheres * y_dimension / 2 - distBetweenSpheres / 2;
    
    surroundingObject = new cMultiMesh();
    for (int i = 0; i < x_dimension; i++) {
        for (int j = 0; j < y_dimension; j++) {
            for (int k = 0; k < z_dimension; k++) {
                cMesh *sphere = new cMesh();
                cVector3d sphereOrigin = cVector3d(0 + i * distBetweenSpheres, -startingYDist + j * distBetweenSpheres, startingXDist - k * distBetweenSpheres );
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

double dotProduct(cVector3d& vec1, cVector3d& vec2) {
    return vec1.get(0) * vec2.get(0) + vec1.get(1) * vec2.get(1) + vec1.get(2) * vec2.get(2);
}

cVector3d BristleBrushController::calculateForces(cVector3d currSpherePos, cVector3d fixedSphere, double sprCons, cVector3d velocity) {
    cVector3d totalForce(0,0,0);
    cVector3d vec_p0_q0 = currSpherePos - fixedSphere;
    cVector3d uVec_p0_q0 = vec_p0_q0 / vec_p0_q0.length();
    
    double differenceInLength = vec_p0_q0.length() - naturalSpringLength;
    cVector3d gravDir = cVector3d(-1, 0, 0);
    
    cVector3d gravity = mass * 9.8 * gravDir;
    totalForce += gravity;
    
    cVector3d force0 = -sprCons * differenceInLength * uVec_p0_q0 / 2;
    totalForce += force0;
    
    cVector3d damping = -damperConstant  * (dotProduct(velocity, uVec_p0_q0)) * uVec_p0_q0 / 2;
    totalForce += damping;
    return totalForce;
}

void BristleBrushController::updateHaptics(double time, cVector3d position, double deviceForceScale) {
    double d = earthRadius;
    
    bool contact = false;
    //Calculate forces on spheres
    //        for (int a = 0; a < x_dimension; a++) {
    //            for (int b = 0; b < y_dimension; b++) {
    //                for (int i = 0; i < z_dimension; i++) {
    //                    cVector3d dist = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i] - sphereCenter;
    //                    cVector3d f(0,0,0);
    //                    if (dist.length() < d ) {
    //                        contact = true;
    //                    }
    //                }
    //            }
    //        }
    
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
                            if (x + k < 0 || x + k > x_dimension - 1) continue;
                            cVector3d fixedSphere;
                            double sprCons = springConstant;
                            if (k == 0) {
                                //Grid forces
                                if (x > x_dimension / 4 )  {
                                    cVector3d pos = position + cVector3d(0, -startingDist + y * sphereRadius, startingDist - z * sphereRadius);
                                    fixedSphere = cVector3d(pos.x() - .002 * double(k) - sphereRadius * (x_dimension - x + 3), pos.y() + .002 * (double)l, pos.z()  + .002 * (double)m);
                                } else {
                                    //FORCES BETWEEN BOTTOM SPHERES
                                    continue;
                                    if (x + k >= 0 && y + l >= 0 && x + k < x_dimension && y + l < y_dimension) {
                                        fixedSphere = spheresArray[x][y + l][z + m]->getLocalPos() + originArray[x][y + l][z + m];
                                        sprCons /= 10;
                                    } else {
                                        continue;
                                    }
                                }
                            }
                            else {
                                //Spring forces between spheres
                                fixedSphere = spheresArray[x + k][y][z]->getLocalPos() + originArray[x + k][y][z];
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
            }
        }
    }
    
    
    
    /////////////////////////////////////////////////////////////////////
    // COMPUTE FORCES
    /////////////////////////////////////////////////////////////////////
    
    cVector3d force = cVector3d(0,0,0);
    
    cVector3d dist = position - sphereCenter;
    cVector3d normal = cNormalize(dist);
    double k = 3000;
    //
    //        //Cursor force
    //        if (dist.length() < d) {
    //            force += -k * (dist.length() - d) * normal;
    //        }
    //
    //        //Calculate forces on spheres
    //        for (int a = 0; a < x_dimension; a++) {
    //            for (int b = 0; b < y_dimension; b++) {
    //                for (int i = 0; i < z_dimension; i++) {
    //                    cVector3d dist = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i] - sphereCenter;
    //                    cVector3d normal = cNormalize(dist);
    //                    //Sphere force
    //                    cVector3d f(0,0,0);
    //                    if (dist.length() < d) {
    //
    //                        f += -k * (dist.length() - d) * normal;
    //                        cVector3d acc = f / mass;
    //                        accArray[a][b][i] += acc;
    //                    }
    //                }
    //            }
    //        }
    
    
    
    //Cursor force
    if (position.x() < planePos.x()) {
        force += -k * (position.x() - planePos.x()) * cVector3d(1, 0, 0);
    }
    
        //Calculate forces on spheres
        for (int a = 0; a < x_dimension; a++) {
            for (int b = 0; b < y_dimension; b++) {
                for (int i = 0; i < z_dimension; i++) {
                    cVector3d pos = spheresArray[a][b][i]->getLocalPos() + originArray[a][b][i];
                    //Sphere force
                    cVector3d f(0,0,0);
                    if (pos.x() < planePos.x()) {
                        f += -k * (pos.x() - planePos.x()) * cVector3d(1, 0,0);
                        cVector3d acc = f / mass;
                        accArray[a][b][i] += acc;
                    }
                }
            }
        }


        //Add forces of spheres to cursor
        for (int a = 0; a < x_dimension; a++) {
            for (int b = 0; b < y_dimension; b++) {
                for (int c = 0; c < z_dimension; c++) {
                cVector3d f = accArray[a][b][c] * mass;
                force += f * ((double)a * 1.0 / pow(x_dimension, paintBrushWeakness));
                }
            }
        }

    

    surroundingObject->updateBoundaryBox();
    
    
    hapticDevice->setForce(force);
}

void BristleBrushController::updateGraphics() {
    
}