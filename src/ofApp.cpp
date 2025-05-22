#include "ofApp.h"

//------------------------------------------------------------------------------------------------
void ofApp::setup(){
    cam.setPosition(0, 0, 800);
    cam.lookAt(glm::vec3(0, 0, 0));
    ofSetBackgroundColor(0, 0, 0); //black
    generateMaze();
    player = Player();
    keysCollected = false;
    keys = Keys();
    startScreen = true;
    gameRunning = false;
    Victory = false;
    gameOver = false;
    slimeDead = false;
    gameStartTime = 0;
    logMessages.clear();
    addLogMessage("Collect the key and escape!");
    addLogMessage("Type 'Take key' when near the key");
    addLogMessage("...There may be monsters blocking your way");
    addLogMessage("Type 'attack' when near a monster");
}
//--------------------------------------------------------------------------------------------------
void ofApp::update(){
    if (Victory || gameOver || startScreen) return;
    updateCamera();
    float elapsedTime = ofGetElapsedTimef() - gameStartTime;
    float remainingTime = timeLimit - elapsedTime;
    if (remainingTime <= 0.0f){
        gameOver = true;
        gameRunning = false;
        return;
    }
    // Check victory conditions
    float distanceToExit = glm::distance(player.position, exit);
    if (keysCollected && distanceToExit < 30.0f) {
        Victory = true;
        addLogMessage("Well done! You reached the exit!");
    }
    // reveal walls based on player being nearby
    for (auto& wall : mazeWalls) {
        if (glm::distance(wall.position, player.position) < 100) {
            wall.wallColour = ofColor(128, 128, 128);  //grey
        }
    }
    if (keysCollected) {
        player.setColor(ofColor(255, 215, 0));  //gold
    }
}
//---------------------------------------------------------------------------------------------------
void ofApp::draw(){
    if (startScreen) {
        ofSetColor(255, 255, 255);
        ofDrawBitmapString("TYPING DUNGEON", ofGetWidth() / 2 - 100, ofGetHeight() / 2 - 50);
        ofDrawBitmapString("Use arrow keys to move.", ofGetWidth() / 2 - 150, ofGetHeight() / 2);
        ofDrawBitmapString("Collect the key and find the exit.", ofGetWidth() / 2 - 200, ofGetHeight() / 2 + 30);
        ofDrawBitmapString("Press Enter to Start!", ofGetWidth() / 2 - 120, ofGetHeight() / 2 + 60);
        return;
    }
    if (Victory) {
        ofSetColor(0, 255, 0); //green
        ofDrawBitmapString("YOU WIN!", ofGetWidth() / 2 - 50, ofGetHeight() / 2);
        ofDrawBitmapString("Press 'R' to Restart", ofGetWidth() / 2 - 100, ofGetHeight() / 2 + 20);
        return;
    }
    if (gameOver) {
        ofSetColor(255, 0, 0); //red
        ofDrawBitmapString("GAME OVER", ofGetWidth() / 2 - 50, ofGetHeight() / 2);
        ofDrawBitmapString("Press 'R' to Restart", ofGetWidth() / 2 - 100, ofGetHeight() / 2 + 20);
        return;
    }
    if (gameRunning) {
    cam.begin();
    for (auto& wall : mazeWalls) {
        wall.draw();
    }
    ofSetColor(128, 128, 128);  //grey
    ofDrawSphere(entrance, 20);
    ofSetColor(100, 100, 0);  //brown
    ofDrawSphere(exit, 20);
    player.draw();
    keys.draw();
    slime.draw();
    cam.end();
    ofSetColor(255, 255, 0);
    ofDrawBitmapString("Type: " + typingInput, 850, 20);
    drawMessageBox();
    ofSetColor(255, 255, 255);
    float elapsedTime = ofGetElapsedTimef() - gameStartTime;
    float remainingTime = timeLimit - elapsedTime;
    string timerText = "Time left: " + ofToString(max(remainingTime, 0.0f), 1) + "s";
    ofDrawBitmapString(timerText, 10, ofGetHeight() - 20);
   }
}
//-----------------------------------------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (startScreen && key == OF_KEY_RETURN) {
        // Start the game
        startScreen = false;
        gameRunning = true;
        gameStartTime = ofGetElapsedTimef();
        return;
    }
    if ((Victory || gameOver) && key == 'r') {
         setup();  //restart
         return;
     }
    if (gameRunning) {
    glm::vec3 newPosition = player.position;

    if (key == OF_KEY_UP) {
        newPosition.x -= 10;
    }
    if (key == OF_KEY_DOWN) {
        newPosition.x += 10;
    }
    if (key == OF_KEY_LEFT) {
        newPosition.z += 10;
    }
    if (key == OF_KEY_RIGHT) {
        newPosition.z -= 10;
    }
    if (!collisionDetection(newPosition)) {
        player.position = newPosition;
    }
    if (key == OF_KEY_RETURN) {
        if (typingInput == "take key" && isNearKey(player.position, keys.position)) {
            keys.collect();
            keysCollected = true;
            addLogMessage("Key collected!");
        }  else if(keysCollected == false && isNearKey(player.position, keys.position)){
            addLogMessage("try again");
        }
        if (typingInput == "attack" && isNearSlime(player.position, slime.position)) {
            slime.attack();
            slimeDead = true;
            addLogMessage("Be Gone Slime!");
        }  else if(slimeDead == false && isNearSlime(player.position, slime.position)){
            addLogMessage("oh no! you missed");
        }
        typingInput = "";
    }
  }
}
//-------------------------------------------------------------------------------------------
bool ofApp::isNearKey(glm::vec3 playerPos, glm::vec3 keyPos) {
    float distance = glm::distance(player.position, keys.position);
    ofLog() << "Distance from player to key: " << distance;
    return distance < 30.0f;
}

