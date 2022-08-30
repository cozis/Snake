#include "config.h"

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
Snake   *Snake_new(int start_x, int start_y);
void     Snake_step(Snake *snake);
void     Snake_grow(Snake *snake);
void     Snake_changeDirection(Snake *snake, Direction new_dir);
_Bool    Snake_getBodyPosition(Snake *snake, unsigned int n, Position *pos);
