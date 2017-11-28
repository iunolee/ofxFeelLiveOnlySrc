#pragma once

#include "ofMain.h"
#include "ofxRipples.h"

class Boid {
    
public:
    void setup(){
        velocity.set(ofRandom(-2, 2), ofRandom(-2, 2));
        acceleration.set(0, 0);
        r = 2;
        maxSpeed = 5;
        maxForce = 0.01;
        mass = 3;
    }
    
    // Method to update location
    void update(){
        // Update velocity
        velocity += acceleration;
        // Limit speed
        velocity.limit(maxSpeed);
        location += velocity;
        // Reset acceleration to 0 each cycle
        acceleration *= 0;
    }
    
    void applyForce(const ofVec2f & force) {
        // We could add mass here if we want A = F / M
        ofVec2f f(force/mass);
        acceleration += f;
    }
    
    //    void applyForce(const ofVec2f & force){
    //        ofVec2f f(force/mass);
    //        acceleration += f;
    //    }
    //
    // We accumulate a new acceleration each time based on three rules
    void flock(vector<Boid> boids) {
        ofVec2f sep = separate(boids);   // Separation
        ofVec2f ali = align(boids);      // Alignment
        ofVec2f coh = cohesion(boids);   // Cohesion
        // Arbitrarily weight these forces
        sep *= 1.0;//1.5
        ali *= 1.0;//1.0
        coh *= 1.0;//1.0
        // Add the force vectors to acceleration
        applyForce(sep);
        applyForce(ali);
        applyForce(coh);
    }
    
    // Separation
    // Method checks for nearby boids and steers away
    ofVec2f separate (vector<Boid> boids) {
        float desiredseparation = 50.0f;
        ofVec2f sum;
        int count = 0;
        
        // For every boid in the system, check if it's too close
        
        // one step further in stl C++ programming
        // we're using iterators:
        vector<Boid>::iterator other;
        for (other = boids.begin(); other < boids.end(); other++) {
            float d = (location - other->getLocation()).length();
            // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
            if ((d > 0) && (d < desiredseparation)) {
                // Calculate vector pointing away from neighbor
                ofVec2f diff = location - other->getLocation();
                diff.normalize();
                diff /= d;        // Weight by distance
                sum += diff;
                count++;            // Keep track of how many
            }
        }
        // Average -- divide by how many
        if (count > 0) {
            sum /= count;
            // Our desired vector is the average scaled to maximum speed
            // As long as the vector is greater than 0
            if (sum.length() > 0){
                sum.normalize();
                sum *= maxSpeed;
                // Implement Reynolds: Steering = Desired - Velocity
                ofVec2f steer = sum - velocity;
                steer.limit(maxForce);
                
                return steer;
            }
        }
        return ofVec2f(0, 0);
    }
    
    // Alignment
    // For every nearby boid in the system, calculate the average velocity
    ofVec2f align (vector<Boid> boids) {
        float neighbordist = 50;
        ofVec2f sum = ofVec2f(0,0);
        int count = 0;
        vector<Boid>::iterator other;
        for (other = boids.begin(); other < boids.end(); other++) {
            float d = (location - other->getLocation()).length();
            if ((d > 0) && (d < neighbordist)) {
                sum += other->getVelocity();
                count++;
            }
        }
        if (count > 0) {
            sum /= (float)count;
            sum.normalize();
            sum *= maxSpeed;
            ofVec2f steer = sum - velocity;
            steer.limit(maxForce);
            return steer;
        } else {
            return ofVec2f(0,0);
        }
    }
    
    // Cohesion
    // For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
    ofVec2f cohesion (vector<Boid> boids) {
        float neighbordist = 50;
        ofVec2f sum = ofVec2f(0,0);   // Start with empty vector to accumulate all locations
        int count = 0;
        vector<Boid>::iterator other;
        for (other = boids.begin(); other < boids.end(); other++) {
            float d = (location - other->getLocation()).length();
            if ((d > 0) && (d < neighbordist)) {
                sum += other->getLocation(); // Add location
                count++;
            }
        }
        if (count > 0) {
            sum /= count;
            return seek(sum);  // Steer towards the location
        }
        return ofVec2f(0,0);
    }
    
