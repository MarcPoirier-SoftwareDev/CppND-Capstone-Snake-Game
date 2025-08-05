#include "renderer.h"
#include <iostream>
#include <string>
#include "SDL.h"
#include "SDL_ttf.h"
#include "game.h"  // Added: For MovingObstacle struct access

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Initialize SDL_ttf  // Added
  if (TTF_Init() < 0) {
    std::cerr << "SDL_ttf could not initialize.\n";
    std::cerr << "TTF_Error: " << TTF_GetError() << "\n";
  }

  // Load font (use system DejaVuSans.ttf; )
  font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 48);  // Modified: Use full system path; Font size 48 for visibility
  if (font == nullptr) {
    std::cerr << "Failed to load font.\n";
    std::cerr << "TTF_Error: " << TTF_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  TTF_CloseFont(font);  // Added: Clean up font
  TTF_Quit();  // Added: Quit SDL_ttf
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::Render(Snake const snake, Snake const &ai_snake, SDL_Point const &food, bool paused, bool game_over,
                      int score, int ai_score, const std::string &name_input, int global_high_score,
                      const std::string &global_high_name,
                      const std::vector<SDL_Point> &fixed_obstacles,
                      const std::vector<MovingObstacle> &moving_obstacles) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Modified: Render game elements (food, snake, obstacles) always, to show final state on game over

  // Render food (green)
  SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xFF, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render player's snake body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render player's snake head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Added: Render AI snake's body (gray)
  SDL_SetRenderDrawColor(sdl_renderer, 0xAA, 0xAA, 0xAA, 0xFF);
  for (SDL_Point const &point : ai_snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Added: Render AI snake's head (green if alive)
  block.x = static_cast<int>(ai_snake.head_x) * block.w;
  block.y = static_cast<int>(ai_snake.head_y) * block.h;
  if (ai_snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xCC, 0x7A, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Added: Render fixed obstacles (red)
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  for (const auto& ob : fixed_obstacles) {
    block.x = ob.x * block.w;
    block.y = ob.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Added: Render moving obstacles (yellow)
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF); 
  for (const auto& mo : moving_obstacles) {
    block.x = static_cast<int>(mo.x) * block.w;
    block.y = static_cast<int>(mo.y) * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render paused text if applicable
  if (paused && font != nullptr) {
    SDL_Color textColor = {255, 255, 255, 255};
    RenderText("PAUSED", screen_width / 2, screen_height / 2, textColor, true);
  }

  // Added: Render AI score in top-right
  SDL_Color textColor = {255, 255, 255, 255};
  std::string ai_score_text = "AI: " + std::to_string(ai_score);
  RenderText(ai_score_text, screen_width - 10, 10, textColor, false);

  if (game_over) {
    // Render Game Over texts (overlaid on the game state)
    RenderText("Game Over", screen_width / 2, screen_height / 4, textColor, true);

    std::string score_text = "Score: " + std::to_string(score);
    RenderText(score_text, screen_width / 2, screen_height / 3, textColor, true);

    std::string high_text = "High Score: " + global_high_name + " - " + std::to_string(global_high_score);
    RenderText(high_text, screen_width / 2, screen_height / 2.5, textColor, true);

    if (score > global_high_score) {
      RenderText("New High Score!", screen_width / 2, screen_height / 2, textColor, true);
    }

    RenderText("Enter your name:", screen_width / 2, screen_height / 1.5, textColor, true);

    // Add blinking cursor
    std::string display_name = name_input;
    if ((SDL_GetTicks() / 500) % 2 == 0) {
      display_name += "_";
    }
    RenderText(display_name, screen_width / 2, screen_height / 1.4, textColor, true);

    // Added: Show AI score on game over
    std::string ai_final_text = "AI Score: " + std::to_string(ai_score);
    RenderText(ai_final_text, screen_width / 2, screen_height / 2.2, textColor, true);
  }

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}

// Added: Helper function
void Renderer::RenderText(const std::string &text, int x, int y, SDL_Color color, bool center) {
  if (text.empty() || font == nullptr) return;

  SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
  if (textSurface == nullptr) return;

  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
  if (textTexture == nullptr) {
    SDL_FreeSurface(textSurface);
    return;
  }

  int textWidth = textSurface->w;
  int textHeight = textSurface->h;
  SDL_Rect textRect = {x, y, textWidth, textHeight};
  if (center) {
    textRect.x -= textWidth / 2;
  }

  SDL_RenderCopy(sdl_renderer, textTexture, nullptr, &textRect);
  SDL_DestroyTexture(textTexture);
  SDL_FreeSurface(textSurface);
}