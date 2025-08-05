#include "game.h"
#include <iostream>
#include <fstream>  // Added
#include <string>   // Added (though included via header)
#include "SDL.h"
#include <cmath>
#include <queue>    // Added: For priority_queue in A*
#include <tuple>    // Added: For priority_queue tuples
#include <limits>   // Added: For numeric_limits
#include <vector>
#include <algorithm>  // For reverse

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      ai_snake(grid_width, grid_height),  // Added
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      grid_width_(grid_width),  // Added
      grid_height_(grid_height) {  // Added
  // Added: Random distribution for directions
  std::uniform_int_distribution<int> random_dir(0, 3);

  // Added: Initialize AI snake position and direction
  ai_snake.head_x = grid_width / 4.0f;
  ai_snake.head_y = grid_height / 2.0f;
  ai_snake.direction = Snake::Direction::kRight;

  // Added: Place fixed obstacles (5)
  for (int i = 0; i < 5; ++i) {
    int x, y;
    bool valid;
    do {
      x = random_w(engine);
      y = random_h(engine);
      valid = !snake.SnakeCell(x, y) && !ai_snake.SnakeCell(x, y);  // Modified: Avoid AI snake
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
      valid = !snake.SnakeCell(x, y) && !ai_snake.SnakeCell(x, y) && !IsObstacle(x, y);  // Modified: Avoid AI snake
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

    // Modified: Passed obstacles and AI snake to renderer
    renderer.Render(snake, ai_snake, food, paused, game_over, score, ai_score, name_input, global_high_score, global_high_name,
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
    // Modified: Also check not on AI snake or obstacle
    if (!snake.SnakeCell(x, y) && !ai_snake.SnakeCell(x, y) && !IsObstacle(x, y)) {
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

  // Added: Compute AI direction using A*
  if (ai_snake.alive) {
    ai_snake.direction = ComputeAIDirection();
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
  if (ai_snake.alive) {
    ai_snake.Update();
  }

  int player_x = static_cast<int>(snake.head_x);
  int player_y = static_cast<int>(snake.head_y);
  int ai_x = static_cast<int>(ai_snake.head_x);
  int ai_y = static_cast<int>(ai_snake.head_y);

  // Added: Check for obstacle collision for both
  if (IsObstacle(player_x, player_y)) {
    snake.alive = false;
    game_over = true;
    return;
  }
  if (ai_snake.alive && IsObstacle(ai_x, ai_y)) {
    ai_snake.alive = false;
  }

  // Added: Check inter-snake collisions
  if (snake.alive && ai_snake.alive) {
    if (player_x == ai_x && player_y == ai_y) {
      // Head-to-head collision
      snake.alive = false;
      ai_snake.alive = false;
      game_over = true;
      return;
    }
    if (ai_snake.SnakeCell(player_x, player_y)) {
      // Player head hits AI body
      snake.alive = false;
      game_over = true;
      return;
    }
    if (snake.SnakeCell(ai_x, ai_y)) {
      // AI head hits player body
      ai_snake.alive = false;
    }
  }

  // Check food for player
  if (snake.alive && food.x == player_x && food.y == player_y) {
    score++;
    PlaceFood();
    snake.GrowBody();
    snake.speed += 0.02;
  }

  // Check food for AI
  if (ai_snake.alive && food.x == ai_x && food.y == ai_y) {
    ai_score++;
    PlaceFood();
    ai_snake.GrowBody();
    ai_snake.speed += 0.02;
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

// Added: Helper for A* to check blocked cells (obstacles + both snakes)
bool Game::IsBlocked(int x, int y) const {
  if (IsObstacle(x, y)) return true;
  if (snake.SnakeCell(x, y)) return true;
  if (ai_snake.SnakeCell(x, y)) return true;
  return false;
}

// Added: Compute direction for AI using A* pathfinding
Snake::Direction Game::ComputeAIDirection() {
  int start_x = static_cast<int>(ai_snake.head_x);
  int start_y = static_cast<int>(ai_snake.head_y);
  int goal_x = food.x;
  int goal_y = food.y;

  if (start_x == goal_x && start_y == goal_y) {
    return ai_snake.direction;
  }

  const float INF = std::numeric_limits<float>::infinity();
  std::vector<std::vector<float>> g_score(grid_height_, std::vector<float>(grid_width_, INF));  // Modified: Use grid_height_ and grid_width_
  std::vector<std::vector<float>> f_score(grid_height_, std::vector<float>(grid_width_, INF));  // Modified: Use grid_height_ and grid_width_
  std::vector<std::vector<SDL_Point>> came_from(grid_height_, std::vector<SDL_Point>(grid_width_, {-1, -1}));  // Modified: Use grid_height_ and grid_width_

  auto compare = [](const std::tuple<float, int, int>& a, const std::tuple<float, int, int>& b) {
    return std::get<0>(a) > std::get<0>(b);
  };
  std::priority_queue<std::tuple<float, int, int>, std::vector<std::tuple<float, int, int>>, decltype(compare)> open(compare);

  g_score[start_y][start_x] = 0.0f;
  f_score[start_y][start_x] = std::abs(start_x - goal_x) + std::abs(start_y - goal_y);
  open.push({f_score[start_y][start_x], start_x, start_y});

  std::vector<std::pair<int, int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};  // Up, down, left, right

  while (!open.empty()) {
    auto [cost, cx, cy] = open.top();
    open.pop();

    if (cx == goal_x && cy == goal_y) {
      // Reconstruct path
      std::vector<SDL_Point> path;
      SDL_Point current = {cx, cy};
      while (current.x != start_x || current.y != start_y) {
        path.push_back(current);
        current = came_from[current.y][current.x];
      }
      path.push_back({start_x, start_y});
      std::reverse(path.begin(), path.end());

      if (path.size() < 2) return ai_snake.direction;
      SDL_Point next = path[1];

      if (next.x > start_x) return Snake::Direction::kRight;
      if (next.x < start_x) return Snake::Direction::kLeft;
      if (next.y > start_y) return Snake::Direction::kDown;
      if (next.y < start_y) return Snake::Direction::kUp;

      return ai_snake.direction;
    }

    for (auto [dx, dy] : dirs) {
      int nx = cx + dx;
      int ny = cy + dy;
      if (nx < 0 || nx >= static_cast<int>(grid_width_) || ny < 0 || ny >= static_cast<int>(grid_height_)) continue;  // Modified: Use grid_width_ and grid_height_
      if (IsBlocked(nx, ny)) continue;

      float tentative_g = g_score[cy][cx] + 1.0f;
      if (tentative_g < g_score[ny][nx]) {
        came_from[ny][nx] = {cx, cy};
        g_score[ny][nx] = tentative_g;
        f_score[ny][nx] = tentative_g + std::abs(nx - goal_x) + std::abs(ny - goal_y);
        open.push({f_score[ny][nx], nx, ny});
      }
    }
  }

  // No path found, keep current direction
  return ai_snake.direction;
}