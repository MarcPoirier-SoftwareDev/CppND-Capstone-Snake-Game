#include "game.h"
#include <iostream>
#include <fstream>  // Added
#include <string>   // Added (though included via header)
#include "SDL.h"
#include <cmath>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  // Added: Random distribution for directions
  std::uniform_int_distribution<int> random_dir(0, 3);

  // Added: Place fixed obstacles (5)
  for (int i = 0; i < 5; ++i) {
    int x, y;
    bool valid;
    do {
      x = random_w(engine);
      y = random_h(engine);
      valid = !snake.SnakeCell(x, y);
      for (const auto& ob : fixed_obstacles) {
        if (ob.x == x && ob.y == y) valid = false;
      }
    } while (!valid);
    fixed_obstacles.push_back({x, y});
  }

  // Added: Place moving obstacles (3)
  for (int i = 0; i < 3; ++i) {
    int x, y;
    bool valid;
    do {
      x = random_w(engine);
      y = random_h(engine);
      valid = !snake.SnakeCell(x, y) && !IsObstacle(x, y);
    } while (!valid);
    MovingObstacle mo;
    mo.x = static_cast<float>(x);
    mo.y = static_cast<float>(y);
    mo.dir = static_cast<Snake::Direction>(random_dir(engine));
    moving_obstacles.push_back(mo);
  }

  PlaceFood();

  // Added: Load high scores
  std::ifstream in("highscore.txt");
  std::string n;
  int s;
  while (in >> n >> s) {
    high_scores[n] = s;
    if (s > global_high_score) {
      global_high_score = s;
      global_high_name = n;
    }
  }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  bool text_input_active = false;  // Added

  while (running) {
    frame_start = SDL_GetTicks();

    controller.HandleInput(running, snake, paused, game_over, name_input);  // Modified: Passed game_over and name_input

    if (game_over) {
      if (!text_input_active) {
        SDL_StartTextInput();
        text_input_active = true;
      }
    } else if (!paused) {
      Update();
    }

    // Modified: Passed obstacles to renderer
    renderer.Render(snake, food, paused, game_over, score, name_input, global_high_score, global_high_name,
                    fixed_obstacles, moving_obstacles);

    frame_end = SDL_GetTicks();

    frame_count++;
    frame_duration = frame_end - frame_start;

    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }

  if (text_input_active) {
    SDL_StopTextInput();
  }

  // Added: Save if applicable
  if (game_over && !name_input.empty()) {
    SaveHighScore();
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Modified: Also check not on obstacle
    if (!snake.SnakeCell(x, y) && !IsObstacle(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) {
    game_over = true;  // Modified: Set game_over
    return;
  }

  // Added: Update moving obstacles first
  for (auto& mo : moving_obstacles) {
    switch (mo.dir) {
      case Snake::Direction::kUp:
        mo.y -= mo.speed;
        break;
      case Snake::Direction::kDown:
        mo.y += mo.speed;
        break;
      case Snake::Direction::kLeft:
        mo.x -= mo.speed;
        break;
      case Snake::Direction::kRight:
        mo.x += mo.speed;
        break;
    }
    // Wrap around like snake
    mo.x = std::fmod(mo.x + static_cast<float>(random_w.max() + 1), static_cast<float>(random_w.max() + 1));
    mo.y = std::fmod(mo.y + static_cast<float>(random_h.max() + 1), static_cast<float>(random_h.max() + 1));
  }

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Added: Check for obstacle collision
  if (IsObstacle(new_x, new_y)) {
    snake.alive = false;
    game_over = true;
    return;
  }

  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

// Added
void Game::SaveHighScore() {
  if (score > high_scores[name_input]) {
    high_scores[name_input] = score;
  }
  std::ofstream out("highscore.txt");
  for (const auto& p : high_scores) {
    out << p.first << " " << p.second << std::endl;
  }
}

// Added: Helper to check if a cell is an obstacle
bool Game::IsObstacle(int x, int y) const {
  for (const auto& ob : fixed_obstacles) {
    if (ob.x == x && ob.y == y) return true;
  }
  for (const auto& mo : moving_obstacles) {
    if (static_cast<int>(mo.x) == x && static_cast<int>(mo.y) == y) return true;
  }
  return false;
}