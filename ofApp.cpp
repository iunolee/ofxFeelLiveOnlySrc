#include "ofApp.h"
#include "ofxRipples.h"
#include "Boid.hpp"
#include "attractor.h"


vector <vector<Boid>> boids;
vector<attractor> attractors;
//--------------------------------------------------------------

void ofApp::setup(){
    
    // GRAPHIC SETTING
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    //    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    // BOOLEAN SETTING
//    bInfoText = true;
    bRipple = false;
    bVideo = false;
    bHide = true;
//    isPlayed = false;
    urlShow = true;
    showTweetRandomly = false;
    otherWordsDisapper = false;
    scattered = false;
    
    // GUI
    gui.setup();
    gui.setPosition(ofGetWindowSize()[0]/2, 20);
    gui.add(rippleSize.setup("size", 100, 1, 150));
    gui.add(rippleDistance.setup("distance", 2, 1, 10));
    gui.add(rippleDamping.setup("damping", 0.995, 0.900, 0.999));
    gui.add(urlYpos.setup("urlYpos", ofGetWindowHeight()/2, 0, ofGetWindowHeight()));
    gui.add(urlXpos1.setup("urlXpos1", 200, 0, ofGetWindowSize()[0]));
    gui.add(urlXpos2.setup("urlXpos2", ofGetWindowSize()[0]/2, 0, ofGetWindowSize()[0]));
    gui.add(urlXpos3.setup("urlXpos3", ofGetWindowSize()[0]*2/3, 0, ofGetWindowSize()[0]));
    gui.add(urlSize.setup("urlSize", 150, 10, 300));
    gui.add(instructionSize.setup("instructionSize", 30, 10, 300));
    gui.add(effectAStart.setup("effectAStart", 10000, 0, 100000));
    gui.add(effectATime.setup ("effectATime", 1000,  0, 10000));    // maximum 10 second
    gui.add(effectBStart.setup("effectBStart", 30000, 0, 100000));
    gui.add(effectBTime.setup ("effectBTime", 3000,  0, 10000));
    gui.add(effectCStart.setup("effectCStart", 50000, 0, 100000));
    gui.add(effectCTime.setup ("effectCTime", 5000,  0, 10000));
    gui.add(line.setup("line", false));
    gui.loadFromFile("settings.xml");
    ofHideCursor();
    
    // SET STARTING POINT OF RIPPLE
    rip.allocate(ofGetWindowSize()[0], ofGetWindowHeight());
    
    // SOCKET SETUP
    isConnected = false;
    address = "http://67.205.153.66:9999";
    status = "not connected";
    socketIO.setup(address);
    ofAddListener(socketIO.notifyEvent, this, &ofApp::gotEvent);
    ofAddListener(socketIO.connectionEvent, this, &ofApp::onConnection);
    
    // INIT BASIT SETTING FOR EMOTION COLORS AND KEYWORD
    initSetting();
    
    // SET TWEET TEXT SIZE & RAINDROP SPEED
    fontSize = 20;
    forceValue = 0.005;
    
    // VIDEO
    video.setPixelFormat(OF_PIXELS_RGBA);
    //setPixelFormat() has to be called before loading a movie...
    //vid_calm.setDesiredFrameRate(30);
    video.setLoopState(OF_LOOP_NORMAL);
    video.load("1117_Orbits_LinuxFont_PJ_Q90.mov");

    
    // URL AND INSTRUCTION
    url.load("Roboto-Thin.ttf",urlSize, true, false, true);
    instruction.load("Roboto-Thin.ttf",instructionSize, true, false, true);

    // SOUND SETUP
    soundSetup();

    
    // ATTRACTOR SETUP
    for (int i = 0; i < 8 ; i++) {
        attractor myAttractors;
        attractors.push_back(myAttractors);
        attractors[i].setup();
        attractors[i].setLocation((ofGetWindowSize()[0]*0.05)+(i * ofGetWindowSize()[0]*0.128), ofGetWindowHeight()/2);
    }
    
    // BLOB ARRAY SETUP
    boids.push_back(blobs1);
    boids.push_back(blobs2);
    boids.push_back(blobs3);
    boids.push_back(blobs4);
    boids.push_back(blobs5);
    boids.push_back(blobs6);
    boids.push_back(blobs7);
    boids.push_back(blobs8);
}

