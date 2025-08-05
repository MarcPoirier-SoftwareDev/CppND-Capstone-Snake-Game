#include "game.h"
#include <iostream>
#include <fstream>  // Added
#include <string>   // Added (though included via header)
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
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

    renderer.Render(snake, food, paused, game_over, score, name_input, global_high_score, global_high_name);  // Modified: Added extra params

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
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
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

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

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