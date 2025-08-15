# CPPND: Capstone Snake Game Example

This is a starter repo for the Capstone project in the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). The code for this repo was inspired by [this](https://codereview.stackexchange.com/questions/212296/snake-game-in-c-with-sdl) excellent StackOverflow post and set of responses.

<img src="snake_game.gif"/>

The Capstone Project gives you a chance to integrate what you've learned throughout this program. This project will become an important part of your portfolio to share with current and future colleagues and employers.

In this project, you can build your own C++ application or extend this Snake game, following the principles you have learned throughout this Nanodegree Program. This project will demonstrate that you can independently create applications using a wide range of C++ features.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* SDL2 >= 2.0
  * All installation instructions can be found [here](https://wiki.libsdl.org/Installation)
  >Note that for Linux, an `apt` or `apt-get` installation is preferred to building from source.
* SDL2-TTF >= 2.0
  * Linux: `sudo apt install libsdl2-ttf-dev`
  * MacOS: `brew install sdl2_ttf`
  * Windows: libraries for MinGW included in the `lib` folder
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./SnakeGame`.


## New Features Added

The following new features have been added to the base Snake game, along with their expected behavior:

* **Pause Screen:** Press the ESC key to pause/unpause the game. When paused, the game stops updating, and a "PAUSED" message is displayed in the center of the screen. The game resumes from the same state when unpaused.

* **Start Screen Pause:** The game starts in a paused state with a "Press Enter to Start" message displayed. Press Enter to begin the game. Press 'q' to quit without starting.

* **Player Name and High Score Saving:** Upon game over, the player can enter their name using keyboard input. The name and score are saved to a `highscore.txt` file if it's a new high for that name. The global high score and name are displayed on the game over screen. Expected output: High scores are loaded at startup and saved at end; text input shows with a blinking cursor.

* **Fixed and Moving Obstacles:** 5 fixed obstacles (red blocks) and 3 moving obstacles (yellow blocks) are randomly placed at the start. Fixed ones stay static; moving ones move in random directions at a slower speed (0.05f) and wrap around the screen. Snakes die upon collision with any obstacle. Food and snakes avoid spawning on obstacles.

* **AI-Controlled Snake:** A second snake (gray body, green head) is controlled by the computer using the A* search algorithm to pathfind to the food. It avoids obstacles, the player snake, and its own body. The AI snake has its own score displayed in the top-right. Both snakes can eat the food and grow independently. Collisions between snakes (head-to-body or head-to-head) kill the respective snake(s).

## Rubric Points

### Loops, Functions, I/O (Meets 4 criteria)

* **The project demonstrates an understanding of C++ functions and control structures:** Variety of control structures (loops, if/else, switch) used. Examples: `src/game.cpp` lines 170-200 (for loops in Update for obstacle updates), `src/controller.cpp` lines 20-70 (switch in HandleInput), `src/renderer.cpp` lines 100-150 (if conditions in Render).

* **The project code is clearly organized into functions:** Code is modularized into functions like Update, Render, HandleInput. Examples: `src/game.cpp` (Update at lines 150-220), `src/snake.cpp` (UpdateHead at lines 20-40).

* **The project reads data from a file and processes the data, or writes data to a file:** High scores read from `highscore.txt` and processed in Game constructor; written in SaveHighScore. `src/game.cpp` lines 50-60 (read in constructor), lines 230-240 (SaveHighScore).

* **The project accepts user input and processes the input:** Keyboard input for directions, pause (ESC), start (Enter), name entry. `src/controller.cpp` lines 10-80 (HandleInput processes SDL events).


### Object Oriented Programming (Meets 3 criteria)

* **One or more classes are added to the project with appropriate access specifiers for class members:** Classes like Game, Snake, Renderer, Controller with public/private specifiers. `src/game.h` lines 10-50 (public/private in Game), `src/snake.h` lines 10-30 (public/private in Snake).

* **Class constructors utilize member initialization lists:** Game constructor uses init lists for snake, engine, etc. `src/game.cpp` lines 10-20.

* **Classes abstract implementation details from their interfaces:** Methods like Update() hide details (e.g., head/body updates in Snake). `src/snake.cpp` lines 10-50 (Update abstracts UpdateHead and UpdateBody), `src/game.cpp` lines 150-220 (Update abstracts snake and obstacle logic).


### Memory Management (Meets 3 criteria)

* **The project makes use of references in function declarations:** Pass-by-reference used extensively. Examples: `src/game.h` line 15 (Run takes Controller const &), `src/controller.h` line 10 (HandleInput takes Snake &).

* **The project uses destructors appropriately:** Renderer destructor cleans up SDL resources. `src/renderer.cpp` lines 60-70 (~Renderer).

* **The project follows scope / Resource Acquisition Is Initialization (RAII) where appropriate:** SDL resources acquired in Renderer constructor and released in destructor; scope ensures automatic destruction. `src/renderer.cpp` lines 20-50 (constructor initializes SDL), lines 60-70 (destructor destroys).




### Concurrency (Meets 3 criteria)

* **The project uses multithreading:** std::async used for AI pathfinding in separate thread. `src/game.cpp` lines 260-300 (async in ComputeAIDirection).

* **A promise and future is used in the project:** std::future from async to get AI direction. `src/game.cpp` lines 260-270 (async launch), line 350 (future.get()).

* **A mutex or lock is used in the project:** std::mutex and lock_guard protect shared data (snakes, obstacles). `src/game.cpp` lines 140-150 (lock in Update), lines 280-290 (lock in async lambda).



## CC Attribution-ShareAlike 4.0 International


Shield: [![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

This work is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
