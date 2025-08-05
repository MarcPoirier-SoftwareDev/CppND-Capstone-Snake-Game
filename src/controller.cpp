#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

void Controller::HandleInput(bool &running, Snake &snake, bool &paused, bool game_over, std::string &name_input) const {  // Modified: Added game_over and name_input
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (game_over) {
      // Handle text input for name
      if (e.type == SDL_TEXTINPUT) {
        name_input += e.text.text;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_BACKSPACE:
            if (!name_input.empty()) {
              name_input.pop_back();
            }
            break;
          case SDLK_RETURN:
            running = false;  // Exit loop to save
            break;
          case SDLK_q:
            running = false;  // Quit without saving
            break;
        }
      }
    } else {
      // Normal game input
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_UP:
            ChangeDirection(snake, Snake::Direction::kUp,
                            Snake::Direction::kDown);
            break;

          case SDLK_DOWN:
            ChangeDirection(snake, Snake::Direction::kDown,
                            Snake::Direction::kUp);
            break;

          case SDLK_LEFT:
            ChangeDirection(snake, Snake::Direction::kLeft,
                            Snake::Direction::kRight);
            break;

          case SDLK_RIGHT:
            ChangeDirection(snake, Snake::Direction::kRight,
                            Snake::Direction::kLeft);
            break;

          case SDLK_ESCAPE:
            paused = !paused;
            break;

          case SDLK_q:
            running = false;
            break;
        }
      }
    }
  }
}