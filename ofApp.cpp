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
    ofEnableAntiAliasing();
    
    // BOOLEAN SETTING
    bRipple = false;
    bVideo1 = false;
    bVideo2 = false;
    bHide = true;
    vid1isPlayed = true;
    vid2isPlayed = true;
    urlShow = true;
    scattered = false;
    showSentenceTransition = true;
    
    // GUI
    gui.setup();
    gui.setPosition(ofGetWindowSize()[0]/2, guiPosition);
    gui.add(rippleSize.setup("size", 100, 1, 150));
    gui.add(rippleDistance.setup("distance", 2, 1, 10));
    gui.add(rippleDamping.setup("damping", 0.995, 0.900, 0.999));
    gui.add(urlYpos.setup("urlYpos", 300, 0, ofGetWindowHeight()));
    gui.add(urlXpos1.setup("urlXpos1", 1075, 0, ofGetWindowSize()[0]));
    gui.add(urlXpos2.setup("urlXpos2", 3380, 0, ofGetWindowSize()[0]));
    gui.add(urlXpos3.setup("urlXpos3", 5450, 0, ofGetWindowSize()[0]));
    gui.add(urlSize.setup("urlSize", 100, 10, 300));
    gui.add(instructionSize.setup("instructionSize", 26, 10, 300));
    gui.add(effectAStart.setup("effectAStart", 50, 0, 1000));
    gui.add(effectATime.setup ("effectATime", 10,  0, 1000));
    gui.add(effectBStart.setup("effectBStart", 150, 0, 1000));
    gui.add(effectBTime.setup ("effectBTime", 30,  0, 1000));
    gui.add(effectCStart.setup("effectCStart", 250, 0, 1000));
    gui.add(effectCTime.setup ("effectCTime", 50,  0, 1000));
//    gui.loadFromFile("settings.xml");
    ofHideCursor();
    
    // SCENE MANAGER
    sceneManager.setup();
    sceneManager.setPosition(ofGetWindowSize()[0]/2 + 230, guiPosition);
    sceneManager.add(scene0.setup("scene0",  600, 0, 3000 )); // blobs go to attraction
    sceneManager.add(scene1.setup("scene1",  900, 0, 3000 )); // video 1 play
    sceneManager.add(scene2.setup("scene2", 1450, 0, 3000 )); // video 2 play, blobs disappear
    sceneManager.add(scene3.setup("scene3", 1810, 0, 3000 )); // sentences come and go out
    sceneManager.add(scene4.setup("scene4", 1910, 0, 3000 )); // words particles come in
    sceneManager.add(scene5.setup("scene5", 2120, 0, 3000 )); // words go out letters drop
    
    // SET STARTING POINT OF RIPPLE
    rip.allocate(ofGetWindowSize()[0], ofGetWindowHeight());
    
    // SOCKET SETUP
    isConnected = false;
    address = "http://67.205.153.66:9999";
    status = "not connected";
    socketIO.setup(address);
    ofAddListener(socketIO.notifyEvent, this, &ofApp::gotEvent);
    ofAddListener(socketIO.connectionEvent, this, &ofApp::onConnection);
    
    // VIDEO LOAD
    video1.setPixelFormat(OF_PIXELS_RGBA);          // has to be called before loading a movie
    video1.setLoopState(OF_LOOP_NORMAL);
    video1.load("1130_S01_PJQ90_p351.mov");
    video2.setPixelFormat(OF_PIXELS_RGBA);          // has to be called before loading a movie
    video2.setLoopState(OF_LOOP_NORMAL);
    video2.load("1130_S02_PJQ90_2.mov");
    
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
    
    // INIT BASIT SETTING FOR EMOTION COLORS AND KEYWORD
    initSetting();
    
    // SET TWEET TEXT SIZE & RAINDROP SPEED
    fontSize = 22;
    forceValue = 0.002;
    
    // Setup font for particle
    font.setup("LinLibertine_R.ttf", 1.0, 1024, false, 0, 1);
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
    countSentence = 0;
    spd = 0;
    countWord = 0;
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
        //Shuffle to change the order when showing each sentence
        random_shuffle(tweetText.begin(), tweetText.end());
    }
    else
    {
        ofLogNotice("ofApp::setup")  << "Failed to parse JSON" << endl;
    }
}

