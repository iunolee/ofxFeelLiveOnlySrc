#pragma once

#include "ofMain.h"
#include "Boid.hpp"


class attractor {

	public:
    void setup(){
      location.set(ofGetWidth()/2, ofGetHeight()/2);
      mass = 20;
      G = 1;
      dragVector.set(0, 0);
    }

    ofVec2f attract(Boid & m){
      // Calculate direction of force
      ofVec2f force = location - m.getLocation();
      // Distance between objects
      float distance = force.length();
      // Limiting the distance to eliminate "extreme" results for very close or very far objects
      distance = ofClamp(distance,5.0,25.0);
      // Normalize vector (distance doesn't matter here, we just want this vector for direction)
      force.normalize();
      // Calculate gravitional force magnitude
      float strength = (G * mass * m.getMass()) / (distance * distance);
      // Get force vector --> magnitude * direction
      force *= strength;
      return force;
    }

    void draw(){
      if (isDragging){
        ofSetColor(50);
      } else if (isRollover){
        ofSetColor(100);
      } else {
        ofSetColor(175);
      }
      ofDrawEllipse(location, 10, 10);
    }

    void setMass(float m){
      mass = m;
    }

    void setLocation( float x, float y){
      location.set(x, y);
    }

    // The methods below are for mouse interaction
    void clicked(int mx, int my) {
      ofVec2f mouse(mx, my);
      float d = (mouse - location).length();
      if (d < mass) {
        isDragging = true;
        dragVector = location - mouse;
      }
    }

    void rollover(int mx, int my) {
      ofVec2f mouse(mx, my);
      float d = (mouse - location).length();
      if (d < mass) {
        isRollover = true;
      } 
      else {
        isRollover = false;
      }
    }

    void stopDragging() {
      isDragging = false;
    }

    void drag(int mx, int my) {
      ofVec2f mouse(mx, my);
      if (isDragging) {
        location = mouse + dragVector;
      }
    }

		
  public:
    ofVec2f location;
    float mass;
    float G;
    bool isDragging;
    bool isRollover;
    ofVec2f dragVector;
};
