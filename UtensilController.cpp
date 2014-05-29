//
//  UtensilController.cpp
//  GEL
//
//  Created by Will Harvey on 5/22/14.
//
//

#include "UtensilController.h"

UtensilController::UtensilController(cWorld *newWorld, cMesh *newCanvas, string resourceRoot, shared_ptr<cGenericHapticDevice> newHapticDevice) {
    world = newWorld;
    canvas = newCanvas;
    hapticDevice = newHapticDevice;
    paintColor.setBlueLightSky();
}

void UtensilController::setCanvasSize(double newSize) {
    canvasSize = newSize;
}

void UtensilController::drawAtPoint(const cVector3d texCoord, double force, double timeInterval, bool left, bool right) {
    if (canvasSize == 0) return;
    
   // return;
    
    // retrieve pixel information
    int px, py;
    
    cVector3d newCoord;
    
    newCoord.x((texCoord.y() - canvas->getLocalPos().y() + canvasSize/2)/canvasSize);
    newCoord.y((texCoord.z() - canvas->getLocalPos().z() + canvasSize/2)/canvasSize);
    

    newCoord.z(0);
    canvas->m_texture->m_image->getPixelLocation(newCoord, px, py);
    
    if (px == 0 || py == 0) {
        cout << "px " << px << "py " << py << endl;
        cout << "break " << endl;
    }
    
    
    double K_INK = 10;
    double K_SIZE = 5;//30;
    int BRUSH_SIZE_Y = 25;//30;
    int BRUSH_SIZE_X = BRUSH_SIZE_Y;
    double size = cClamp((K_SIZE * force), 0.0, (double)(BRUSH_SIZE_Y));
    
    if (!left && !right) {
        //Middle
        BRUSH_SIZE_X = 100;
    }
    
    
    //Set the new pixels
    for (int x=-BRUSH_SIZE_X; x<BRUSH_SIZE_X; x++)
    {
        for (int y=-BRUSH_SIZE_Y; y<BRUSH_SIZE_Y; y++)
        {
            // compute new color percentage
            double distance = sqrt(x*x + y*y);
            
            
            if (distance <= size)
            {
                // get color at location
                cColorb color, newColor;
                canvas->m_texture->m_image->getPixelColor(px+x, py+y, color);
                
                // compute color factor based of pixel position and force interaction
                double factor = cClamp(K_INK * timeInterval * cClamp(force, 0.0, 10.0) * cClamp(1 - distance/size, 0.0, 1.0), 0.0, 1.0);
                
                // compute new color
                newColor.setR((1.0 - factor) * color.getR() + factor * paintColor.getR());
                newColor.setG((1.0 - factor) * color.getG() + factor * paintColor.getG());
                newColor.setB((1.0 - factor) * color.getB() + factor * paintColor.getB());
               // cout << "drawing at " << px + x << ", " <<py + y << endl;
                
                // assign new color to pixel
                canvas->m_texture->m_image->setPixelColor(px+x, py+y, newColor);
            }
        }
    }
}

void UtensilController::drawBetweenPoints(const cVector3d texCoord, const cVector3d texCoord2, double force, double timeInterval) {
        
    if (canvasSize == 0) return;
        
    // retrieve pixel information
    int px, py;
    cVector3d newCoord;
    
    newCoord.x((texCoord.y() - canvas->getLocalPos().y() + canvasSize/2)/canvasSize);
    newCoord.y((texCoord.z() - canvas->getLocalPos().z() + canvasSize/2)/canvasSize);
    
    newCoord.z(0);
    canvas->m_texture->m_image->getPixelLocation(newCoord, px, py);
    
    int px2, py2;
    cVector3d newCoord2;
    
    newCoord2.x((texCoord2.y() - canvas->getLocalPos().y() + canvasSize/2)/canvasSize);
    newCoord2.y((texCoord2.z() - canvas->getLocalPos().z() + canvasSize/2)/canvasSize);
    
    newCoord2.z(0);
    canvas->m_texture->m_image->getPixelLocation(newCoord2, px2, py2);
    
    
    double K_INK = 10;
    double K_SIZE = 30;
    int BRUSH_SIZE = 30;
    double size = cClamp((K_SIZE * force), 0.0, (double)(BRUSH_SIZE));
    
//    if (!left && !right) {
//        //Middle
//        BRUSH_SIZE_X = 100;
//    }
//    
    
    //Set the new pixels
    for (int x=px; x<px2; x++)
    {
        for (int y=-BRUSH_SIZE; y<BRUSH_SIZE; y++)
        {
            // compute new color percentage
            double distance = sqrt(y*y);
            
            
            
            
            if (distance <= size)
            {
                // get color at location
                cColorb color, newColor;
                canvas->m_texture->m_image->getPixelColor(x, py+y, color);
                
                // compute color factor based of pixel position and force interaction
                double factor = cClamp(K_INK * timeInterval * cClamp(force, 0.0, 10.0) * cClamp(1 - distance/size, 0.0, 1.0), 0.0, 1.0);
        
                // compute new color
                newColor.setR((1.0 - factor) * color.getR() + factor * paintColor.getR());
                newColor.setG((1.0 - factor) * color.getG() + factor * paintColor.getG());
                newColor.setB((1.0 - factor) * color.getB() + factor * paintColor.getB());
        
        
                // assign new color to pixel
                //     cout << "new color " << newColor.getR() << " " << newColor.getG() << " " << newColor.getB() << endl;
                canvas->m_texture->m_image->setPixelColor(x, py+y, newColor);
            }
        }
    }
}



void UtensilController::updateHaptics(double time, cVector3d pos, double deviceForceScale) {
    
}

void UtensilController::updateGraphics() {
    
}

void UtensilController::removeFromWorld() {
    
}

void UtensilController::switchPaintCol(cColorb newColor) {
    paintColor = newColor;
}