//--------------------------------------------------------------

void ofApp::initSetting () {
    //SET COLOR LIST
    colors = new ofColor[colorNum+1];
    
    //ALPHA VALUE
    for(int i = 0; i< colorNum+1; i++){
        colors[i].a = alpha;
    }
    
    //initial value
    colors[0].r = 255;
    colors[0].g = 255;
    colors[0].b = 255;
    
    //joy
    colors[1].r = 248;
    colors[1].g = 198;
    colors[1].b = 51;
    
    //trust
    colors[2].r = 178;
    colors[2].g = 187;
    colors[2].b = 58;
    
    //fear
    colors[3].r = 10;
    colors[3].g = 108;
    colors[3].b = 67;
    
    //surprise
    colors[4].r = 6;
    colors[4].g = 128;
    colors[4].b = 157;
    
    //sadness
    colors[5].r = 118;
    colors[5].g = 59;
    colors[5].b = 123;
    
    //disgust
    colors[6].r = 145;
    colors[6].g = 30;
    colors[6].b = 71;
    
    //anger
    colors[7].r = 186;
    colors[7].g = 39;
    colors[7].b = 58;
    
    //anticipation
    colors[8].r = 216;
    colors[8].g = 97;
    colors[8].b = 43;
    
    
    //EMOTION LIST
    emotionKeywords = {"confused", "rejected", "helpless", "submissive", "insecure", "anxious", "hurt", "hostile", "angry", "selfish", "hateful", "critical", "tired", "bored", "lonely", "depressed", "ashamed", "guilty", "gloomy", "excited", "sensuous", "energetic", "cheerful", "creative", "aware", "proud", "respected", "appreciated", "important", "faithful", "nurturing", "trusting", "loving", "intimate", "thoughtful", "content", "curious", "interest", "expect", "amaze", "astonish", "distract", "bored", "dislike", "loath"};
    
    joy = {"excited", "sensuous", "energetic", "cheerful", "creative"};
    trust = {"aware", "proud", "respected", "appreciated", "important", "faithful", "nurturing", "trusting", "loving", "intimate", "thoughtful", "content"};
    fear = {"confused", "rejected", "helpless", "submissive", "insecure", "anxious"};
    surprise = {"amaze", "astonish", "distract"};
    sadness = {"tired", "bored", "lonely", "depressed", "ashamed", "guilty", "gloomy"};
    disgust = {"bored", "dislike", "loath"};
    anger = {"hurt", "hostile", "angry", "selfish", "hateful", "critical"};
    anticipation = {"curious", "interest", "expect"};
    
    
    //SET COUNT TO VISUALIZE EACH WORD ONE BY ONE
    count = 0;
}

//--------------------------------------------------------------

