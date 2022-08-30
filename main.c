#include <assert.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "snake.h"

typedef struct {
  unsigned int fps;
  Snake *snake;
} Game;

typedef enum {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
} Button;

static Direction buttonToDirection(Button button)
{
  switch (button) {
    case BUTTON_UP:    return DIR_UP;
    case BUTTON_DOWN:  return DIR_DOWN;
    case BUTTON_LEFT:  return DIR_LEFT;
    case BUTTON_RIGHT: return DIR_RIGHT;
  }
  /* UNREACHABLE */
  return DIR_UP; // To shut up the warning.
}

static void Game_pressButton(Game *game, Button button)
{
  Snake_changeDirection(game->snake, buttonToDirection(button));
}

const char *Game_init(Game *game,
            unsigned int fps,
            unsigned int snake_x,
            unsigned int snake_y)
{
  Snake *snake = Snake_new(snake_x, snake_y);
  if (snake == NULL)
    return "Snake limit reached";

  game->snake = snake;
  game->fps = fps;
  return NULL;
}

int scale;
int window_w;
int window_h;

void drawPixel(SDL_Renderer *ren, int x, int y)
{
  int tile_w = window_w / DISPLAY_WIDTH;
  int tile_h = window_h / DISPLAY_HEIGHT;
  SDL_Rect rect;
  rect.w = tile_w;
  rect.h = tile_h;
  rect.x = x * tile_w;
  rect.y = y * tile_h;
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &rect);
}

int main(void) 
{

  SDL_Init(SDL_INIT_VIDEO);

  scale = 6;
  window_w = DISPLAY_WIDTH  * scale;
  window_h = DISPLAY_HEIGHT * scale;

  SDL_Window *win = SDL_CreateWindow("GAME", 
                     SDL_WINDOWPOS_CENTERED, 
                     SDL_WINDOWPOS_CENTERED, 
                     window_w, window_h, 0);
  if (win == 0) {
    fprintf(stderr, "Failed to create window\n");
    return -1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (ren == 0) {
    fprintf(stderr, "Failed to create renderer\n");
    return -1;
  }

  unsigned int fps = 10;
  unsigned int snake_x = 30;
  unsigned int snake_y = 30;

  Game game;
  const char *error = Game_init(&game, fps, snake_x, snake_y);
  if (error) {
    /* Do nothing for now */
    fprintf(stderr, "Failed to create game object\n");
    return -1;
  }

  for (int i = 0; i < 8; ++i) {
    Snake_grow(game.snake);
    Snake_step(game.snake);
  }

  _Bool running = 1;

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

        case SDL_QUIT: running = 0; break;

        case SDL_KEYDOWN:
        {
          switch (event.key.keysym.sym) {
            case SDLK_UP:    Game_pressButton(&game, BUTTON_UP);    break;
            case SDLK_DOWN:  Game_pressButton(&game, BUTTON_DOWN);  break;
            case SDLK_LEFT:  Game_pressButton(&game, BUTTON_LEFT);  break;
            case SDLK_RIGHT: Game_pressButton(&game, BUTTON_RIGHT); break;
            case SDLK_SPACE: Snake_grow(game.snake); break;
          }
          break;
        }
      }
    }

    Snake_step(game.snake);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    /* Render the snake */
    Position pos;
    for (int i = 0; Snake_getBodyPosition(game.snake, i, &pos); ++i) {
      printf("(x=%d, y=%d)\n", pos.x, pos.y);
      drawPixel(ren, pos.x, pos.y);
    }

    /* Update */
    SDL_RenderPresent(ren);
    SDL_Delay(1000 / game.fps);
  }

  SDL_Quit();
}