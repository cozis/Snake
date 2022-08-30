#include <assert.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "snake.h"

typedef struct {
  unsigned int fps;
  Snake *snake;
  _Bool lost;

  _Bool    apple_spawned;
  Position apple_pos;
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

static Position randomPosition()
{
  return POSITION(rand(), rand());
}

static void Game_spawnApple(Game *game)
{
  if (game->apple_spawned)
    return; // Can't have multiple apples at once.

  Position pos;
  do 
    pos = randomPosition();
  while (Snake_occupiesPosition(game->snake, pos));

  game->apple_spawned = 1;
  game->apple_pos = pos;
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
  game->lost = 0;
  game->apple_spawned = 0;
  Game_spawnApple(game);
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
  SDL_RenderFillRect(ren, &rect);
}

typedef enum {
  GEV_WIN,
  GEV_LOSE,
  GEV_NONE,
} GameEvent;

GameEvent Game_step(Game *game)
{
  switch(Snake_step(game->snake)) {
    case SEV_BITE: return GEV_LOSE;
    case SEV_NONE: /* OK */ break;
  }

  if (game->apple_spawned && Snake_occupiesPosition(game->snake, game->apple_pos)) {

    if (Snake_size(game->snake) == DISPLAY_WIDTH * DISPLAY_HEIGHT)
      return GEV_WIN;

    Snake_grow(game->snake);
    game->apple_spawned = 0;
    Game_spawnApple(game);
  }

  return GEV_NONE;
}

static void drawSnake(Game *game, SDL_Renderer *ren)
{
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

  Position pos;
  for (int i = 0; Snake_getBodyPosition(game->snake, i, &pos); ++i)
    drawPixel(ren, pos.x, pos.y);
}

static void drawApple(Game *game, SDL_Renderer *ren)
{
  if (game->apple_spawned) {
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    drawPixel(ren, game->apple_pos.x, 
                   game->apple_pos.y);
  }
}

static void drawGame(Game *game, SDL_Renderer *ren)
{
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  drawSnake(game, ren);
  drawApple(game, ren);

  SDL_RenderPresent(ren);
}

static _Bool handleEvents(Game *game)
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      
      case SDL_QUIT: 
      return 0;
      
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym) {
          case SDLK_UP:    Game_pressButton(game, BUTTON_UP);    break;
          case SDLK_DOWN:  Game_pressButton(game, BUTTON_DOWN);  break;
          case SDLK_LEFT:  Game_pressButton(game, BUTTON_LEFT);  break;
          case SDLK_RIGHT: Game_pressButton(game, BUTTON_RIGHT); break;        
        }
        break;
      }
    }
  }
  return 1;
}

typedef enum {
  GRES_WIN,
  GRES_LOSE,
  GRES_BORED,
} GameResult;

GameResult Game_play(Game *game, SDL_Renderer *ren)
{
  while (handleEvents(game)) {
    switch (Game_step(game)) {
      case GEV_WIN:  return GRES_WIN;
      case GEV_LOSE: return GRES_LOSE;
      case GEV_NONE: /* OK */ break;
    }
    drawGame(game, ren);
    SDL_Delay(1000 / game->fps);
  }
  return GRES_BORED;
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

  unsigned int fps = 30;
  unsigned int snake_x = 30;
  unsigned int snake_y = 30;

  Game game;
  const char *error = Game_init(&game, fps, snake_x, snake_y);
  if (error) {
    /* Do nothing for now */
    fprintf(stderr, "Failed to create game object\n");
    return -1;
  }

  switch (Game_play(&game, ren)) {
    case GRES_WIN:  fprintf(stderr, "You win!\n"); break;
    case GRES_LOSE: fprintf(stderr, "You lose!\n"); break;
    case GRES_BORED:break;
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}