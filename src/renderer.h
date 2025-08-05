#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>  // Added
#include "SDL.h"
#include "SDL_ttf.h"
#include "snake.h"

struct MovingObstacle;

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  // Modified: Added obstacle params (updated to use non-nested MovingObstacle)
  void Render(Snake const snake, SDL_Point const &food, bool paused, bool game_over,
              int score, const std::string &name_input, int global_high_score,
              const std::string &global_high_name,
              const std::vector<SDL_Point> &fixed_obstacles,
              const std::vector<MovingObstacle> &moving_obstacles);
  void UpdateWindowTitle(int score, int fps);

 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  TTF_Font *font;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;

  // Added: Helper for text rendering
  void RenderText(const std::string &text, int x, int y, SDL_Color color, bool center);
};

#endif