#include "particle.hpp"

particle::particle() {
    appearForceX = ofRandom(0.00001, 0.00003);
    appearForceY = ofRandom(0.00001, 0.00003);
    appearForceZ = ofRandom(0.00001, 0.00003);
}

void particle::resetForce(){
    frc.set(0, 0, 0);
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
    opacity = opacity - 2.5;
}

void particle::appearAllWords(){
    if(opacity <= 255) {
        opacity = opacity + ofRandom(0.5,1.5);
    }
}

void particle::draw(ofxFontStash * particleFont){
    ofPushMatrix();
    ofSetColor(fontColor, opacity);
    ofTranslate(pos.x, pos.y, pos.z);
    ofRotate(angle, 0.015, 0.03, 0.05);
    particleFont->drawBatch(finalWord, particleFontSize, 0, 0);
    ofPopMatrix();
}

void particle::drawCircle(){
    ofPushMatrix();
    ofSetColor(fontColor, opacity);
    ofTranslate(pos.x, pos.y, pos.z);
    ofRotate(angle, 1.5, 3, 2);
//    ofSetCircleResolution(20);
    ofDrawCircle(0, 0, 1);
    ofPopMatrix();
}

