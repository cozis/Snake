#include "snake.h"

#ifndef MAX_SNAKES
#define MAX_SNAKES 1
#endif

#ifndef MAX_SNAKE_LEN
#define MAX_SNAKE_LEN 32
#endif

typedef struct {
  Direction data[MAX_SNAKE_LEN];
  unsigned int size, head;
} DirectionQueue;

struct Snake {
  Position head;
  Direction dir;
  DirectionQueue body;
  _Bool grow;
};

static Snake snake_pool[MAX_SNAKES];
static int snake_pool_usage = 0;

static void DirectionQueue_init(DirectionQueue *queue)
{
  queue->size = 0;
  queue->head = 0;
}

static void DirectionQueue_push(DirectionQueue *queue, Direction dir)
{
  queue->data[queue->head] = dir;
  queue->head = (queue->head + 1) % MAX_SNAKE_LEN;

  if (queue->size < MAX_SNAKE_LEN)
    queue->size++;
}

static void DirectionQueue_pop(DirectionQueue *queue)
{
  if (queue->size > 0)
    queue->size--;
}

static unsigned int DirectionQueue_size(DirectionQueue *queue)
{
  return queue->size;
}

static Direction DirectionQueue_top(DirectionQueue *queue, unsigned int top)
{
  int i = (queue->head-1 - top) % MAX_SNAKE_LEN;
  return queue->data[i];
}

static Position evaluateNextPosition(Position pos, Direction dir)
{
  switch (dir) {
    case DIR_UP:    return POSITION(pos.x,   pos.y-1);
    case DIR_DOWN:  return POSITION(pos.x,   pos.y+1);
    case DIR_LEFT:  return POSITION(pos.x-1, pos.y);
    case DIR_RIGHT: return POSITION(pos.x+1, pos.y);
  }
  /* UNREACHABLE */
  return POSITION(0, 0); // For the warning.
}

static void Snake_init(Snake *snake, int start_x, int start_y)
{
  snake->head = POSITION(start_x, start_y);
  snake->dir = DIR_LEFT;
  snake->grow = 0;
  DirectionQueue_init(&snake->body);
}

Snake *Snake_new(int start_x, int start_y)
{
  if (snake_pool_usage == MAX_SNAKES)
    return 0; // La pool и piena!

  Snake *snake = snake_pool + snake_pool_usage;
  snake_pool_usage++;

  Snake_init(snake, start_x, start_y);
  return snake;
}

static Direction oppositeDirection(Direction dir)
{
  switch (dir) {
  case DIR_UP: return DIR_DOWN;
  case DIR_DOWN: return DIR_UP;
  case DIR_LEFT: return DIR_RIGHT;
  case DIR_RIGHT: return DIR_LEFT;
  }
  /* UNREACHABLE */
  return DIR_LEFT; // For the warning.
}

void Snake_changeDirection(Snake *snake, Direction new_dir)
{
  if (new_dir != oppositeDirection(snake->dir))
    snake->dir = new_dir;
}

void Snake_step(Snake *snake)
{
  DirectionQueue_push(&snake->body, oppositeDirection(snake->dir));
  snake->head = evaluateNextPosition(snake->head, snake->dir);
  if (!snake->grow)
    DirectionQueue_pop(&snake->body);
  else
    snake->grow = 0;
}

void Snake_grow(Snake *snake)
{
  snake->grow = 1;
}

/*
const char *directionName(Direction dir)
{
  switch (dir) {
    case DIR_UP:    return "DIR_UP";
    case DIR_DOWN:  return "DIR_DOWN";
    case DIR_LEFT:  return "DIR_LEFT";
    case DIR_RIGHT: return "DIR_RIGHT";
  }
  /* UNREACHABLE */
  return 0;
}
*/

_Bool Snake_getBodyPosition(Snake *snake, unsigned int n, Position *pos)
{
  if (n > DirectionQueue_size(&snake->body))
    return 0;

  Position p = snake->head;
  for (unsigned int i = 0; i < n; ++i) {
    Direction dir = DirectionQueue_top(&snake->body, i);
    p = evaluateNextPosition(p, dir);
  }

  if (pos)
    *pos = p;

  return 1;
}