void ofApp::initTweetDataProcessing() {
    //PROCESSING TWEET DATA
    bool parsingSuccessful = tweetData.open("tweetSearchData1122_formatted.json");
    
    if (parsingSuccessful)
    {
        //GET TWEET ACCORDING TO AUDIENCE SELECTIONS OF EMOTION
        for (int i=0; i < selectionPool.size(); i++) {
            string joySelection = joy[ofRandom(joy.size())];
            string trustSelection = trust[ofRandom(trust.size())];
            string fearSelection = fear[ofRandom(fear.size())];
            string surpriseSelection = surprise[ofRandom(surprise.size())];
            string sadnessSelection = sadness[ofRandom(sadness.size())];
            string disgustSelection = disgust[ofRandom(disgust.size())];
            string angerSelection = anger[ofRandom(anger.size())];
            string anticipationSelection = anticipation[ofRandom(anticipation.size())];
            
            int randomNum = int(ofRandom(10));
            
            if (selectionPool[i] == 1) {
                string temp = tweetData[joySelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 2) {
                string temp = tweetData[trustSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 3) {
                string temp = tweetData[fearSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 4) {
                string temp = tweetData[surpriseSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 5) {
                string temp = tweetData[sadnessSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 6) {
                string temp = tweetData[disgustSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 7) {
                string temp = tweetData[angerSelection][randomNum].asString();
                tweetText.push_back(temp);
            } else if (selectionPool[i] == 8) {
                string temp = tweetData[anticipationSelection][randomNum].asString();
                tweetText.push_back(temp);
            }
        }
        
        //MAKE SET OF TWEET TEXT INTO ONE STRING
        tweetTextFinal = accumulate(begin(tweetText), end(tweetText), tweetTextFinal);
        cout<<"tweet data processing done"<<endl;
    }
    else
    {
        ofLogNotice("ofApp::setup")  << "Failed to parse JSON" << endl;
    }
}

//--------------------------------------------------------------

void ofApp::initTextParticle(){
    /*INIT TEXT PARTICLE*/
    
    //SPLIT TWEET TEXT INTO EVERY WORDS
    vector<string> letters;
    letters = ofSplitString(tweetTextFinal, " ");
    
    //LOAD FONT
    font.setup("LinLibertine_R.ttf", 1.0, 1024, false, 0, 1);
    
    //MAKE WORD PARTICLE WITH ASSIGNING ATTRIBUTES
    ofRectangle firstbbox = font.getBBox(letters[0], fontSize, 0, 0);
    float fontHeight = firstbbox.height;
    
    float xPosition = -firstbbox.width;
    float yPosition = fontHeight;
    
    ofRectangle spacebbox = font.getBBox("s", fontSize, 0, 0);
    float space = spacebbox.width;
    
    for (int i = 0; i < letters.size(); i++){
        ofRectangle currentbbox = font.getBBox(letters[i], fontSize, 0, 0);
        float currentFontWidth = currentbbox.width;
        particle myParticle;
        myParticle.setInitialCondition(xPosition,yPosition,0, 0, 0, 0);
        myParticle.particleFontSize = fontSize;
        myParticle.angle = 0;
        myParticle.finalWord = letters[i];
        myParticle.opacity = 0;
        
        ofColor assignedColor;
        if(find(joy.begin(), joy.end(), letters[i]) != joy.end()) {
            myParticle.fontColor = colors[1];
            assignedColor = colors[1];
        } else if(find(trust.begin(), trust.end(), letters[i]) != trust.end()) {
            myParticle.fontColor = colors[2];
            assignedColor = colors[2];
        } else if(find(fear.begin(), fear.end(), letters[i]) != fear.end()) {
            myParticle.fontColor = colors[3];
            assignedColor = colors[3];
        } else if(find(surprise.begin(), surprise.end(), letters[i]) != surprise.end()) {
            myParticle.fontColor = colors[4];
            assignedColor = colors[4];
        } else if(find(sadness.begin(), sadness.end(), letters[i]) != sadness.end()) {
            myParticle.fontColor = colors[5];
            assignedColor = colors[5];
        } else if(find(disgust.begin(), disgust.end(), letters[i])!= disgust.end()) {
            myParticle.fontColor = colors[6];
            assignedColor = colors[6];
        } else if(find(anger.begin(), anger.end(), letters[i]) != anger.end()) {
            myParticle.fontColor = colors[7];
            assignedColor = colors[7];
        } else if(find(anticipation.begin(), anticipation.end(), letters[i]) != anticipation.end()) {
            myParticle.fontColor = colors[8];
            assignedColor = colors[8];
        } else {
            myParticle.fontColor = colors[0];
            assignedColor = colors[0];
        }
        
        
        //MAKE LETTER PARTICLE FOR DROPPING LATER
        if(find(emotionKeywords.begin(), emotionKeywords.end(), letters[i]) != emotionKeywords.end()) {
            
            string currentLetter = letters[i];
            
            float xPositionLetter = xPosition;
            float yPositionLetter = yPosition;
            
            for(int j=0; j<currentLetter.size(); j++) {
                char s = currentLetter[j];
                string singleLetter = ofToString(s);
                ofRectangle letterbbox = font.getBBox(singleLetter, fontSize, 0, 0);
                float currentLetterWidth = letterbbox.width;
                //LETTER PARTICLE
                particle letterParticle;
                letterParticle.setInitialCondition(xPositionLetter,yPositionLetter,0, 0, 0, 0);
                letterParticle.particleFontSize = fontSize;
                letterParticle.angle = 0;
                letterParticle.angleSpeed = ofRandom(-1.5, 1.5);
                letterParticle.scatterdForceX = ofRandom(-forceValue/2, forceValue/2);
                letterParticle.scatterdForceY = ofRandom(0, forceValue*2);
                letterParticle.scatterdForceZ = ofRandom(-forceValue/2, forceValue*2.5);
                letterParticle.damping = ofRandom(forceValue*0.01, forceValue*0.05);
                letterParticle.finalWord = singleLetter;
                letterParticle.opacity = 255;
                letterParticle.fontColor = assignedColor;
                letterParticles.push_back(letterParticle);
                
           //CIRCLE PARTICLE
                for(int c = 0; c < 30; c++) {
                particle circleParticle;
                circleParticle.setInitialCondition(xPositionLetter,yPositionLetter,0, 0, 0, 0);
                circleParticle.scatterdForceX = ofRandom(-forceValue/2, forceValue/2);
                circleParticle.scatterdForceY = ofRandom(0, forceValue*2);
                circleParticle.scatterdForceZ = ofRandom(-forceValue/2, forceValue*2.5);
                circleParticle.damping = ofRandom(forceValue*0.01, forceValue*0.05);
                circleParticle.fontColor = assignedColor;
                circleParticle.opacity = 5;
                circleParticles.push_back(circleParticle);
                }
                
                //POSITION EACH LETTER
                xPositionLetter = xPositionLetter + currentLetterWidth;
            }
        }
        
        //POSITION EACH WORD
        if(xPosition > ofGetWindowSize()[0]) {
            yPosition = yPosition + fontHeight*1.09;
            xPosition = 0;
        } else {
            xPosition = xPosition + currentFontWidth + space;
        }
        particles.push_back(myParticle);
    }
}

//--------------------------------------------------------------

void ofApp::eraseAllWordParticle(){
    //ERASE ALL WORD PARTICLES
    particles.clear();
}

//--------------------------------------------------------------

void ofApp::update(){
    int ranSize = ofRandom(-10.0, 10.0);
    
    if(bRipple){
        rip.begin();
        ofFill();
        
        for (unsigned int i = 0; i < boids.size(); i++){
            for (unsigned int j = 0; j < boids[i].size(); j++){
                
                // GO TO THE ATTRACTORS
                ofVec2f force = attractors[i].attract(boids[i][j]);
                boids[i][j].flock(boids[i]);
                
                if(bVideo){
                    boids[i][j].applyForce(force*(2+i*0.1));
                }
                
                boids[i][j].update();
                boids[i][j].borders();
            }
        }
        
        for (unsigned int i = 0; i < boids.size(); i++){
            for (unsigned int j = 0; j < boids[i].size(); j++){
                
                ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                
                boids[i][j].draw(rippleSize + ofRandom(10));
                
                ofSetColor(255);
                
                if(bVideo){
                    // MAKE ATTRACTORS VISIBLE
//                    for (int i = 0 ; i < 8; i++ ){
//                        attractors[i].draw();
//                    }
                }
                
                // SET LINE TIMING
                
                ofSetLineWidth(0.1);
                if(line){
                    ofDrawLine(boids[i][j+2].location.x, boids[i][j+2].location.y, boids[i][j+3].location.x, boids[i][j+3].location.y);
                }
            }
        }
        rip.end();
        rip.update();
    }
    
    // VIDEO PLAY
    if(bVideo){
        video.update();
        // cout << video.getPosition() << endl;
    }
    
    //MAKE NON EMOTION-RELATED WORD DISAPPEAR
    if(showTweetRandomly) {
        for (int i = 0; i < particles.size(); i++){
            particles[i].appearAllWords();
            particles[i].update();
            particles[i].resetForce();
            particles[i].addAppearForce();
        }
        
        for (int i = 0; i < letterParticles.size(); i++){
            letterParticles[i].resetForce();
            letterParticles[i].addAppearForce();
            letterParticles[i].update();
        }
        
        for (int i = 0; i < circleParticles.size(); i++){
            circleParticles[i].resetForce();
            circleParticles[i].addAppearForce();
            circleParticles[i].update();
        }
    }
       
       
    if(otherWordsDisapper) {
        for (int i = 0; i < particles.size(); i++){
            if(find(emotionKeywords.begin(), emotionKeywords.end(), particles[i].finalWord) == emotionKeywords.end()) {
                particles[i].disappearOtherWords();
                if(particles[i].opacity < -100) {
                    scattered = true;
                    eraseAllWordParticle();
                    //                    particles.erase(particles.begin()+i);
                }
            }
        }
    }
    
    //ADD FORCE TO MAKE EMOTION WORDS SCATTERED AND DROPPED
    if(scattered) {
        for (int i = 0; i < letterParticles.size(); i++){
            letterParticles[i].resetForce();
            letterParticles[i].addScatteredForce();
            letterParticles[i].addDampingForce();
            letterParticles[i].update();
        }
        
        for (int i = 0; i < circleParticles.size(); i++){
            circleParticles[i].appearAllWords();
            circleParticles[i].resetForce();
            circleParticles[i].addScatteredForce();
            circleParticles[i].addDampingForce();
            circleParticles[i].update();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    // SET TIMER
    float timer = ofGetElapsedTimeMillis() - startTime;
    
    // URL AND INSTRUCTION
    if(urlShow) {
        //INITIAL
        string urlText = "www.myfeel.live";
        float urlWidth = url.stringWidth(urlText);
        float urlHeight = url.stringHeight(urlText);
        ofSetColor(255,255,255,200);
        url.drawString(urlText, urlXpos1 - urlWidth/2, urlYpos+urlHeight/2);
        url.drawString(urlText, urlXpos2 - urlWidth/2, urlYpos+urlHeight/2);
        url.drawString(urlText, urlXpos3 - urlWidth/2, urlYpos+urlHeight/2);

        string instructionText = "Please open up your mobile and go to this link with any brower";
        ofSetColor(255,255,255,100);
        instruction.drawString(instructionText, urlXpos1 - urlWidth/2, urlYpos+ urlHeight);
        instruction.drawString(instructionText, urlXpos2 - urlWidth/2, urlYpos+ urlHeight);
        instruction.drawString(instructionText, urlXpos2 - urlWidth/2, urlYpos+ urlHeight);
    }
    
    if(bRipple){
        // URL REMAINED
        ofSetColor(255,255,255,255);
        string urlText = "www.myfeel.live";
        float urlWidth = url.stringWidth(urlText);
        float urlHeight = url.stringHeight(urlText);
        url.drawString(urlText, urlXpos1 - urlWidth/2, urlYpos+urlHeight/2);
        url.drawString(urlText, urlXpos2 - urlWidth/2, urlYpos+urlHeight/2);
        url.drawString(urlText, urlXpos3 - urlWidth/2, urlYpos+urlHeight/2);
        
        rip.draw(0,0, ofGetWindowSize()[0], ofGetWindowHeight());
        rip.distance = rippleDistance;
        rip.damping = rippleDamping;
        
        // ADJUSTING SIZE
        if(!bVideo){
            float sinOfTime               = sin( ofGetElapsedTimef()/2);
            float sinOfTimeMapped         = ofMap(sinOfTime, -1, 1, 20, 120);
            float sinOfTimeMappedDamp     = ofMap(sinOfTime, -1, 1, 0.950, 0.999);
            float sinOfTimeMappedDistance = ofMap(sinOfTime, -1, 1, 2.0, 9.0);
            
            rippleSize = sinOfTimeMapped;
            rippleDamping = sinOfTimeMappedDamp;
            rippleDistance = sinOfTimeMappedDistance; // 9  - 2
            
            
            if(timer >= effectAStart  && timer <= effectAStart + effectATime)  {
//                amplitude = 0.1;
                rippleDistance = 2+ofRandom(8);
//                frequency = ofMap(rippleDistance, 2.0, 10.0, 200, 1000);
//                phaseInc = (TWO_PI * frequency)/(float)48000;
            } else if(timer >= effectBStart  && timer <= effectBStart + effectBTime)  {
//                amplitude = 0.1;
                rippleDistance = 2+ofRandom(8);
//                frequency = ofMap(rippleDistance, 2.0, 10.0, 200, 1000);
//                phaseInc = (TWO_PI * frequency)/(float)48000;
            } else if(timer >= effectCStart  && timer <= effectCStart + effectCTime)  {
//                amplitude = 0.1;
                rippleDistance = 2+ofRandom(8);
//                frequency = ofMap(rippleDistance, 2.0, 10.0, 200, 1000);
//                phaseInc = (TWO_PI * frequency)/(float)48000;
            } else {
//                frequency = 0.001;
//                amplitude = 0.001;
            }
        }
    }
    
    if(bVideo){
        
        // URL DISAPPEAR
        urlYpos = -500;
        
        // OFF THE RIPPLE SOUND EFFECT
        soundEffectStop();
        
        // DECREASE RIPPLE SIZE
        if(rippleFlag == 0 ){
            rippleSize = 50;
            rippleFlag = 1;
        }
        
        if(rippleSize > 20){
            rippleSize = rippleSize - 0.1;
        }
        
        if(rippleSize <= 0){
            rippleSize = 0;
            for (unsigned int i = 0; i < boids.size(); i++){
                boids[i].pop_back();
            }
        }
        
        if (rippleDamping > 0.9) {
            rippleDamping = rippleDamping - 0.0001;
        }
        rippleDistance = 2.0;
        
        //DELETE ALPHA CHANNEL
        ofEnableAlphaBlending();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        video.draw(0, 0, ofGetWindowSize()[0], ofGetWindowHeight());
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ofDisableAlphaBlending();
    }
    
    if(bHide == false) {
        gui.draw();
        guiPosition = ofGetWindowHeight()/10;
        string fr = "fr: " + ofToString(ofGetFrameRate());
        string bn = "total blob: " + ofToString(boids[0].size()+boids[1].size()+boids[2].size()+boids[3].size()+boids[4].size()+boids[5].size()+boids[6].size()+boids[7].size());
        string t = "time: " +ofToString(timer);
        
        // INFORMATION TEXT
        ofDrawBitmapString(fr, ofGetWindowSize()[0]/2-500, guiPosition);
        ofDrawBitmapString(bn, ofGetWindowSize()[0]/2-500, guiPosition+15);
        ofDrawBitmapString(t, ofGetWindowSize()[0]/2-500, guiPosition+30);
        ofDrawBitmapString(mouseX, ofGetWindowSize()[0]/2-500, guiPosition+45);
        ofDrawBitmapString(mouseY, ofGetWindowSize()[0]/2-450, guiPosition+45);
        
        for(int i = 0; i<8; i++) {
            ofDrawBitmapString(boids[i].size(), ofGetWindowSize()[0]/2-500+(i*20), guiPosition+60);
        }
        
        ofShowCursor();

    } else {
        guiPosition = -500;
        ofHideCursor();
    }

    
    //TWEET WORDS COMING INTO SCREEN
    if(showTweetRandomly) {
//        ofBackground(0);
        font.beginBatch();
        for (int i = 0; i < count ; i++){
            particles[i].draw(&font);
        }
        count = count + particles.size()/200;
        if (count > particles.size()) {
            count = particles.size(); }
        font.endBatch();
    }
    
    //MAKE LETTERS SCATTERED AND DROPPED
    if(scattered) {
        font.beginBatch();
        for (int i = 0; i < letterParticles.size(); i++){
            letterParticles[i].draw(&font);
        }
        font.endBatch();
        
        for (int i = 0; i < circleParticles.size(); i++){
            circleParticles[i].drawCircle();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //    switch (key) {
    //        case 'f' :
    //            ofToggleFullscreen();
    //            break;
    //    }
    
    //HIDE GUI
    if(key == 'h') {
        bHide = !bHide;
    }
    
    //PLAY VIDEO AND STOP RIPPLE
    if ( key == 'v') {
        cout << "video play";
        bVideo = true;
        video.play();
        sound1.play();
        video.setVolume(0.0);
    }
    
    
    //STOP VIDEO & TWEET TEXT BEGIN
    if ( key == 't') {
        video.stop();
        bVideo = false;
        bRipple = false;
        
        initTweetDataProcessing();
        ofSleepMillis(1000);
        initTextParticle();
        ofSleepMillis(500);
        random_shuffle(particles.begin(), particles.end());
        showTweetRandomly = true;
    }
    
    //OTHER WORDS DISAPPEAR & SCATTERED/DROPPED
    if(key == 'd') {
        otherWordsDisapper = true;
    }
    
    if(key == 's') {
        gui.saveToFile("settings.xml");
    }
    if(key == 'l') {
        gui.loadFromFile("settings.xml");
    }
}

//--------------------------------------------------------------

void ofApp::soundSetup(){
    sound1.load("bgm.wav");
    int nFiles = dir.listDir("cloud");
    
    if(nFiles) {
        for(int i=0; i<dir.size(); i++) {
            // add the sound to the vector
            string filePath = dir.getPath(i);
            sclouds.push_back(ofSoundPlayer());
            sclouds.back().load(filePath);
        }
        
        for(int i=0; i<sclouds.size(); i++) {
            sclouds[i].setVolume(0.1f);
        }
    }
}

//--------------------------------------------------------------

void ofApp::soundEffectStop(){
        for(int i=0; i<sclouds.size(); i++) {
            sclouds[i].stop();
        }
    }

//--------------------------------------------------------------


void ofApp::audioOut(float* buffer, int bufferSize, int nChannels){
//    for(int i = 0; i< bufferSize; i++){
//        float currentSample = 0;
//
//        //        phase = ofGetElapsedTimef()*TWO_PI * frequency;
//        currentSample = sin(phase * amplitude);
//        phase = phase + phaseInc;
//
//        buffer[i*nChannels + 0] = currentSample; // left channel
//        buffer[i*nChannels + 1] = currentSample; // right channel
//    }
}


//--------------------------------------------------------------

void ofApp::onConnection () {
    isConnected = true;
    bindEvents();
}

//--------------------------------------------------------------

void ofApp::bindEvents () {
    std::string serverEventName = "server-event";
    socketIO.bindEvent(serverEvent, serverEventName);
    ofAddListener(serverEvent, this, &ofApp::onServerEvent);
}

//--------------------------------------------------------------

void ofApp::gotEvent(string& name) {
    status = name;
}

//--------------------------------------------------------------

void ofApp::onServerEvent (ofxSocketIOData& data) {
    
    //COUNT TOTAL INPUT NUMBER
    gotDataTotalNum++;
    
    int gotData = data.getIntValue("intData");
    
    //DATA -> PICK TWEET AND STORE THEM
    for(int i=0; i<20; i++) {
        selectionPool.push_back(gotData);
    }
    
    //DATA -> MAKE RIPPLE
    if (gotData){
        urlShow = false;
        bRipple = true;
        
        sclouds[ofRandom(22)].play();
        
        // CREATE A BLOB
        Boid v1;
        v1.setup();
        v1.color = colors[gotData]; // data range : 1~8
        v1.setLocation(ofGetWindowSize()[0]/2, ofGetWindowHeight()/2);
        
        // ALLOCATE TO DIFFERENT ARRAYS GROUPED BY COLORS
        boids[gotData-1].push_back(v1); // now has size
    }
}