//--------------------------------------------------------------

void ofApp::initTextParticle(){
    
    //Sentence particle setup
    for (int i = 0; i < tweetText.size(); i++){
        ofRectangle sentencebbox = font.getBBox(tweetText[i], fontSize*15, 0, 0);
        float currentSentenceWidth = sentencebbox.width;
        float currentSentenceHeight = sentencebbox.height;
        
        particle sentenceParticle;
        sentenceParticle.setInitialCondition(ofGetWindowSize()[0]/2-currentSentenceWidth/2,ofGetWindowHeight()/2,500, 0, 0, 0);
        sentenceParticle.particleFontSize = fontSize*15;
        sentenceParticle.angle = ofRandom(-3, 3);
        sentenceParticle.finalWord = tweetText[i];
        sentenceParticle.fontColor = colors[int(ofRandom(1,9))];
        sentenceParticle.opacity = 175;
        sentenceParticle.sentenceForceX = ofRandom(-0.005, 0.005);
        sentenceParticle.sentenceForceY = ofRandom(-0.005, 0.005);
        sentenceParticle.sentenceForceZ = ofRandom(-15, -20);
        sentenceParticles.push_back(sentenceParticle);
    }
    
    //MAKE SET OF TWEET TEXT INTO ONE STRING
    tweetTextFinal = accumulate(begin(tweetText), end(tweetText), tweetTextFinal);
    cout<<"tweet data processing done"<<endl;
    
    //SPLIT TWEET TEXT INTO EVERY WORDS
    vector<string> words;
    words = ofSplitString(tweetTextFinal, " ");
    
    //MAKE WORD PARTICLE WITH ASSIGNING ATTRIBUTES
    ofRectangle firstbbox = font.getBBox(words[0], fontSize, 0, 0);
    float fontHeight = firstbbox.height;
    float xPosition = -firstbbox.width;
    float yPosition = fontHeight;
    ofRectangle spacebbox = font.getBBox("s", fontSize, 0, 0);
    float space = spacebbox.width;
    
    //WORD PARTICLE SETUP
    for (int i = 0; i < words.size(); i++){
        ofRectangle currentbbox = font.getBBox(words[i], fontSize, 0, 0);
        float currentFontWidth = currentbbox.width;
        particle wordParticle;
        wordParticle.setInitialCondition(xPosition,yPosition,0, 0, 0, 0);
        wordParticle.particleFontSize = fontSize;
        wordParticle.angle = ofRandom(-0.5, 0.5);
        wordParticle.finalWord = words[i];
        wordParticle.opacity = 0;
        wordParticle.opacitySpeed = ofRandom(0.6, 0.8);
        
        ofColor assignedColor;
        if(find(joy.begin(), joy.end(), words[i]) != joy.end()) {
            wordParticle.fontColor = colors[1];
            assignedColor = colors[1];
        } else if(find(trust.begin(), trust.end(), words[i]) != trust.end()) {
            wordParticle.fontColor = colors[2];
            assignedColor = colors[2];
        } else if(find(fear.begin(), fear.end(), words[i]) != fear.end()) {
            wordParticle.fontColor = colors[3];
            assignedColor = colors[3];
        } else if(find(surprise.begin(), surprise.end(), words[i]) != surprise.end()) {
            wordParticle.fontColor = colors[4];
            assignedColor = colors[4];
        } else if(find(sadness.begin(), sadness.end(), words[i]) != sadness.end()) {
            wordParticle.fontColor = colors[5];
            assignedColor = colors[5];
        } else if(find(disgust.begin(), disgust.end(), words[i])!= disgust.end()) {
            wordParticle.fontColor = colors[6];
            assignedColor = colors[6];
        } else if(find(anger.begin(), anger.end(), words[i]) != anger.end()) {
            wordParticle.fontColor = colors[7];
            assignedColor = colors[7];
        } else if(find(anticipation.begin(), anticipation.end(), words[i]) != anticipation.end()) {
            wordParticle.fontColor = colors[8];
            assignedColor = colors[8];
        } else {
            wordParticle.fontColor = colors[0];
            assignedColor = colors[0];
        }
        
        
        //MAKE LETTER PARTICLE FOR DROPPING LATER
        if(find(emotionKeywords.begin(), emotionKeywords.end(), words[i]) != emotionKeywords.end()) {
            
            string currentLetter = words[i];

            float xPositionLetter = xPosition;
            float yPositionLetter = yPosition;
            
            for(int j=0; j<currentLetter.size(); j++) {
                char s = currentLetter[j];
                string singleLetter = ofToString(s);
                ofRectangle letterbbox = font.getBBox(singleLetter, fontSize, 0, 0);
                float currentLetterWidth = letterbbox.width;
                float currentLetterHeight = letterbbox.height;
                
                //LETTER PARTICLE SETUP
                particle letterParticle;
                letterParticle.setInitialCondition(xPositionLetter,yPositionLetter,0, 0, 0, 0);
                letterParticle.particleFontSize = fontSize;
                letterParticle.angle = 0;
                letterParticle.angleSpeed = ofRandom(-2.5, 2.5);
                letterParticle.scatterdForceX = ofRandom(-forceValue*0.2, forceValue*0.2);
                letterParticle.scatterdForceY = ofRandom(0, forceValue*2);
                letterParticle.scatterdForceZ = ofRandom(-forceValue*0.5, forceValue*3);
                letterParticle.damping = ofRandom(forceValue*0.01, forceValue*0.05);
                letterParticle.finalWord = singleLetter;
                letterParticle.opacity = 255;
                letterParticle.fontColor = assignedColor;
                if(yPositionLetter < ofGetWindowHeight() + currentLetterHeight) {
                    letterParticles.push_back(letterParticle);
                }
                
                //CIRCLE PARTICLE SETUP
                for(int c = 0; c < 20; c++) {
                    particle circleParticle;
                    circleParticle.setInitialCondition(xPositionLetter,yPositionLetter,0, 0, 0, 0);
                    circleParticle.scatterdForceX = ofRandom(-forceValue*0.5, forceValue*0.5);
                    circleParticle.scatterdForceY = ofRandom(0, forceValue*2);
                    circleParticle.scatterdForceZ = ofRandom(-forceValue*0.5, forceValue*3.5);
                    circleParticle.damping = ofRandom(forceValue*0.01, forceValue*0.05);
                    circleParticle.fontColor = assignedColor;
                    circleParticle.opacity = 5;
                    circleParticle.opacitySpeed = ofRandom(1.5, 2.5);
                    if(yPositionLetter < ofGetWindowHeight() + currentLetterHeight) {
                        circleParticles.push_back(circleParticle);
                    }
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
        if(yPosition < ofGetWindowHeight() + fontHeight) {
            wordParticles.push_back(wordParticle);
        }
    }
    //Shuffle to change the order when showing each word
    random_shuffle(wordParticles.begin(), wordParticles.end());
}

//--------------------------------------------------------------



//--------------------------------------------------------------

void ofApp::update(){
    int ranSize = ofRandom(-10.0, 10.0);
    
    if(bRipple){
        rip.begin();
        ofFill();
        
        for (unsigned int i = 0; i < boids.size(); i++){
            for (unsigned int j = 0; j < boids[i].size(); j++){
                
                // SET FORCES
                ofVec2f force = attractors[i].attract(boids[i][j]);
                boids[i][j].flock(boids[i]);
                
                // SCENE 1 GO TO THE ATTRACTORS
                if(timer >= scene0){
                    boids[i][j].applyForce(force*(2+i*0.1));
                    if(timer <= scene0 + 25) {
                        if(j>0){
                            ofDrawLine(boids[i][j].location.x, boids[i][j].location.y, boids[i][j-1].location.x, boids[i][j-1].location.y);
                        }
                    }
                }
                
                boids[i][j].update();
                boids[i][j].borders();
            }
        }
        
        for (unsigned int i = 0; i < boids.size(); i++){
            for (unsigned int j = 0; j < boids[i].size(); j++){
                
                ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                boids[i][j].draw(rippleSize + ofRandom(10));
                
                if(bVideo1){
                    //                     MAKE ATTRACTORS VISIBLE
//                    for (int i = 0 ; i < 8; i++ ){
//                        attractors[i].draw();
//                    }
                }
            }
        }
        rip.end();
        rip.update();
    }
    
    // VIDEO PLAY
    if(bVideo1){
        video1.update();
    }
    
    if(bVideo2){
        video2.update();
    }
    
    // SCENE3
    // Sentence particle force update
    if(timer > scene3) {
        for (int i = 0; i < countSentence; i++){
            sentenceParticles[i].addSentenceForce();
            sentenceParticles[i].update();
            sentenceParticles[i].resetForce();
            sentenceParticles[i].addAppearForce();
        }
        spd += 0.003f;
        if (countSentence >= 250)
        {
            countSentence = 250;
            sentenceParticles.clear();
        } else {
            countSentence = powf(1+spd, 8);;
        }
//        cout<<countSentence<<endl;
    }
    
    // SCENE 4
    if(timer>scene4) {
        // Word particle force update
        for (int i = 0; i < wordParticles.size(); i++){
            wordParticles[i].appearAllWords();
            wordParticles[i].update();
            wordParticles[i].resetForce();
            wordParticles[i].addAppearForce();
        }
        
        // Letter particle force update
        for (int i = 0; i < letterParticles.size(); i++){
            letterParticles[i].resetForce();
            letterParticles[i].addAppearForce();
            letterParticles[i].update();
        }
        
        // Circle particle force update
        for (int i = 0; i < circleParticles.size(); i++){
            circleParticles[i].resetForce();
            circleParticles[i].addAppearForce();
            circleParticles[i].update();
        }
    }
    
    // SCENE 5 : MAKE NON EMOTION-RELATED WORD DISAPPEAR
    if(timer>scene5) {
        for (int i = 0; i < wordParticles.size(); i++){
            if(find(emotionKeywords.begin(), emotionKeywords.end(), wordParticles[i].finalWord) == emotionKeywords.end()) {
                wordParticles[i].disappearOtherWords();
                if(wordParticles[i].opacity < -200) {
                    scattered = true;
                    wordParticles.clear();
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
            circleParticles[i].appearAllCircles();
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
    timer = int(ofGetElapsedTimeMillis() - startTime)/100;
    

    if(urlShow) {
        //URL AND INSTRUCTION INITIAL
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
        instruction.drawString(instructionText, urlXpos3 - urlWidth/2, urlYpos+ urlHeight);
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
        
        // ADJUSTING BLOB SIZE
        if(bVideo1 == false && bVideo2 == false){
            float sinOfTime               = sin( ofGetElapsedTimef()/2);
            float sinOfTimeMapped         = ofMap(sinOfTime, -1, 1, 20, 120);
            float sinOfTimeMappedDamp     = ofMap(sinOfTime, -1, 1, 0.950, 0.999);
            float sinOfTimeMappedDistance = ofMap(sinOfTime, -1, 1, 2.0, 9.0);
            
            rippleSize = sinOfTimeMapped;
            rippleDamping = sinOfTimeMappedDamp;
            rippleDistance = sinOfTimeMappedDistance; // 9  - 2
            
            if(timer >= effectAStart  && timer <= effectAStart + effectATime)  {
                rippleDistance = 2+ofRandom(8);
            } else if(timer >= effectBStart  && timer <= effectBStart + effectBTime)  {
                rippleDistance = 2+ofRandom(8);
            } else if(timer >= effectCStart  && timer <= effectCStart + effectCTime)  {
                rippleDistance = 2+ofRandom(8);
            }
        }
    }
    
    // SCENE 1 SETUP
    if(timer >= scene1){
        bVideo1 = true;
        if(vid1isPlayed == true) {
            video1.play();
            sound1.play();
            video1.setVolume(0.0);
            vid1isPlayed = false;
        }
    }
    
    // SCENE 1 DRAW
    if(bVideo1){
        // URL DISAPPEAR
        urlYpos = -500;
        
        // OFF THE RIPPLE SOUND EFFECT
        soundEffectStop();
        
        // DECREASE RIPPLE SIZE
        if(rippleFlag == 0 ){
            rippleSize = 50;
            rippleFlag = 1;
        }
        
        // DECREASE AND FIX THE SIZE WHILE HOVERING
        if(rippleSize > 20) {
            rippleSize = rippleSize - 0.2;
        }
        
        if (rippleDamping > 0.9) {
            rippleDamping = rippleDamping - 0.001;
        }
        rippleDistance = 2.0;
        
        //DELETE ALPHA CHANNEL
        ofEnableAlphaBlending();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        video1.draw(0, 0, ofGetWindowSize()[0], ofGetWindowHeight());
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ofDisableAlphaBlending();
    }
    
    // SCENE 2 SETUP
    if (timer >= scene2 && vid2isPlayed == true) {     // start decreasing size
        bVideo1 = false;
        bVideo2 = true;
        video1.close();
        video2.play();
        video2.setVolume(0.0);
        //        if (rippleFlag == 1) {
        //            rippleSize = 20;
        //            rippleFlag = 2;
        //        }
        vid2isPlayed = false;
    }
    
    // SCENE 2 DRAW
    if(bVideo2){
        ofEnableAlphaBlending();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        video2.draw(0, 0, ofGetWindowSize()[0], ofGetWindowHeight());
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ofDisableAlphaBlending();
        
        //rippleSize = 20;
        rippleSize = rippleSize - 0.1;
    }
    
    // BLOB DISAPPEAR AFTER SCENE 2
    if(rippleSize <= 1){
        rippleSize = 0;
        for (unsigned int i = 0; i < 8 ; i++){
            for (unsigned int j = 0; j < boids[i].size(); j++){
                boids[i].pop_back();
            }
        }
    }
    
    // GUI & INFORMATION TEXT DRAW
    gui.draw();
    sceneManager.draw();
    
    string fr = "fr: " + ofToString(ofGetFrameRate());
    string bn = "total blob: " + ofToString(boids[0].size()+boids[1].size()+boids[2].size()+boids[3].size()+boids[4].size()+boids[5].size()+boids[6].size()+boids[7].size());
    string t = "time: " +ofToString(timer);
    string pt = "total sentence: " + ofToString(sentenceParticles.size()) +" total word: " + ofToString(wordParticles.size()) + " total letter: " + ofToString(letterParticles.size()) + " total circle: " + ofToString(circleParticles.size());
    
    
    ofSetColor(255,255,255);
    ofDrawBitmapString(fr, ofGetWindowSize()[0]/2-500, guiPosition);
    ofDrawBitmapString(bn, ofGetWindowSize()[0]/2-500, guiPosition+15);
    ofDrawBitmapString(t, ofGetWindowSize()[0]/2-500, guiPosition+30);
    ofDrawBitmapString(mouseX, ofGetWindowSize()[0]/2-500, guiPosition+45);
    ofDrawBitmapString(mouseY, ofGetWindowSize()[0]/2-450, guiPosition+45);
    ofDrawBitmapString(pt, ofGetWindowSize()[0]/2-500, guiPosition+75);
    
    
    for(int i = 0; i<8; i++) {
        ofDrawBitmapString(boids[i].size(), ofGetWindowSize()[0]/2-500+(i*20), guiPosition+60);
    }
    
    if(bHide == false) {
        guiPosition = ofGetWindowHeight()/10;
        gui.setPosition(ofGetWindowSize()[0]/2, guiPosition);
        sceneManager.setPosition(ofGetWindowSize()[0]/2+230, guiPosition);
        ofShowCursor();
        
    } else {
        guiPosition = -500;
        gui.setPosition(ofGetWindowSize()[0]/2, guiPosition);
        sceneManager.setPosition(ofGetWindowSize()[0]/2+230, guiPosition);
        ofHideCursor();
    }
    
    
    // SCENE3 SETUP : SENTENCE PARTICLE
    if (timer >= scene3 && showSentenceTransition == true) {
        bVideo2 = false;
        video2.close();
        bRipple = false;
        ofSleepMillis(500);
        initTweetDataProcessing();
        ofSleepMillis(1000);
        initTextParticle();
        showSentenceTransition = false;
    }
        
        
    if (timer >= scene3) {
        font.beginBatch();
        for (int i = 0; i < countSentence; i++){
            sentenceParticles[i].draw(&font);
        }
        font.endBatch();
    }
    
    //SCENE4 DRAW : WORDS PARTICLE
    if(timer>scene4) {
        font.beginBatch();
        for (int i = 0; i < countWord ; i++){
            wordParticles[i].draw(&font);
        }
        countWord = countWord + wordParticles.size()/200;
        if (countWord > wordParticles.size()) {
            countWord = wordParticles.size(); }
        font.endBatch();
    }
    
    
    if(scattered) {
        //LETTER PARTICLE DRAW
        font.beginBatch();
        for (int i = 0; i < letterParticles.size(); i++){
            letterParticles[i].draw(&font);
        }
        font.endBatch();
        
        //CIRCLE PARTICLE DRAW
        for (int i = 0; i < circleParticles.size(); i++){
            circleParticles[i].drawCircle();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    //HIDE GUI
    if(key == 'h') {
        bHide = !bHide;
    }
    
    //PLAY VIDEO AND STOP RIPPLE
    //    if ( key == 'v') {
    //        cout << "video play";
    //        bVideo1 = true;
    //        video1.play();
    //        sound1.play();
    //        video1.setVolume(0.0);
    //    }
    
    
    if(key == 'z') {
//        video2.close();
//        bVideo2 = false;
//        bRipple = false;
//
//        ofSleepMillis(500);
//        initTweetDataProcessing();
//        ofSleepMillis(1000);
//        initTextParticle();
//        showSentenceTransition = true;
    }
    
    //STOP VIDEO & TWEET TEXT BEGIN
    if ( key == 't') {
//        showTweetRandomly = true;
    }
    
    //OTHER WORDS DISAPPEAR & SCATTERED/DROPPED
    if(key == 'd') {
//        otherWordsDisapper = true;
    }
    
//    if(key == 's') {
//        gui.saveToFile("settings.xml");
//        sceneManager.saveToFile("settings.xml");
//    }
//    if(key == 'l') {
//        gui.loadFromFile("settings.xml");
//        sceneManager.saveToFile("settings.xml");
//    }
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
    for(int i=0; i<10; i++) {
        selectionPool.push_back(gotData);
    }
    
    //DATA -> MAKE RIPPLE
    if (gotData){
        urlShow = false;
        bRipple = true;
        
        sclouds[ofRandom(9)].play();
        
        // CREATE A BLOB
        Boid v1;
        v1.setup();
        v1.color = colors[gotData]; // data range : 1~8
        v1.setLocation(ofGetWindowSize()[0]*1/4 + ofRandom(ofGetWindowSize()[0]/2), ofGetWindowHeight()/4 + ofRandom(ofGetWindowHeight()/2));
        // ALLOCATE TO DIFFERENT ARRAYS GROUPED BY COLORS
        boids[gotData-1].push_back(v1); // now has size
    }
}
