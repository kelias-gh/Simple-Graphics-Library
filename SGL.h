#ifndef sgl_main
#define sgl_main

#define BLACK 0x000000
#define RED 0xFF0000
#define BLUE 0x0000FF
#define GREEN 0x00FF00
#define WHITE 0xFFFFFF
#define GRAY 0x808080
#define YELLOW 0xFFFF00
#define PURPLE 0x800080
#define ORANGE 0xFFA500
#define TEAL 0x008080
#define NAVY 0x000080
#define OLIVE 0x808000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

typedef float f32;
typedef unsigned int u32;
typedef int32_t i32;
typedef uint8_t byte;

typedef struct image
{
  HWND window_handle;

  u32 w;
  u32 h;
  u32 *pixels;
} image;

typedef enum KEYS
{
  SGL_0 = 0,
  SGL_1,
  SGL_2,
  SGL_3,
  SGL_4,
  SGL_5,
  SGL_6,
  SGL_7,
  SGL_8,
  SGL_9,
  SGL_A,
  SGL_B,
  SGL_C,
  SGL_D,
  SGL_E,
  SGL_F,
  SGL_G,
  SGL_H,
  SGL_I,
  SGL_J,
  SGL_K,
  SGL_L,
  SGL_M,
  SGL_N,
  SGL_O,
  SGL_P,
  SGL_Q,
  SGL_R,
  SGL_S,
  SGL_T,
  SGL_U,
  SGL_V,
  SGL_W,
  SGL_X,
  SGL_Y,
  SGL_Z,
  SGL_UP_ARROW,
  SGL_LEFT_ARROW,
  SGL_RIGHT_ARROW,
  SGL_DOWN_ARROW,
  SGL_LMB,
  SGL_RMB,
  SGL_MMB,
  SGL_TAB,
  SGL_BACKSPACE,
  SGL_SHIFT,
  SGL_CTRL,
  SGL_ESC,
  SGL_ENTER,
  KEY_ACTION_COUNT
} KEYS;

char CHARS[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
};

extern bool key_is_down[KEY_ACTION_COUNT];

typedef union vec2
{
  struct
  {
    f32 x, y;
  };
  f32 values[2];
} vec2;

typedef union ivec2
{
  struct
  {
    i32 x, y;
  };
  i32 values[2];
} ivec2;

extern vec2 mouse_pos;

image *
get_image();

void initialize_SGL(u32 image_width, u32 image_height, const char *title);

void present(u32 background_color);

void receive_msg(bool *is_running);

void fill_image(u32 color);

void set_pixel(i32 xp, i32 yp, u32 color);

void draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u32 color);

void draw_line_dotted(u32 x1, u32 y1, u32 x2, u32 y2, u32 spacing, u32 color);

void draw_rectangle(bool filled, i32 x1, i32 y1, i32 x2, i32 y2, u32 color, u32 fill_color);
bool button_rect(i32 x1, i32 y1, i32 x2, i32 y2);

void draw_circle(bool filled, i32 radius, i32 x1, i32 y1, u32 color, u32 fill_color);
bool button_circle(i32 radius, i32 x1, i32 y1);

void draw_cubic_bezier(u32 *x, u32 *y, u32 num_points, u32 color);

u32 save_image_to_ppm(const char *output_ppm);

char *strtolower(char *p);

f32 ms_time_now();

#endif