#include "EEPROM.h"

#include "Joystick.h"
#include "Matrix.h"
#include "Display.h"

Joystick joystick(A0, A1);
Matrix matrix(12, 11, 10, 13);
Display display(9, 8, 7, 6, 5, 4, 3);

//  Display states
enum GameState {
    INTRO_MESSAGE,
    MENU,
    IN_GAME,
    LOST_GAME,
    WON_GAME
};
GameState gameState = INTRO_MESSAGE;

const int introMessageDelay = 2000;

//  Menu states
enum MenuState {
    START_GAME,
    SETTINGS,
    ABOUT,
    MAIN_MENU
};
MenuState menuState = MAIN_MENU;
MenuState menuRow = START_GAME;
const byte menuSize = ABOUT - START_GAME + 1;

//  Settings states
enum SettingsState {
    LCD_BRIGHTNESS,
    MATRIX_BRIGHTNESS,
    BACK
};
SettingsState settingsState = LCD_BRIGHTNESS;
const byte settingsSize = BACK - LCD_BRIGHTNESS + 1;


//  About states
enum AboutState {
    GAME,
    AUTHOR,
    GITHUB
};
AboutState aboutState = GAME;
const byte aboutSize = GITHUB - GAME + 1;

//  Game lost states
enum GameLostState {
    QUIT,
    TRY_AGAIN
};
GameLostState gameLostState = TRY_AGAIN;

unsigned long gameStartTime = 0;

//  EEPROM addresses for all saved values
//  Every data address come exactly after the next

//  LCD brightness
const int maxLcdBrightness = 8;
const int lcdBrightnessAddress = 256;
byte lcdBrightness = 8;

//  Matrix brightness
const int maxMatrixBrightness = 8;
const int matrixBrightnessAddress = lcdBrightnessAddress + sizeof(lcdBrightness);
byte matrixBrightness = 2;

void setup() {
    Serial.begin(9600);

    joystick.setup();
    matrix.setup();
    display.setup();

    EEPROM.get(lcdBrightnessAddress, lcdBrightness);
    EEPROM.get(matrixBrightnessAddress, matrixBrightness);

    matrix.setBrightness(matrixBrightness);
    display.setBrightness(lcdBrightness);

    showIntroMessage();
}

void loop() {
    static int move;
    
    move = joystick.readValues();

    showMatrix(move);
    showDisplay(move);
}

////////////////////////////////////////////////////////////////  Display  ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MenuState previousMenuState(const MenuState state) {
    return (state - 1 + menuSize) % menuSize;
}

MenuState nextMenuState(const MenuState state) {
    return (state + 1) % menuSize;
}

SettingsState previousSettingsState(const SettingsState state) {
    return (state - 1 + settingsSize) % settingsSize;
}

SettingsState nextSettingsState(const SettingsState state) {
    return (state + 1) % settingsSize;
}

AboutState previousAboutState(const AboutState state) {
    return (state - 1 + aboutSize) % aboutSize;
}

AboutState nextAboutState(const AboutState state) {
    return (state + 1) % aboutSize;
}

//////////////////////////////////////////////////////////////  Show Display //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void showIntroMessage() {
    display.lcd.clear();

    display.lcd.setCursor(3, 0);
    display.lcd.print(F("Welcome to"));

    display.lcd.setCursor(3, 1);
    display.lcd.print(F("Maze Quest!"));
}

void showMenuRow(const byte menuRow, const byte LcdRow) {
    display.lcd.setCursor(3, LcdRow);

    switch (menuRow) {
        case 0: {
            display.lcd.print(F("Start Game"));

            break;
        }
        case 1: {
            display.lcd.print(F("Settings"));

            break;
        }
        case 2: {
            display.lcd.print(F("About"));

            break;
        }
    }
}

void showMainMenu() {
    display.lcd.clear();

    display.lcd.setCursor(1, 0);
    display.lcd.print(F(">"));

    display.lcd.setCursor(15, 0);
    display.lcd.write((byte)UP_ARROW);

    display.lcd.setCursor(15, 1);
    display.lcd.write((byte)DOWN_ARROW);

    showMenuRow(menuRow, 0);
    showMenuRow(nextMenuState(menuRow), 1);
}

