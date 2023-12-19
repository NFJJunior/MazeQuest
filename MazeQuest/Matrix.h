#include "LedControl.h"

struct Position {
    byte x;
    byte y;
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
#define KEY 3
#define RED_LED 4
#define DOOR 5

struct Matrix {
    //  Pins
    const byte din;
    const byte clock;
    const byte load;

    const byte redLed;
    const byte yellowLed;

    //  LedControl
    LedControl lc;

    //  Player
    Position playerPosition = {1, 1};
    byte playerLedState = HIGH;

    //  Map
    static const byte maxMatrixSize = 24;
    static const byte fogOfViewSize = 8;

    //  The position of top-left corner of the fog of view
    Position corner = {0, 0};

    byte matrixSize = 8;
    byte matrixMap[maxMatrixSize][maxMatrixSize];

    //  Bombs
    static const byte maxNrBombs = 37;

    byte nrBombs;
    Position bombs[maxNrBombs];

    //  Key
    Position keyPosition;
    bool hasKey = false;
    bool hasPickedUp = false;

    //  Door
    const Position doorPositions[3] = {{4, 6}, {0, 0}, {0, 0}};
    Position doorPosition;
    byte doorLedState = HIGH;

    //  The four directions the player can go to
    const int directions[4][2] = {
        {-1, 0},  //  UP
        {1, 0},   //  DOWN
        {0, -1},  //  LEFT
        {0, 1}   //  RIGHT
    };

    //  Flags
    bool gameLost = false;
    bool gameWon = false;

    unsigned long gameLostTime = 0;

    //  Constructor
    Matrix(byte din, byte clock, byte load, byte redLed, byte yellowLed) : din(din), clock(clock), load(load), redLed(redLed), yellowLed(yellowLed), lc(LedControl(din, clock, load, 1)) {}

    void setBrightness(const byte brightness) {
        lc.setIntensity(0, 16 / 8 * brightness - 1);
    }

    void setup() {
        lc.shutdown(0, false);
        lc.clearDisplay(0);

        pinMode(redLed, OUTPUT);
        pinMode(yellowLed, OUTPUT);
    }

    void movePlayer(const int move) {
        static Position temp;

        hasPickedUp = false;

        if (move == -1) {
            return;
        }

        temp.x = (byte)((int)playerPosition.x + directions[move][0]);
        temp.y = (byte)((int)playerPosition.y + directions[move][1]);

        //  If the next position is the bottom right corner,
        //  the game is won
        if (temp.x == matrixSize - 2 && temp.y == matrixSize - 2) {
            gameWon = true;

            return;
        }

        switch (matrixMap[temp.x][temp.y]) {
            case EMPTY: {
                playerPosition.x = temp.x;
                playerPosition.y = temp.y;

                break;
            }
            case BOMB: {
                gameLost = true;
                gameLostTime = millis();

                break;
            }
            case KEY: {
                playerPosition.x = temp.x;
                playerPosition.y = temp.y;

                matrixMap[temp.x][temp.y] = EMPTY;
                hasKey = true;
                hasPickedUp = true;

                break;
            }
            case RED_LED: {
                playerPosition.x = temp.x;
                playerPosition.y = temp.y;

                break;
            }
            case DOOR: {
                if (hasKey) {
                    playerPosition.x = temp.x;
                    playerPosition.y = temp.y;

                    matrixMap[temp.x][temp.y] = EMPTY;
                    doorLedState = HIGH;
                }

                break;
            }
        }
    }

    void changePlayerLed() {
        static const unsigned int blinkDelay = 200;
        static unsigned long lastBlinkTime;

        if (millis() - lastBlinkTime >= blinkDelay) {
            playerLedState = !playerLedState;
            lastBlinkTime = millis();
        }
    }

    void changeDoorLed() {
        static const unsigned int blinkDelay = 500;
        static unsigned long lastBlinkTime;

        if (millis() - lastBlinkTime >= blinkDelay) {
            doorLedState = !doorLedState;
            lastBlinkTime = millis();
        }
    }

    //  Find the top-left corner position for the fog of view
    void findCorner() {
        corner.x = (byte)min(max(0, (int)playerPosition.x - 3), matrixSize - fogOfViewSize);
        corner.y = (byte)min(max(0, (int)playerPosition.y - 3), matrixSize - fogOfViewSize);
    }

    void resetGame() {
        playerPosition.x = 1;
        playerPosition.y = 1;

        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);

        if (keyPosition.x != 0) {
            matrixMap[keyPosition.x][keyPosition.y] = KEY;
        }

        if (doorPosition.x != 0) {
            matrixMap[doorPosition.x][doorPosition.y] = DOOR;
        }

        gameLost = false;
        gameWon = false;

        hasKey = false;
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
        if (matrixMap[doorPosition.x][doorPosition.y] == DOOR) {
            changeDoorLed();
        }
        findCorner();

        for (int row = 0; row < fogOfViewSize; row++) {
            for (int col = 0; col < fogOfViewSize; col++) {
                if (corner.x + row == playerPosition.x && corner.y + col == playerPosition.y) {
                    lc.setLed(0, row, col, playerLedState);

                    continue;
                }

                if (corner.x + row == doorPosition.x && corner.y + col == doorPosition.y && matrixMap[doorPosition.x][doorPosition.y] == DOOR) {
                    lc.setLed(0, row, col, doorLedState);

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

        for (int k = 0; k < 4; k++) {
            if (matrixMap[playerPosition.x + directions[k][0]][playerPosition.y + directions[k][1]] == KEY) {
                digitalWrite(yellowLed, HIGH);

                return;
            }
        }

        digitalWrite(yellowLed, LOW);
    }
};

