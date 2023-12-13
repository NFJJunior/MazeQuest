//  The moves the player can make with the joystick
#define NO_MOVE -1
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

struct Joystick {
    //  Pins
    const byte X;
    const byte Y;

    //  Constructor
    Joystick(byte X, byte Y) : X(X), Y(Y) {}

    void setup() {
        pinMode(X, INPUT);
        pinMode(Y, INPUT);
    }

    //  Read X and Y values with debounce
    int readValues() {
        static int xValue;
        static int yValue;

        static const int minThreshold = 400;
        static const int maxThreshold = 640;

        //  Flag that tells us if the joy has moved
        static bool joyMoved = false;

        //  Variables for debounce
        static const unsigned int moveDelay = 50;
        static unsigned long lastMoveTime;

        xValue = analogRead(X);
        yValue = analogRead(Y);

        if (xValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
            joyMoved = true;

            return UP;
        }

        if (xValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
            joyMoved = true;

            return DOWN;
        }

        if (yValue < minThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
            joyMoved = true;

            return RIGHT;
        }

        if (yValue > maxThreshold && joyMoved == false && (millis() - lastMoveTime) >= moveDelay) {
            joyMoved = true;

            return LEFT;
        }

        if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
            joyMoved = false;

            lastMoveTime = millis();
        }

        return NO_MOVE;
    }
};
