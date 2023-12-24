# MazeQuest
Matrix game for IntroductionToRobotics course, Faculty of Mathematics and Computer Science, University of Bucharest.

## Backstory
Last year, a friend of mine took the IntrodoctionToRobotics course. For his final project, he made a maze with invisible bombs.
I enjoyed the game creation process and offered to design a map too. The final level of this game is a modified version of the map that I designed last year.

For my final project, I decided to enhanced my friend's game, adding new features.

## Description
The game is a maze with invisible bombs. Your goal is to move from top-left to botton-right without exploding any bomb.
Whenever you are near a bomb the red LED is on.

The player is moved with the joystick. The maze map is bigger than the LED matrix, so in order to navigate it, the visible
area is locked with the player in the center.

The game is design to logically deduce the path you need to take without guessing.

There are 2 levels that contain additional objects. In these levels, you firstly need to find a key to open a door in order to progress till the end.
The yellow light signals that you are near the key.

## Instructions
The player starts from the top-left corner. When the red LED is on, a bomb is adjacent to the player position in one of the 4 possible directions: UP, DOWN, LEFT or RIGHT.

Two bombs cannot be adjacent to each other. Two bombs cannot light up the same place. Because of these rules, the following bombs placements are illegal:

-X &nbsp; &nbsp; &nbsp; &nbsp; -XX- &nbsp; &nbsp; &nbsp; &nbsp; -X-X- <br/> X-

Using the illegal placements, the player can logically navigate the maze and emerge victorious.

The yellow LED is on when you are adjacent to the key.

The player blinks rapidly, while the door blinks slowly.

There are six levels in total.

## Components
* Arduino UNO board
* 2 medium breadboards
* 8x8 LED matrix
* MAX7219 (matrix driver)
* LCD display
* joystick
* buzzer
* red and yellow LEDs
* potentiometer (for LCD contrast)
* 10 uF 50V capacitor
* 104 ceramic capacitor
* resistors and wires

## [Demo Video](https://www.youtube.com/watch?v=pZ_hT0BBJQs)

## Setup
![Setup](https://github.com/NFJJunior/MazeQuest/blob/main/setup.jpeg)

