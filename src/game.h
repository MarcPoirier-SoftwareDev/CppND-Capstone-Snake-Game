#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>  // Added
#include <map>     // Added
#include "SDL.h"
#include "controller.h"
#include "snake.h"  // Moved up for Snake::Direction in MovingObstacle

// Added: Moved struct outside Game class for visibility in renderer.h
struct MovingObstacle {
  float x;
  float y;
  Snake::Direction dir;
  float speed{0.05f};  // Slower than snake's initial 0.1f
};

#include "renderer.h"  // Include after struct definition to avoid issues

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  Snake ai_snake;  // Added: AI-controlled snake
  SDL_Point food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  int ai_score{0};  // Added: Score for AI snake
  bool paused{false};

  // Added
  bool game_over{false};
  std::string name_input;
  std::map<std::string, int> high_scores;
  int global_high_score{0};
  std::string global_high_name;

  // Added: For obstacles (updated to use non-nested MovingObstacle)
  std::vector<SDL_Point> fixed_obstacles;
  std::vector<MovingObstacle> moving_obstacles;
  bool IsObstacle(int x, int y) const;
  bool IsBlocked(int x, int y) const;  // Added: For A* to check blocked cells

  // Added: Grid dimensions as members
  std::size_t grid_width_;
  std::size_t grid_height_;

  void PlaceFood();
  void Update();
  void SaveHighScore();  // Added
  Snake::Direction ComputeAIDirection();  // Added: A* for AI direction
};

#endif