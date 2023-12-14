#include "LedControl.h"

struct Position {
    int x;
    int y;
};

#define MAZE 0
#define TROPHY 1
#define H 2
#define WRENCH 3
#define INFO 4

const byte images[5][8] = {
    {
        0b11111111,
        0b10000001,
        0b10101101,
        0b10101101,
        0b10010001,
        0b10110111,
        0b10010001,
        0b11111111
    }, {
        0b01111110,
        0b01111110,
        0b01111110,
        0b00111100,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00111100
    }, {
        0b01100110,
        0b01100110,
        0b01100110,
        0b01111110,
        0b01111110,
        0b01100110,
        0b01100110,
        0b01100110
    }, {
        0b00001100,
        0b00011000,
        0b00011001,
        0b00011111,
        0b00111110,
        0b01110000,
        0b11100000,
        0b11000000
    }, {
        0b00011000,
        0b00011000,
        0b00000000,
        0b00111000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00111100
    }
};

const byte explosionSize = 8;
const byte explosion[explosionSize][8] = {
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011000,
        0b00000000,
        0b00000000,
        0b00000000
    }, {
        0b00000000,
        0b00000000,
        0b00011000,
        0b00111100,
        0b00111100,
        0b00011000,
        0b00000000,
        0b00000000
    }, {
        0b00000000,
        0b00011000,
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b00011000,
        0b00000000
    }, {
        0b00011000,
        0b00111100,
        0b01100110,
        0b11000011,
        0b11000011,
        0b01100110,
        0b00111100,
        0b00011000
    }, {
        0b00111100,
        0b01100110,
        0b11000011,
        0b10000001,
        0b10000001,
        0b11000011,
        0b01100110,
        0b00111100
    }, {
        0b01100110,
        0b11000011,
        0b10000001,
        0b00000000,
        0b00000000,
        0b10000001,
        0b11000011,
        0b01100110
    }, {
        0b10000001,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b10000001
    }, {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
    }
};

#define EMPTY 0
#define WALL 1
#define BOMB 2
#define RED_LED 3
#define YELLOW_LED 4

struct Matrix {
    //  Pins
    const byte din;
    const byte clock;
    const byte load;

    const byte redLed;

    static const byte maxMatrixSize = 24;
    static const byte fogOfViewSize = 8;
    byte matrixSize = 8;

    LedControl lc;

    // byte matrixMap[maxMatrixSize][maxMatrixSize] = {
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	//     {1, 0, 3, 2, 3, 0, 0, 1, 0, 3, 0, 0, 3, 0, 0, 3, 0, 3, 2, 1},
	//     {1, 0, 0, 3, 3, 1, 0, 1, 3, 2, 1, 1, 2, 1, 1, 2, 0, 3, 3, 1},
	//     {1, 1, 1, 1, 2, 3, 3, 2, 3, 3, 0, 1, 3, 0, 1, 3, 1, 2, 3, 1},
	//     {1, 3, 0, 0, 3, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 3, 0, 1, 0, 1},
	//     {1, 2, 3, 1, 3, 0, 1, 3, 2, 3, 1, 3, 2, 3, 3, 2, 3, 1, 0, 1},
    //     {1, 3, 0, 1, 2, 3, 0, 0, 1, 0, 1, 0, 3, 0, 0, 3, 0, 1, 0, 1},
    //     {1, 0, 0, 1, 3, 0, 0, 1, 0, 0, 1, 1, 0, 0, 3, 0, 1, 3, 0, 1},
    //     {1, 3, 0, 0, 1, 3, 3, 2, 3, 0, 1, 2, 3, 3, 2, 3, 1, 2, 3, 1},
    //     {1, 2, 1, 0, 3, 2, 3, 1, 3, 0, 1, 3, 0, 0, 1, 0, 1, 3, 0, 1},
    //     {1, 3, 0, 3, 0, 1, 0, 1, 2, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    //     {1, 0, 1, 2, 3, 1, 0, 1, 3, 0, 1, 0, 0, 0, 1, 3, 2, 3, 0, 1},
    //     {1, 0, 0, 3, 0, 1, 0, 1, 3, 0, 1, 3, 1, 0, 1, 1, 3, 0, 0, 1},
    //     {1, 0, 1, 3, 0, 1, 0, 3, 2, 1, 3, 2, 3, 3, 1, 3, 0, 0, 3, 1},
    //     {1, 3, 3, 2, 3, 1, 0, 0, 3, 0, 0, 3, 3, 2, 1, 2, 3, 3, 2, 1},
    //     {1, 2, 1, 3, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 3, 3, 0, 1, 1, 1},
    //     {1, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 2, 3, 0, 0, 0, 1},
    //     {1, 0, 1, 3, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 1},
    //     {1, 0, 3, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 3, 1},
    //     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    // };

