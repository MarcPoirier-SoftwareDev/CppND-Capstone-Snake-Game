#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include <string>

class Controller {
 public:
  void HandleInput(bool &running, Snake &snake, bool &paused, bool game_over, std::string &name_input) const;  // Modified: Added bool &paused parameter game_over and name_input

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif