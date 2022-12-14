#include "config.h"

typedef enum {
  SEV_NONE,
  SEV_BITE,
} SnakeEvent;

typedef struct {
  unsigned char x, y;
} Position;

typedef enum {
  DIR_LEFT, DIR_RIGHT,
  DIR_UP,   DIR_DOWN,
} Direction;

#define POSITION(X, Y) ((Position) { \
  .x = (X) % DISPLAY_WIDTH,          \
  .y = (Y) % DISPLAY_HEIGHT          \
})

typedef struct Snake Snake;
Snake*       Snake_new(int start_x, int start_y);
SnakeEvent   Snake_step(Snake *snake);
void         Snake_grow(Snake *snake);
unsigned int Snake_size(Snake *snake);
void         Snake_changeDirection(Snake *snake, Direction new_dir);
_Bool        Snake_getBodyPosition(Snake *snake, unsigned int n, Position *pos);
_Bool        Snake_occupiesPosition(Snake *snake, Position pos);