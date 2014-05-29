//
//  WideBrushController.cpp
//  GEL
//
//  Created by Will Harvey on 5/22/14.
//
//

#include "WideBrushController.h"

#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


void WideBrushController::moveNodesToCursor(const cVector3d pos, bool onlyFixed) {
    for (int y = 0; y < numYNodes; y++) {
        for (int x = 0; x < numXNodes; x++) {
            
            
            
            
            if (!onlyFixed || nodes[x][y]->m_fixed) {
                
                cVector3d offset(-(0.1 * 10 / numXNodes)*(double)x, 0.42 - (0.1 * 10 / numYNodes)*(double)y,0);
                nodes[x][y]->m_pos = pos + offset;
            }
        }
    }
    
    cylinder->setLocalPos(pos);
    box->setLocalPos(pos);
    
}

cVector3d WideBrushController::computeForceOnPalette(const cVector3d& a_spherePos,
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


WideBrushController::WideBrushController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice>newHapticDevice)  : UtensilController(newWorld, newCanvas, resourceRoot, newHapticDevice) {
    
    // interaction stiffness between objects and deformable model
    stiffness = 100;
    
    //-----------------------------------------------------------------------
    // COMPOSE THE VIRTUAL SCENE
    //-----------------------------------------------------------------------
    
    // create a world which supports deformable object
    defWorld = new cGELWorld();
    world->addChild(defWorld);
    
    // create a deformable mesh
    defObject = new cGELMesh();
    defWorld->m_gelMeshes.push_front(defObject);
    
	// load model
    bool fileload = defObject->loadFromFile(RESOURCE_PATH("resources/models/box/box.obj"));
    if (!fileload)
    {
#if defined(_MSVC)
        fileload = defObject->loadFromFile("../../../bin/resources/models/box/box.obj");
#endif
    }
    if (!fileload)
    {
        cout << "Error - 3D Model failed to load correctly." << endl;
        return (-1);
    }
    
    // set some material color on the object
    cMaterial mat;
    mat.setBrownTan();
    mat.setShininess(10);
    defObject->setMaterial(mat, true);
    
    
    // let's create a some environment mapping
    shared_ptr<cTexture2d> texture(new cTexture2d());
    fileload = texture->loadFromFile(RESOURCE_PATH("resources/images/shadow.jpg"));
    if (!fileload)
    {
#if defined(_MSVC)
        fileload = texture->loadFromFile("../../../bin/resources/images/shadow.jpg");
#endif
    }
    if (!fileload)
    {
        cout << "Error - Texture failed to load correctly." << endl;
        return (-1);
    }
    
    // enable environmental texturing
    texture->setEnvironmentMode(GL_DECAL);
    texture->setSphericalMappingEnabled(true);
    
    // assign and enable texturing
    //defObject->setTexture(texture, true);
    //defObject->setUseTexture(true, true);
    
    // set object to be transparent
    defObject->setTransparencyLevel(0.98, true, true);
    
    // build dynamic vertices
    defObject->buildVertices();
    
    
    
    
    // set default properties for skeleton nodes
    cGELSkeletonNode::s_default_radius        = 0.05;  // [m]
    cGELSkeletonNode::s_default_kDampingPos   = 5.5;
    cGELSkeletonNode::s_default_kDampingRot   = 0.6;
    cGELSkeletonNode::s_default_mass          = 0.002; // [kg]
    //cGELSkeletonNode::s_default_showFrame     = true;
    //cGELSkeletonNode::s_default_color.setBlueCornflower();
    cGELSkeletonNode::s_default_useGravity    = true;
    cGELSkeletonNode::s_default_gravity.set(0.00, -9.81, 0.0);
    radius = cGELSkeletonNode::s_default_radius;
    
    // use internal skeleton as deformable model
    defObject->m_useSkeletonModel = true;
    
    // create an array of nodes
    
    //moveNodesToCursor(cVector3d(0,0,0), true);
    
    
    for (int y=0; y<numYNodes; y++)
    {
        for (int x=0; x<numXNodes; x++)
        {
            cGELSkeletonNode* newNode = new cGELSkeletonNode();
            nodes[x][y] = newNode;
            defObject->m_nodes.push_front(newNode);
            newNode->m_pos.set( (0.45 - (0.1 * 10 / numXNodes)*(double)x), (0.43 - (0.1 * 10 / numYNodes)*(double)y), 0.0);
        }
    }
    
    // set corner nodes as fixed
    //    nodes[0][0]->m_fixed = true;
    //    nodes[0][9]->m_fixed = true;
    //    nodes[1][0]->m_fixed = true;
    //    nodes[1][9]->m_fixed = true;
    
    
    for (int y = 0; y < numYNodes; y++) {
        for (int x = 0; x < numFixedRows; x++) {
            nodes[x][y]->m_fixed = true;
        }
    }
    
    //    for (int y = numYNodes/2 - 2; y < numYNodes/2 + 2; y++) {
    //        for (int x = numFixedRows; x < numFixedRows + 3; x++) {
    //            nodes[x][y]->m_fixed = true;
    //        }
    //    }
    
    
    //nodes[9][0]->m_fixed = true;
    //nodes[9][9]->m_fixed = true;
    
    
    // set default physical properties for links
    cGELSkeletonLink::s_default_kSpringElongation = 15.0;  // [N/m]
    cGELSkeletonLink::s_default_kSpringFlexion    = 0.5;   // [Nm/RAD] //0.5
    cGELSkeletonLink::s_default_kSpringTorsion    = 0.4;   // [Nm/RAD]
    cGELSkeletonLink::s_default_color.setBlueCornflower();
    
    // create links between nodes
    for (int y=0; y<numYNodes-1; y++)
    {
        for (int x=0; x<numXNodes-1; x++)
        {
            cGELSkeletonLink* newLinkX0 = new cGELSkeletonLink(nodes[x+0][y+0], nodes[x+1][y+0]);
            cGELSkeletonLink* newLinkX1 = new cGELSkeletonLink(nodes[x+0][y+1], nodes[x+1][y+1]);
            cGELSkeletonLink* newLinkY0 = new cGELSkeletonLink(nodes[x+0][y+0], nodes[x+0][y+1]);
            cGELSkeletonLink* newLinkY1 = new cGELSkeletonLink(nodes[x+1][y+0], nodes[x+1][y+1]);
            defObject->m_links.push_front(newLinkX0);
            defObject->m_links.push_front(newLinkX1);
            defObject->m_links.push_front(newLinkY0);
            defObject->m_links.push_front(newLinkY1);
        }
    }
    
    // connect skin (mesh) to skeleton (GEM)
    defObject->connectVerticesToSkeleton(false);
    
    // show/hide underlying dynamic skeleton model
    defObject->m_showSkeletonModel = false;
    
    
    cylinder = new cShapeCylinder(0.1, 0.1, 1.4);
    cylinder->rotateAboutGlobalAxisDeg(cVector3d(0,1,0), 90);
    world->addChild(cylinder);
    
    box = new cShapeBox(0.15, 1.05, 0.15);
    world->addChild(box);
    
}