    byte matrixMap[maxMatrixSize][maxMatrixSize] = {
        {1, 1, 1, 1, 1, 1, 1, 1},
	    {1, 0, 2, 0, 0, 0, 0, 1},
	    {1, 0, 0, 0, 0, 0, 0, 1},
	    {1, 0, 0, 0, 0, 0, 0, 1},
	    {1, 0, 0, 0, 0, 0, 0, 1},
	    {1, 0, 0, 0, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };



    //  The four directions the player can go to
    const int directions[4][2] = {
        {-1, 0},  //  UP
        {1, 0},   //  DOWN
        {0, -1},  //  LEFT
        {0, 1}   //  RIGHT
    };

    //  Player variables
    Position playerPosition = {1, 1};
    byte playerLedState = HIGH;

    //  The position of top-left corner of the fog of view
    Position corner = {0, 0};

    bool gameLost = false;
    unsigned long gameLostTime = 0;

    bool gameWon = false;

    //  Flag that tells us if this level has a key
    bool hasKey = false;

    //  Constructor
    Matrix(byte din, byte clock, byte load, byte redLed) : din(din), clock(clock), load(load), redLed(redLed), lc(LedControl(din, clock, load, 1)) {}

    void setBrightness(const byte brightness) {
        lc.setIntensity(0, 16 / 8 * brightness - 1);
    }

    void setup() {
        lc.shutdown(0, false);
        lc.clearDisplay(0);

        pinMode(redLed, OUTPUT);
    }

    void movePlayer(const int move) {
        static Position temp;

        if (move == -1) {
            return;
        }

        temp.x = playerPosition.x + directions[move][0];
        temp.y = playerPosition.y + directions[move][1];

        //  If the next position is the bottom right corner,
        //  the game is won
        if (temp.x == matrixSize - 2 && temp.y == matrixSize - 2) {
            gameWon = true;

            return;
        }

        //  If the next position is 0, move the player to it
        if (matrixMap[temp.x][temp.y] != 1) {
            playerPosition.x = temp.x;
            playerPosition.y = temp.y;
        }
        
        //  If the next position is a bomb, the game is lost
        if (matrixMap[temp.x][temp.y] == 2) {
            gameLost = true;
            gameLostTime = millis();
        }
    }

    void changePlayerLed() {
        static const unsigned int blinkDelay = 250;
        static unsigned long lastBlinkTime;

        if (millis() - lastBlinkTime >= blinkDelay) {
            playerLedState = !playerLedState;
            lastBlinkTime = millis();
        }
    }

    //  Find the top-left corner position for the fog of view
    void findCorner() {
        corner.x = min(max(0, playerPosition.x - 3), matrixSize - fogOfViewSize);
        corner.y = min(max(0, playerPosition.y - 3), matrixSize - fogOfViewSize);
    }

    void resetGame() {
        playerPosition.x = 1;
        playerPosition.y = 1;

        gameLost = false;
        gameWon = false;
    }

    void showImage(const byte image) {
        for (int row = 0; row < fogOfViewSize; row++) {
            lc.setRow(0, row, images[image][row]);
        }
    }   

    //  Show the current state of matrix
    void showGame() {
        if (gameLost) {
            static const int explosionDelay = 100;

            for (int row = 0; row < fogOfViewSize; row++) {
                lc.setRow(0, row, explosion[min((millis() - gameLostTime) / explosionDelay, explosionSize - 1)][row]);
            }

            return;
        }

        if (gameWon) {
            showImage(TROPHY);

            return;
        }

        changePlayerLed();
        findCorner();

        for (int row = 0; row < fogOfViewSize; row++) {
            for (int col = 0; col < fogOfViewSize; col++) {
                if (corner.x + row == playerPosition.x && corner.y + col == playerPosition.y) {
                    lc.setLed(0, row, col, playerLedState);

                    continue;
                }

                if (matrixMap[corner.x + row][corner.y + col] == WALL) {
                    lc.setLed(0, row, col, HIGH);
                }
                else {
                    lc.setLed(0, row, col, LOW);
                }
            }
        }

        if (matrixMap[playerPosition.x][playerPosition.y] == RED_LED) {
            digitalWrite(redLed, HIGH);
        }
        else {
            digitalWrite(redLed, LOW);
        }
    }
};

