#include "particle.hpp"

particle::particle() {
    appearForceX = ofRandom(-0.000015, 0.000015);
    appearForceY = ofRandom(-0.000015, 0.000015);
    appearForceZ = ofRandom(-0.000015, 0.000015);
}

void particle::resetForce(){
    frc.set(0, 0, 0);
}

void particle::addSentenceForce(){
    frc.x = frc.x + sentenceForceX;
    frc.y = frc.y + sentenceForceY;
    frc.z = frc.z + sentenceForceZ;
}

void particle::addAppearForce(){
    frc.x = frc.x + appearForceX;
    frc.y = frc.y + appearForceY;
    frc.z = frc.z + appearForceZ;
}

void particle::addScatteredForce(){
    frc.x = frc.x + scatterdForceX;
    frc.y = frc.y + scatterdForceY;
    frc.z = frc.z + scatterdForceZ;
    angle = angle + angleSpeed;
}

void particle::addDampingForce(){
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
    frc.z = frc.z - vel.z * damping;
    angle = angle - angleSpeed * damping;
}

void particle::setInitialCondition(float px, float py, float pz, float vx, float vy, float vz){
    pos.set(px, py, pz);
    vel.set(vx, vy, vz);
}

void particle::update(){
    vel = vel + frc;
    pos = pos + vel;
}

void particle::disappearOtherWords(){
    opacity = opacity - 3.5;
}

void particle::appearAllWords(){
    if(opacity < 255) {
        opacity = opacity + opacitySpeed;
    }
}

void particle::appearAllCircles(){
    if(opacity < 200) {
        opacity = opacity + opacitySpeed;
    }
}

void particle::draw(ofxFontStash * particleFont){
    ofPushMatrix();
    ofSetColor(fontColor, opacity);
    ofTranslate(pos.x, pos.y, pos.z);
    ofRotate(angle, 0.05, 0.05, 0.5);
    particleFont->drawBatch(finalWord, particleFontSize, 0, 0);
    ofPopMatrix();
}

void particle::drawCircle(){
    ofPushMatrix();
    ofSetColor(fontColor, opacity);
    ofTranslate(pos.x, pos.y, pos.z);
//    ofSetCircleResolution(20);
    ofDrawCircle(0, 0, 0.75);
    ofPopMatrix();
}

