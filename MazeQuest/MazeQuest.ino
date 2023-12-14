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
    WON_GAME,
    NEXT_GAME
};
GameState gameState = INTRO_MESSAGE;

const int introMessageDelay = 2000;

//  Menu states
enum MenuState {
    START_GAME,
    SELECT_LEVEL,
    HIGHSCORES,
    HOW_TO_PLAY,
    SETTINGS,
    ABOUT,
    MAIN_MENU
};
MenuState menuState = MAIN_MENU;
MenuState menuRow = START_GAME;
const byte menuSize = ABOUT - START_GAME + 1;

byte highscoreLevel = 1;

//  Settings states
enum SettingsState {
    LCD_BRIGHTNESS,
    MATRIX_BRIGHTNESS,
    RESET_HIGHSCORES,
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
enum LostGameState {
    QUIT_LOST,
    TRY_AGAIN
};
LostGameState lostGameState = TRY_AGAIN;

enum WonGameState {
    NAME,
    NEXT
};
WonGameState wonGameState = NEXT;

enum NextGameState {
    QUIT_NEXT,
    PLAY
};
NextGameState nextGameState = PLAY;

unsigned long gameStartTime = 0;

//  EEPROM addresses for all saved values

//  Player name
const byte nameSize = 4;

const int nameAddress = 0;
char name[nameSize] = "IAN";

byte namePosition = 0;
const byte nrLetters = 26;
const byte ordA = 65;

//  Level
const byte nrLevels = 2;

const int levelAddress = nameAddress + sizeof(name);
byte level;

//  Maps
struct Map {
    byte mapSize;
    byte matrixMap[matrix.maxMatrixSize][matrix.maxMatrixSize / 8 + (matrix.maxMatrixSize % 8 != 0)];
    byte nrBombs;
    Position bombs[matrix.maxNrBombs];
    Position keyPosition;

};

const int mapsAddress = levelAddress + sizeof(level);
Map levelMap;

//  Highscores
struct Highscore {
    char names[3][nameSize];
    int scores[3];
};

const int highscoresAddress = mapsAddress + sizeof(Map) * nrLevels;
Highscore highscore;

const int nextAddress = highscoresAddress + sizeof(Highscore) * nrLevels;

//  LCD brightness
const int maxLcdBrightness = 8;

const int lcdBrightnessAddress = highscoresAddress + sizeof(Highscore) * nrLevels;
byte lcdBrightness;

//  Matrix brightness
const int maxMatrixBrightness = 8;

const int matrixBrightnessAddress = lcdBrightnessAddress + sizeof(lcdBrightness);
byte matrixBrightness;


void setup() {
    Serial.begin(9600);

    joystick.setup();
    matrix.setup();
    display.setup();

    EEPROM.get(nameAddress, name);
    EEPROM.get(levelAddress, level);

    matrixInit();

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

void matrixInit() {
    EEPROM.get(mapsAddress + sizeof(Map) * (level - 1), levelMap);

    matrix.matrixSize = levelMap.mapSize;
    for (int i = 0; i < matrix.maxMatrixSize; i++) {
        for (int j = 0; j < matrix.maxMatrixSize; j++) {
            matrix.matrixMap[i][j] = bitRead(levelMap.matrixMap[i][j / 8], 7 - (j % 8));
        }
    }

    matrix.nrBombs = levelMap.nrBombs;
    for (int i = 0; i < matrix.nrBombs; i++) {
        matrix.matrixMap[levelMap.bombs[i].x][levelMap.bombs[i].y] = 2;

        for (int k = 0; k < 3; k++) {
            matrix.matrixMap[levelMap.bombs[i].x + matrix.directions[k][0]][levelMap.bombs[i].y + matrix.directions[k][1]] = 3;
        }
    }

    matrix.resetGame();
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
    display.lcd.print(F("Welcome To"));

    display.lcd.setCursor(3, 1);
    display.lcd.print(F("Maze Quest!"));
}

void showMenuRow(const MenuState menuRow, const byte LcdRow) {
    display.lcd.setCursor(3, LcdRow);

    switch (menuRow) {
        case START_GAME: {
            display.lcd.print(F("Start Game"));

            break;
        }
        case SELECT_LEVEL: {
            display.lcd.print(F("Level: "));
            display.lcd.print(level);

            break;
        }
        case HIGHSCORES: {
            display.lcd.print(F("Highscores"));

            break;
        }
        case HOW_TO_PLAY: {
            display.lcd.print(F("How To Play"));

            break;
        }
        case SETTINGS: {
            display.lcd.print(F("Settings"));

            break;
        }
        case ABOUT: {
            display.lcd.print(F("About"));

            break;
        }
    }
}

void showMainMenu() {
    display.lcd.clear();

    if (menuRow == SELECT_LEVEL) {
        if (level > 1) {
            display.lcd.setCursor(0, 0);
            display.lcd.print(F("<"));
        }

        if (level < nrLevels) {
            display.lcd.setCursor(1, 0);
            display.lcd.print(F(">"));
        }
    }
    else {
        display.lcd.setCursor(1, 0);
        display.lcd.print(F(">"));
    }

    display.lcd.setCursor(15, 0);
    display.lcd.write((byte)UP_ARROW);

    display.lcd.setCursor(15, 1);
    display.lcd.write((byte)DOWN_ARROW);

    showMenuRow(menuRow, 0);
    showMenuRow(nextMenuState(menuRow), 1);
}

void showHighscores() {
    display.lcd.clear();

    display.lcd.setCursor(0, 0);
    display.lcd.print(F("< Lvl:"));
    display.lcd.print(highscoreLevel);

    display.lcd.setCursor(8, 0);
    display.lcd.print(highscore.names[0]);
    display.lcd.print(F(":"));
    display.lcd.print(highscore.scores[0]);

    display.lcd.setCursor(15, 0);
    display.lcd.write((byte)UP_ARROW);

    display.lcd.setCursor(0, 1);
    display.lcd.print(highscore.names[1]);
    display.lcd.print(F(":"));
    display.lcd.print(highscore.scores[1]);

    display.lcd.setCursor(8, 1);
    display.lcd.print(highscore.names[2]);
    display.lcd.print(F(":"));
    display.lcd.print(highscore.scores[2]);

    display.lcd.setCursor(15, 1);
    display.lcd.write((byte)DOWN_ARROW);
}

void showHowToPlay() {

}

void showSettingsRow(const SettingsState settingsRow, const byte lcdRow) {
    switch (settingsRow) {
        case LCD_BRIGHTNESS: {
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
        case MATRIX_BRIGHTNESS: {
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
        case RESET_HIGHSCORES: {
            if (lcdRow == 0) {
                display.lcd.setCursor(1, lcdRow);
                display.lcd.print(F(">"));
            }

            display.lcd.setCursor(3, lcdRow);
            display.lcd.print(F("Reset HS"));

            break;
        }
        case BACK: {
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
    display.lcd.print(F("Level:"));
    display.lcd.print(level);
    display.lcd.print(F(" Time:"));
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

    if (lostGameState == QUIT_LOST) {
        display.lcd.setCursor(0, 1);
        display.lcd.print(F("<"));
    }
    else {
        display.lcd.setCursor(6, 1);
        display.lcd.print(F(">"));
    }
}

void showWonGame() {
    display.lcd.clear();

    display.lcd.setCursor(0, 0);
    display.lcd.print(F("You won level "));
    display.lcd.print(level);
    display.lcd.print(F("!"));
    
    display.lcd.setCursor(0, 1);
    display.lcd.print(F("Name:"));
    display.lcd.print(name);

    display.lcd.setCursor(11, 1);
    display.lcd.print(F("Next"));

    if (wonGameState == NAME) {
        display.lcd.setCursor(5 + namePosition, 1);
        display.lcd.cursor();
    }
    else {
        display.lcd.setCursor(10, 1);
        display.lcd.print(F(">"));
        display.lcd.noCursor();
    }
}

void showNextGame() {
    display.lcd.clear();

    display.lcd.setCursor(4, 0);
    display.lcd.print(F("Level: "));
    display.lcd.print(level + 1);

    display.lcd.setCursor(2, 1);
    display.lcd.print("Quit");

    display.lcd.setCursor(10, 1);
    display.lcd.print("Play");

    if (nextGameState == QUIT_NEXT) {
        display.lcd.setCursor(1, 1);
        display.lcd.print("<");
    }
    else {
        display.lcd.setCursor(9, 1);
        display.lcd.print(">");
    }
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
        case LEFT: {
            if (menuRow == SELECT_LEVEL && level > 1) {
                level --;
                EEPROM.put(levelAddress, level);
                matrixInit();
            }

            showMainMenu();

            break;
        }
        case RIGHT: {
            switch (menuRow) {
                case START_GAME: {
                    gameState = IN_GAME;

                    gameStartTime = millis();
                    display.lcd.clear();
                    showInGame();

                    break;
                }
                case SELECT_LEVEL: {
                    if (level < nrLevels) {
                        level ++;
                        EEPROM.put(levelAddress, level);
                        matrixInit();
                    }

                    showMainMenu();

                    break;
                }
                case HIGHSCORES: {
                    menuState = HIGHSCORES;
                    highscoreLevel = 1;
                    EEPROM.get(highscoresAddress + sizeof(Highscore) * (highscoreLevel - 1), highscore);

                    showHighscores();

                    break;
                }
                case HOW_TO_PLAY: {
                    break;
                }
                case SETTINGS: {
                    menuState = SETTINGS;
                    settingsState = LCD_BRIGHTNESS;

                    showSettings();

                    break;
                }
                case ABOUT: {
                    menuState = ABOUT;
                    aboutState = GAME;

                    showAbout();

                    break;
                }
            }

            break;
        }
    }
}

void highscores(const int move) {
    switch (move) {
        case UP: {
            highscoreLevel = (highscoreLevel - 2 + nrLevels) % nrLevels + 1;
            EEPROM.get(highscoresAddress + sizeof(Highscore) * (highscoreLevel - 1), highscore);

            showHighscores();

            break;
        }
        case DOWN: {
            highscoreLevel = highscoreLevel % nrLevels + 1;
            EEPROM.get(highscoresAddress + sizeof(Highscore) * (highscoreLevel - 1), highscore);

            showHighscores();

            break;
        }
        case LEFT: {
            menuState = MAIN_MENU;
            menuRow = HIGHSCORES;

            showMainMenu();

            break;
        }
    }
}

void howToPlay(const int move) {

}

void resetHighscores() {
    for (int i = 0; i < 3; i++) {
        strcpy(highscore.names[i], "AAA");
        highscore.scores[i] = 999;
    }

    for (int i = 0; i < nrLevels; i++) {
        EEPROM.put(highscoresAddress + sizeof(Highscore) * i, highscore);
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
                        lcdBrightness --;
                        display.setBrightness(lcdBrightness);
                    }

                    showSettings();

                    break;
                }
                case MATRIX_BRIGHTNESS: {
                    if (matrixBrightness > 1) {
                        matrixBrightness --;
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
                        lcdBrightness ++;
                        EEPROM.put(lcdBrightnessAddress, lcdBrightness);
                        display.setBrightness(lcdBrightness);
                    }

                    showSettings();

                    break;
                }
                case MATRIX_BRIGHTNESS: {
                    if (matrixBrightness < maxLcdBrightness) {
                        matrixBrightness ++;
                        EEPROM.put(matrixBrightnessAddress, matrixBrightness);
                        matrix.setBrightness(matrixBrightness);
                    }

                    showSettings();

                    break;
                }
                case RESET_HIGHSCORES: {
                    resetHighscores();

                    showSettings();

                    break;
                }
                case BACK: {
                    menuState = MAIN_MENU;
                    menuRow = SETTINGS;

                    EEPROM.put(lcdBrightnessAddress, lcdBrightness);
                    EEPROM.put(matrixBrightnessAddress, matrixBrightness);

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
        case HIGHSCORES: {
            highscores(move);

            break;
        }
        case HOW_TO_PLAY: {
            howToPlay(move);

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
        lostGameState = TRY_AGAIN;

        showLostGame();
    }
    else if (matrix.gameWon) {
        gameState = WON_GAME;
        wonGameState = NEXT;

        showWonGame();
    }
    else {
        showInGame();
    }
}

void lostGame(const int move) {
    switch (move) {
        case LEFT: {
            if (lostGameState == QUIT_LOST) {
                gameState = MENU;
                menuState = MAIN_MENU;
                menuRow = START_GAME;

                matrix.resetGame();

                showMainMenu();
            }
            else {
                lostGameState = QUIT_LOST;

                showLostGame();
            }

            break;
        }
        case RIGHT: {
            if (lostGameState == QUIT_LOST) {
                lostGameState = TRY_AGAIN;

                showLostGame();
            }
            else {
                gameState = IN_GAME;

                matrix.resetGame();
                gameStartTime = millis();

                display.lcd.clear();
                showInGame();
            }

            break;
        }
    }
}

void wonGame(const int move) {
    switch (move) {
        case UP: {
            if (wonGameState == NAME) {
                name[namePosition] = (name[namePosition] - ordA - 1 + nrLetters) % nrLetters + ordA;

                showWonGame();
            }

            break;
        }
        case DOWN: {
            if (wonGameState == NAME) {
                name[namePosition] = (name[namePosition] - ordA + 1) % nrLetters + ordA;

                showWonGame();
            }

            break;
        }
        case LEFT: {
            if (wonGameState == NAME) {
                namePosition = max(0, namePosition - 1);
            }
            else {
                wonGameState = NAME;
                namePosition = 2;
            }

            showWonGame();

            break;
        }
        case RIGHT: {
            if (wonGameState == NAME) {
                if (namePosition == 2) {
                    wonGameState = NEXT;
                }
                else {
                    namePosition ++;
                }

                showWonGame();
            }
            else {
                gameState = NEXT_GAME;
                nextGameState = PLAY;

                EEPROM.put(nameAddress, name);

                showNextGame();
            }

            break;
        }
    }
}

void nextGame(const int move) {
    switch(move) {
        case LEFT: {
            if (nextGameState == QUIT_NEXT) {
                gameState = MENU;
                menuState = MAIN_MENU;
                menuRow = START_GAME;

                matrix.resetGame();

                showMainMenu();
            }
            else {
                nextGameState = QUIT_NEXT;

                showNextGame();
            }

            break;
        }
        case RIGHT: {
            if (nextGameState == QUIT_NEXT) {
                nextGameState = PLAY;

                showNextGame();
            }
            else {
                gameState = IN_GAME;

                if (level < nrLevels) {
                    level ++;
                    EEPROM.put(levelAddress, level);
                    matrixInit();
                }

                showInGame();
            }

            break;
        }
    }
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
        case NEXT_GAME: {
            nextGame(move);

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
                case HIGHSCORES: {
                    matrix.showImage(TROPHY);

                    break;
                }
                case HOW_TO_PLAY: {
                    matrix.showImage(H);

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