void showSettingsRow(const byte settingsRow, const byte lcdRow) {
    switch (settingsRow) {
        case 0: {
            if (lcdRow == 0) {
                if (lcdBrightness > 1) {
                    display.lcd.setCursor(0, lcdRow);
                    display.lcd.print(F("<"));
                }

                if (lcdBrightness < maxLcdBrightness) {
                    display.lcd.setCursor(1, lcdRow);
                    display.lcd.print(F(">"));
                }
                
            }

            display.lcd.setCursor(3, lcdRow);
            display.lcd.print(F("LCD Light:"));
            display.lcd.print(lcdBrightness);

            break;
        }
        case 1: {
            if (lcdRow == 0) {
                if (matrixBrightness > 1) {
                    display.lcd.setCursor(0, lcdRow);
                    display.lcd.print(F("<"));
                }

                if (matrixBrightness < maxMatrixBrightness) {
                    display.lcd.setCursor(1, lcdRow);
                    display.lcd.print(F(">"));
                }
            }

            display.lcd.setCursor(3, lcdRow);
            display.lcd.print(F("MTX Light:"));
            display.lcd.print(matrixBrightness);

            break;
        }
        case 2: {
            if (lcdRow == 0) {
                display.lcd.setCursor(1, lcdRow);
                display.lcd.print(F(">"));
            }

            display.lcd.setCursor(3, lcdRow);
            display.lcd.print(F("Back"));

            break;
        }
    }
}

void showSettings() {
    display.lcd.clear();

    display.lcd.setCursor(15, 0);
    display.lcd.write((byte)UP_ARROW);

    display.lcd.setCursor(15, 1);
    display.lcd.write((byte)DOWN_ARROW);

    showSettingsRow(settingsState, 0);
    showSettingsRow(nextSettingsState(settingsState), 1);
}

void showAbout() {
    display.lcd.clear();

    display.lcd.setCursor(0, 0);
    display.lcd.print(F("<"));

    display.lcd.setCursor(15, 0);
    display.lcd.write((byte)UP_ARROW);

    display.lcd.setCursor(15, 1);
    display.lcd.write((byte)DOWN_ARROW);

    switch (aboutState) {
        case 0: {
            display.lcd.setCursor(3, 0);
            display.lcd.print(F("Game:"));
            
            display.lcd.setCursor(3, 1);
            display.lcd.print(F("Maze Quest"));

            break;
        }
        case 1: {
            display.lcd.setCursor(3, 0);
            display.lcd.print(F("Author:Iancu"));
            
            display.lcd.setCursor(3, 1);
            display.lcd.print(F("Ivasciuc"));

            break;
        }
        case 2: {
            display.lcd.setCursor(3, 0);
            display.lcd.print(F("Github:"));
            
            display.lcd.setCursor(3, 1);
            display.lcd.print(F("NFJJunior"));

            break;
        }
    }
}

void showInGame() {
    display.lcd.setCursor(0, 0);
    display.lcd.print(F("Level:1 "));
    display.lcd.print(F("Time:"));
    display.lcd.print((millis() - gameStartTime) / 1000);

    if (matrix.hasKey) {
        display.lcd.setCursor(2, 1);
        display.lcd.write((byte)KEY);
        display.lcd.print(F(":Not found"));
    }
}

void showLostGame() {
    display.lcd.clear();

    display.lcd.setCursor(0, 0);
    display.lcd.print(F("You exploded!"));

    display.lcd.setCursor(1, 1);
    display.lcd.print(F("Quit"));

    display.lcd.setCursor(7, 1);
    display.lcd.print(F("Try again"));

    if (gameLostState == QUIT) {
        display.lcd.setCursor(0, 1);
        display.lcd.print(F("<"));
    }
    else {
        display.lcd.setCursor(6, 1);
        display.lcd.print(F(">"));
    }
}

void showWonGame() {

}

/////////////////////////////////////////////////////////////  Display Logic //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void introMessage() {
    if (millis() >= introMessageDelay) {
        gameState = MENU;
        menuState = MAIN_MENU;
        menuRow = START_GAME;

        showMainMenu();
    }
}

void mainMenu(const int move) {
    switch (move) {
        case UP: {
            menuRow = previousMenuState(menuRow);

            showMainMenu();

            break;
        }
        case DOWN: {
            menuRow = nextMenuState(menuRow);

            showMainMenu();

            break;
        }
        case RIGHT: {
            menuState = menuRow;

            switch (menuState) {
                case START_GAME: {
                    gameState = IN_GAME;

                    gameStartTime = millis();
                    display.lcd.clear();
                    showInGame();

                    break;
                }
                case SETTINGS: {
                    settingsState = LCD_BRIGHTNESS;

                    showSettings();

                    break;
                }
                case ABOUT: {
                    aboutState = GAME;

                    showAbout();

                    break;
                }
            }

            break;
        }
    }
}