    // Wraparound
    void borders() {
        if (location.x < -r) location.x = ofGetWindowSize()[0]+r;
        if (location.y < -r) location.y = ofGetWindowHeight()+r;
        if (location.x > ofGetWindowSize()[0]+r) location.x = -r;
        if (location.y > ofGetWindowHeight()+r) location.y = -r;
    }
    
    void boundaries() {
        float d = 25;
        
        ofVec2f desired;
        
        if (location.x < d) {
            desired = ofVec2f(maxSpeed, velocity.y);
        }
        else if (location.x > ofGetWindowSize()[0] -d) {
            desired = ofVec2f(-maxSpeed, velocity.y);
        }
        
        if (location.y < d) {
            desired = ofVec2f(velocity.x, maxSpeed);
        }
        else if (location.y > ofGetWindowHeight()-d) {
            desired = ofVec2f(velocity.x, -maxSpeed);
        }
        
        //      if (desired != 0) {
        desired.normalize();
        desired *= maxSpeed;
        ofVec2f steer = desired - velocity;
        steer.limit(maxForce);
        applyForce(steer);
        //      }
    }
    
    // A method that calculates a steering force towards a target
    // STEER = DESIRED MINUS VELOCITY
    void arrive(const ofVec2f & target) {
        ofVec2f desired = target - location;  // A vector pointing from the location to the target
        float d = desired.length();
        // Normalize desired and scale with arbitrary damping within 100 pixels
        desired.normalize();
        if (d < 100) {
            float m = ofMap(d, 0, 100, 0, maxSpeed);
            desired *= m;
        } else {
            desired *= maxSpeed;
        }
        
        // Steering = Desired minus velocity
        ofVec2f steer = desired - velocity;
        steer.limit(maxForce);  // Limit to maximum steering force
        
        applyForce(steer);
    }
    
    // A method that calculates a steering force towards a target
    // STEER = DESIRED MINUS VELOCITY
    ofVec2f seek(const ofVec2f & target) {
        ofVec2f desired = target - location;  // A vector pointing from the location to the target
        
        // Normalize desired and scale to maximum speed
        desired.normalize();
        desired *= maxSpeed;
        // Steering = Desired minus velocity
        ofVec2f steer = desired - velocity;
        steer.limit(maxForce);  // Limit to maximum steering force
        
        return steer;
    }
    
    void draw(float r_){
        // Draw a triangle rotated in the direction of velocity
        float theta = ofVec2f(1, 0).angle(velocity) + 90;
        
        ofPushMatrix();
        ofTranslate(location.x,location.y);
        ofRotate(theta);
        //      ofBeginShape();
        ofFill();
        ofSetColor(color);
        // r_ = 30 + ofRandom(10);
        ofDrawEllipse(0,0, r_, r_);
        ofNoFill();
        //        ofSetColor(color);
        //        ofDrawEllipse(0,0, r+10, r+10);
        
        //      ofVertex(0, -r*2);
        //      ofVertex(-r, r*2);
        //      ofVertex(r, r*2);
        //      ofEndShape(true);
        // stroke
        //      ofBeginShape();
        //      ofNoFill();
        //      ofSetColor(0);
        //      ofVertex(0, -r*2);
        //      ofVertex(-r, r*2);
        //      ofVertex(r, r*2);
        //      ofEndShape(true);
        
        ofPopMatrix();
    }
    
    void setLocation( float x, float y){
        location.set(x, y);
    }
    
    void setVelocity( float x, float y){
        velocity.set(x, y);
    }
    
    void setMaxForce( float f){maxForce = f;}
    void setMaxSpeed( float s){maxSpeed = s;}
    
    const ofVec2f& getLocation() const{
        return location;
    }
    
    const ofVec2f& getVelocity() const{
        return velocity;
    }
    
    float getMass() const{
        return mass;
    }
    
    void setMass(float m){
        mass = m;
    }
    
public:
    ofVec2f location;
    ofVec2f velocity;
    ofVec2f acceleration;
    float r;
    float maxForce;
    float maxSpeed;
    
    public :
    ofColor color;
    float mass;
    
};

