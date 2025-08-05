#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>  // Added
#include <map>     // Added
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  SDL_Point food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  bool paused{false};

  // Added
  bool game_over{false};
  std::string name_input;
  std::map<std::string, int> high_scores;
  int global_high_score{0};
  std::string global_high_name;

  void PlaceFood();
  void Update();
  void SaveHighScore();  // Added
};

#endif