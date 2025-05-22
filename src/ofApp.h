#pragma once
#include "ofMain.h"
#include <deque>

//--------------------------------------------
class Keys {
public:
    glm::vec3 position;
    bool isCollected = false;
    void draw() {
        if (!isCollected) {
            position = glm::vec3(8 * 40, 0, 9 * 40);
            ofSetColor(255, 215, 0);  //gold
            ofDrawSphere(position, 20);
        }
    }
    void collect() {
        isCollected = true;
    }
};
//---------------------------------------------
class Slime {
public:
    glm::vec3 position;
    bool isAttacked = false;

    void draw() {
        if (!isAttacked) {
            position = glm::vec3(15 * 40, 0, 16 * 40);
            ofSetColor(0, 255, 0);  //green
            ofDrawSphere(position, 20);
        }
    }

    void attack() {
        isAttacked = true;
    }
};
//--------------------------------------------
class Player {
public:
    glm::vec3 position;
   // float health = 100.0f;
    ofColor playerColour;
    Player() {
        position = glm::vec3(1 * 40, 0, 1 * 40);
        playerColour = ofColor(0, 0, 225);  //blue
    }
    void move(float x, float y, float z) {
        position.x += x;
        position.y += y;
        position.z += z;
    }
    void setColor(const ofColor& color) {
        playerColour = color;
    }
    void draw() {
        ofSetColor(playerColour);
        ofDrawSphere(position, 20);
    }
};
//-----------------------------------------
class MazeWall {
public:
    glm::vec3 position;
    float size = 40;
    ofColor wallColour;
    MazeWall(glm::vec3 pos, ofColor colour = ofColor::black) {
        position = pos;
        wallColour = colour;
    }
    void draw() {
        ofSetColor(wallColour);
        ofDrawBox(position, size);
    }
};
//---------------------------------------
class ofApp : public ofBaseApp {
public:
    Player player;
    ofCamera cam;
    vector<MazeWall> mazeWalls;
    deque<string> logMessages;
    int maxLogMessages = 10;
    void addLogMessage(const string& message);
    void drawMessageBox();
    bool Victory = false;
    float cameraDistance = 150.0f;
    float cameraHeight = 200.0f;
    float cameraAngle = 0.0f;
    int mazeWidth = 20;
    int mazeHeight = 20;
    vector<vector<int>> maze;
    glm::vec3 entrance;
    glm::vec3 exit;
    void setup();
    void update();
    void draw();
    bool isNearExit(glm::vec3 PlayerPos, glm::vec3 exitPos);
    bool isNearSlime(glm::vec3 PlayerPos, glm::vec3 slimePos);
    bool slimeDead = false;
    Slime slime;
    bool isNearKey(glm::vec3 playerPos, glm::vec3 keyPos);
    bool keysCollected = false;
    Keys keys;
    string typingInput = "";
    string inputBuffer = "";
    bool collisionDetection(glm::vec3 newPosition);
    void keyPressed(int key);
    void keyReleased(int key);
    void generateMaze();
    void updateCamera();
    float gameStartTime;
    float timeLimit = 120.0f;
    bool gameOver = false;
    bool gameRunning = false;
    bool startScreen = true;
};