void settings(const int move) {
    switch (move) {
        case UP: {
            settingsState = previousSettingsState(settingsState);

            showSettings();

            break;
        }
        case DOWN: {
            settingsState = nextSettingsState(settingsState);

            showSettings();

            break;
        }
        case LEFT: {
            switch (settingsState) {
                case LCD_BRIGHTNESS: {
                    if (lcdBrightness > 1) {
                        EEPROM.get(lcdBrightnessAddress, lcdBrightness);
                        lcdBrightness --;
                        EEPROM.put(lcdBrightnessAddress, lcdBrightness);

                        display.setBrightness(lcdBrightness);
                    }

                    showSettings();

                    break;
                }
                case MATRIX_BRIGHTNESS: {
                    if (matrixBrightness > 1) {
                        EEPROM.get(matrixBrightnessAddress, matrixBrightness);
                        matrixBrightness --;
                        EEPROM.put(matrixBrightnessAddress, matrixBrightness);

                        matrix.setBrightness(matrixBrightness);
                    }

                    showSettings();

                    break;
                }
            }

            break;
        }
        case RIGHT: {
            switch (settingsState) {
                case LCD_BRIGHTNESS: {
                    if (lcdBrightness < maxLcdBrightness) {
                        EEPROM.get(lcdBrightnessAddress, lcdBrightness);
                        lcdBrightness ++;
                        EEPROM.put(lcdBrightnessAddress, lcdBrightness);

                        display.setBrightness(lcdBrightness);
                    }

                    showSettings();

                    break;
                }
                case MATRIX_BRIGHTNESS: {
                    if (matrixBrightness < maxLcdBrightness) {
                        EEPROM.get(matrixBrightnessAddress, matrixBrightness);
                        matrixBrightness ++;
                        EEPROM.put(matrixBrightnessAddress, matrixBrightness);

                        matrix.setBrightness(matrixBrightness);
                    }

                    showSettings();

                    break;
                }
                case BACK: {
                    menuState = MAIN_MENU;
                    menuRow = SETTINGS;

                    showMainMenu();

                    break;
                }
            }

            break;
        }
    }
}

void about(const int move) {
    switch (move) {
        case UP: {
            aboutState = previousAboutState(aboutState);

            showAbout();

            break;
        }
        case DOWN: {
            aboutState = nextAboutState(aboutState);

            showAbout();

            break;
        }
        case LEFT: {
            menuState = MAIN_MENU;
            menuRow = ABOUT;

            showMainMenu();

            break;
        }
    }
}

void menu(const int move) {
    switch (menuState) {
        case MAIN_MENU: {
            mainMenu(move);

            break;
        }
        case SETTINGS: {
            settings(move);

            break;
        }
        case ABOUT: {
            about(move);

            break;
        }
    }
}

void inGame() {
    if (matrix.gameLost) {
        gameState = LOST_GAME;
        gameLostState = TRY_AGAIN;

        showLostGame();
    }
    else if (matrix.gameWon) {
        gameState = WON_GAME;

        showWonGame();
    }
    else {
        showInGame();
    }
}

void lostGame(const int move) {
    switch (move) {
        case LEFT: {
            switch (gameLostState) {
                case QUIT: {
                    gameState = MENU;
                    menuState = MAIN_MENU;
                    menuRow = START_GAME;

                    matrix.resetGame();

                    showMainMenu();

                    break;
                }
                case TRY_AGAIN: {
                    gameLostState = QUIT;

                    showLostGame();

                    break;
                }
            }

            break;
        }
        case RIGHT: {
            switch (gameLostState) {
                case QUIT: {
                    gameLostState = TRY_AGAIN;

                    showLostGame();

                    break;
                }
                case TRY_AGAIN: {
                    gameState = IN_GAME;

                    matrix.resetGame();

                    gameStartTime = millis();
                    display.lcd.clear();
                    showInGame();

                    break;
                }
            }

            break;
        }
    }
}

void wonGame(const int move) {

}

void showDisplay(const int move) {
    switch (gameState) {
        case INTRO_MESSAGE: {
            introMessage();

            break;
        }
        case MENU: {
            menu(move);

            break;
        }
        case IN_GAME: {
            inGame();

            break;
        }
        case LOST_GAME: {
            lostGame(move);

            break;
        }
        case WON_GAME: {
            wonGame(move);

            break;
        }
    }
}

////////////////////////////////////////////////////////////////  Matrix  ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void showMatrix(const int move) {
    switch (gameState) {
        case INTRO_MESSAGE: {
            matrix.showImage(MAZE);

            break;
        }
        case MENU: {
            switch (menuState) {
                case MAIN_MENU: {
                    matrix.showImage(MAZE);

                    break;
                }
                case SETTINGS: {
                    matrix.showImage(WRENCH);

                    break;
                }
                case ABOUT: {
                    matrix.showImage(INFO);

                    break;
                }
            }

            break;
        }
        case IN_GAME: {
            matrix.movePlayer(move);
            matrix.showGame();

            break;
        }
        case LOST_GAME: {
            matrix.showGame();

            break;
        }
        case WON_GAME: {
            matrix.showGame();

            break;
        }
    }
}