void WideBrushController::updateGraphics() {
    defWorld->updateSkins(true);
}

cVector3d WideBrushController::getNormalAtPosition(cVector3d pos) {
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
    
    return g;
}


void WideBrushController::updateHaptics(double time, cVector3d pos, double deviceForceScale) {
    // clear all external forces
    defWorld->clearExternalForces();
    
    moveNodesToCursor(pos, true);
    
    
    cVector3d force(0.0, 0.0, 0.0);
    for (int y=0; y<numYNodes; y++)
    {
        for (int x=0; x<numXNodes; x++)
        {
            cVector3d nodePos = nodes[x][y]->m_pos;
        
            cVector3d f = computeForceOnPalette(nodePos, radius, stiffness);
            //Got rid of the -1 multiplication here (used to be cVector3d tmpfrc = -1 * f)
            cVector3d tmpfrc = f;
            
            force += tmpfrc * ((double) 3 * x * 1.0 / pow(numXNodes, 1));
            
            //if (x == numXNodes/2+1 && f.length() > 0) {
            
            if (f.length() > 0) {
                
                if (y != 0) {
                    
                    
                    drawBetweenPoints(nodePos, nodes[x][y - 1]->m_pos, f.length(), time);
                    
                    
                    //drawAtPoint(nodePos, f.length(), time, (y == 0), (y == numYNodes - 1));
                } else {
                    //drawBetweenPoints(nodePos, nodes[x][y - 1]->m_pos, f.length(), time, (y == 0), (y == numYNodes - 1));
                }

            }
            nodes[x][y]->setExternalForce(tmpfrc);
        }
    }
    
    // update texture
    canvas->m_texture->markForUpdate();
    
    //Integrate dynamics
    defWorld->updateDynamics(time);
    
    //Send force back to haptic device
    force.mul(deviceForceScale);
    
    hapticDevice->setForce(force);
}
