#ifndef particle_hpp
#define particle_hpp

#include "ofMain.h"
#include "ofxFontStash.h"

class particle{
    
public:
    ofVec3f pos;
    ofVec3f vel;
    ofVec3f frc;
    
    particle();
    
    void resetForce();
    void addScatteredForce();
    void addAppearForce();
    void addDampingForce();
    void disappearOtherWords();
    void appearAllWords();
    void setInitialCondition(float px, float py, float pz, float vx, float vy, float vz);
    void update();
    void draw(ofxFontStash * particleFont);
    void drawCircle();
    float damping;
    int particleFontSize;
    string finalWord;
    ofColor fontColor;
    float angle;
    float angleSpeed;
    float opacity;
    float scatterdForceX, scatterdForceY, scatterdForceZ;
    float appearForceX, appearForceY, appearForceZ;
    float raindropForceX, raindropForceY, raindropForceZ;
};

#endif /* particle_hpp */


