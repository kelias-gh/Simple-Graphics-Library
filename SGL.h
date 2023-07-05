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
  HWND windowHandle;

  u32 w;
  u32 h;
  u32 *pixels;
}image;

typedef enum KEYS
{
  SGL_A = 0,
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
  KEY_ACTION_COUNT
} KEYS;

extern bool key_is_down[KEY_ACTION_COUNT];

typedef union vec2
{
  struct
  {
    f32 x, y;
  };
  f32 values[2];
}vec2;

typedef union ivec2
{
  struct
  {
    i32 x, y;
  };
  i32 values[2];
}ivec2;

extern ivec2 mouse_pos;

image *initialize_SGL(u32 image_width, u32 image_height, const char *title);

void present();

void receive_msg(MSG msg, bool *is_running);

void destroy(image *img);

void fill_image(image *img, u32 color);

void set_pixel(image *img, u32 xp, u32 yp, u32 color);

void draw_line(image *img, u32 x1, u32 y1, u32 x2, u32 y2, u32 color);

void draw_line_dotted(image *img, u32 x1, u32 y1, u32 x2, u32 y2, u32 spacing, u32 color);

void draw_rectangle(image *img, bool filled, u32 x1, u32 y1, u32 x2, u32 y2, u32 color, u32 fill_color);

void draw_circle(image *img, bool filled, i32 radius, i32 x1, i32 y1, u32 color, u32 fill_color);

void draw_cubic_bezier(image *img, u32 *x, u32 *y, u32 num_points, u32 color);

bool button_rect(i32 x1, i32 y1, i32 x2, i32 y2);

bool button_circle(i32 radius, i32 x1, i32 y1);

u32 save_image_to_ppm(image img, const char *output_ppm);

char *strtolower(char *p);