bool ofApp::isNearSlime(glm::vec3 playerPos, glm::vec3 slimePos) {
    float distance = glm::distance(player.position, slime.position);
    ofLog() << "Distance from player to slime: " << distance;
    return distance < 50.0f;
}

//---------------------------------------------------------------------------------------------
bool ofApp::collisionDetection(glm::vec3 newPosition) {
    for (auto& wall : mazeWalls) {
        float wallSize = wall.size;
        glm::vec3 wallMin = wall.position - glm::vec3(wallSize / 2);
        glm::vec3 wallMax = wall.position + glm::vec3(wallSize / 2);

        // check radius of player touches wall
        if (newPosition.x + 20 > wallMin.x && newPosition.x - 20 < wallMax.x &&
            newPosition.z + 20 > wallMin.z && newPosition.z - 20 < wallMax.z) {
            return true;
        }
    }
     if (!slime.isAttacked) {
        float slimeRadius = 20.0f;
        float distanceToSlime = glm::distance(newPosition, slime.position);
        if (distanceToSlime < (slimeRadius + 20.0f)) {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == OF_KEY_BACKSPACE) {
        if (typingInput.length() > 0) {
            typingInput = typingInput.substr(0, typingInput.length() -1);
        }
    } else {
        typingInput += (char)key;
    }
}
//----------------------------------------------------------------------------------------------
void ofApp::addLogMessage(const string& message) {
    logMessages.push_back(message);
    if (logMessages.size() > maxLogMessages) {
        logMessages.pop_front();
    }
}
//-----------------------------------------------------------------------------------------------
void ofApp::drawMessageBox() {
    int boxWidth = 450;
    int boxHeight = 20 * maxLogMessages;
    int boxX = 10;
    int boxY = 10;
    ofSetColor(0, 0, 0, 150); //black with transparency
    ofDrawRectangle(boxX, boxY, boxWidth, boxHeight);
    ofSetColor(255, 255, 255); //white
    int messageY = boxY + 15;  //padding
    for (const string& message : logMessages) {
        ofDrawBitmapString(message, boxX + 10, messageY);
        messageY += 20; //next line
    }
}
//----------------------------------------------------------------------------------------------
void ofApp::generateMaze(){
    maze = {
         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
         {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1},
         {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1},
         {1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1},
         {1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1},
         {1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1},
         {1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1},
         {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1},
         {1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1},
         {1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1},
         {1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
         {1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1},
         {1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
         {1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
         {1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1},
         {1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1},
         {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1},
         {1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1},
         {1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
         {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
        entrance = glm::vec3(1 * 40, 0, 1 * 40);
        exit = glm::vec3(18 * 40, 0, 18 * 40);
     for (int i = 0; i < mazeHeight; ++i) {
        for (int j = 0; j < mazeWidth; ++j) {
            if (maze[i][j] == 1) {
                glm::vec3 wallPosition(j * 40, 0, i * 40);
                mazeWalls.push_back(MazeWall(wallPosition));
            }
        }
    }
}
//------------------------------------------------------------------------------------------------
void ofApp::updateCamera(){
    float camX = player.position.x + cameraDistance * cos(cameraAngle);
    float camZ = player.position.z + cameraDistance * sin(cameraAngle);
    float camY = player.position.y + cameraHeight;
    cam.setPosition(camX, camY, camZ);
    cam.lookAt(player.position);
}
