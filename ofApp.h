#pragma once

#include "ofMain.h"
#include "particle.hpp"
#include "ofEvents.h"
#include "ofxSocketIO.h"
#include "ofxRipples.h"
#include "ofxGUI.h"
#include "ofxFX.h"
#include "ofxXmlSettings.h"
#include "ofxJSON.h"
#include "ofxFontStash.h"
#include "Boid.hpp"



class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    // TWEET TEXT PARTICLE
    void initSetting();
    void initTweetDataProcessing();
    void initTextParticle();
    void eraseAllWordParticle();
    
    // SOCKET PART
    bool isConnected;
    void onConnection();
    void bindEvents();
    void gotEvent(std::string& name);
    void onServerEvent(ofxSocketIOData& data);
    
    // SOUND PART
    void soundSetup();
    void soundEffectStop();
    void audioOut(float* buffer, int bufferSize, int nChannels);
    
    ofxSocketIO socketIO;
    
    ofEvent<ofxSocketIOData&> serverEvent;
    ofEvent<ofxSocketIOData&> pingEvent;
    ofEvent<ofxSocketIOData&> nspingEvent;
    
    std::string address;
    std::string status;
    
    
    // FOR TWEET TEXT PARTICLE
    int fontSize;
    ofxFontStash font;
    
    ofColor *colors;
    
    float forceValue;
    
    vector<particle> particles;
    vector<particle> letterParticles;
    vector<particle> circleParticles;
    
    vector<string> emotionKeywords;
    vector<string> joy;
    vector<string> trust;
    vector<string> fear;
    vector<string> surprise;
    vector<string> sadness;
    vector<string> disgust;
    vector<string> anger;
    vector<string> anticipation;
    
    vector<int> selectionPool;
    ofxJSONElement tweetData;
    vector<string> tweetText;
    string tweetTextFinal;
    
    int count;
    int gotDataTotalNum;
    //    ofColor selectedColor;
    //    int selectedEmotion;
    
    // GRAPHIC SIDE @YUN
    ofAppGLFWWindow * WINDOW;
    int screenHeight;
    ofxIntSlider screenWidth;
    
    // COLOR
    int colorNum = 8;
    float alpha = 100;
    ofBlendMode blendMode;
    
    // RIPPLE
    ofxRipples rip;
    int rippleFlag = 0;
    
    // BLOB ARRAYS
    vector <int> xpos;
    vector <int> ypos;
    
    // BLOB
    vector<Boid> blobs1;
    vector<Boid> blobs2;
    vector<Boid> blobs3;
    vector<Boid> blobs4;
    vector<Boid> blobs5;
    vector<Boid> blobs6;
    vector<Boid> blobs7;
    vector<Boid> blobs8;
    
    // BOOLEAN FOR SCENE CHANGE
//    bool bInfoText;
    bool bHide;
    bool bRipple;
//    bool border;
    bool otherWordsDisapper;
    bool scattered;
    bool showTweetRandomly;
    
    
    // GUI
    ofxFloatSlider rippleSize;
    ofxFloatSlider rippleDistance;
    ofxFloatSlider rippleDamping;
    ofxIntSlider urlYpos;
    ofxIntSlider urlXpos1;
    ofxIntSlider urlXpos2;
    ofxIntSlider urlXpos3;
    ofxIntSlider urlSize;
    ofxIntSlider instructionSize;
    ofxIntSlider effectAStart;
    ofxIntSlider effectATime;
    ofxIntSlider effectBStart;
    ofxIntSlider effectBTime;
    ofxIntSlider effectCStart;
    ofxIntSlider effectCTime;
    ofxToggle line;
    ofxPanel gui;
    
    //TIMER
    float startTime;
    float timer;
    
    // VIDEO PLAYER
    //    ofVideoGrabber vidGrabber;
    //    int camWidth;
    //    int camHeight;
    
    ofAVFoundationPlayer video;
    bool bVideo;
//    bool isPlayed;
    
    // SOUND PLAYER
    ofSoundPlayer  sound1;
    vector <ofSoundPlayer> sclouds;
    ofDirectory dir;
    
    float myArray[100];
    float frequency;
    float phase;
    float amplitude;
    float phaseInc;
    
    // INTRO URL
    ofTrueTypeFont url;
    ofTrueTypeFont instruction;
    bool urlShow;
    int urlAlpha = 100;
    int guiPosition = -500;
};

