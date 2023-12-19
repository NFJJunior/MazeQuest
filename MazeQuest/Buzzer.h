#include "ezBuzzer.h"
#include "Notes.h"

const byte menuNoteLength = 1;
int menuNote[] = { NOTE_E4 };
int menuNoteDurations[] = { 10 };

const byte gameNoteLength = 1;
int gameNote[] = { NOTE_D5 };
int gameNoteDurations[] = { 10 };

const byte explosionLength = 14;
int explosionMelody[] = { NOTE_FS4, RESET, NOTE_A4, NOTE_D5, RESET, NOTE_A4, RESET, NOTE_FS4, NOTE_D4, NOTE_D4, NOTE_D4, RESET, RESET, RESET };
int explosionNoteDurations[] = { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 6 };

const byte winLength = 14;
int winMelody[] = { NOTE_E5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4, NOTE_D4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_D4 };
int winNoteDurations[] = { 6, 8, 8, 8, 8, 8, 2, 4, 4, 8, 8, 2, 4, 1 };

const byte pickKeyLength = 3;
int pickKeyMelody[] = {NOTE_E5, NOTE_G5, NOTE_A5};
int pickKeyNoteDurations[] = { 8, 8, 8 };