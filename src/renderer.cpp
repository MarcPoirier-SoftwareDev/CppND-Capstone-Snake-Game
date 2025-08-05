#include "renderer.h"
#include <iostream>
#include <string>

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

void Renderer::Render(Snake const snake, SDL_Point const &food, bool paused) {  // Modified: Added bool paused parameter
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Added: Render "PAUSED" text if paused
  if (paused && font != nullptr) {
    SDL_Color textColor = {255, 255, 255, 255};  // White text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "PAUSED", textColor);
    if (textSurface != nullptr) {
      SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
      if (textTexture != nullptr) {
        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_Rect textRect = {
            static_cast<int>((screen_width - textWidth) / 2),
            static_cast<int>((screen_height - textHeight) / 2),
            textWidth,
            textHeight
        };
        SDL_RenderCopy(sdl_renderer, textTexture, nullptr, &textRect);
        SDL_DestroyTexture(textTexture);
      }
      SDL_FreeSurface(textSurface);
    }
  }

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}