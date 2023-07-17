#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "SGL.h"

vec2 mouse_pos = {0};

bool key_is_down[KEY_ACTION_COUNT] = {false};

BITMAPINFO bitmap_info;
HBITMAP frame_bitmap = 0;
HDC device_context = 0;

image *img = {0};

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

image *get_image()
{
  return img;
}

void initialize_SGL(u32 image_width, u32 image_height, const char *title)
{
  img = malloc(sizeof(image));
  if (img == NULL)
    return;

  img->pixels = malloc(sizeof(u32) * (image_width * image_height));
  if (img->pixels == NULL)
    return;

  const wchar_t CLASS_NAME[] = L"SimpleGraphicsClass";

  WNDCLASSW WindowClass = {};
  WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass.lpfnWndProc = WndProc;
  WindowClass.hInstance = GetModuleHandleW(NULL);
  WindowClass.lpszClassName = CLASS_NAME;
  WindowClass.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);

  if (RegisterClassW(&WindowClass))
  {
    RECT windowRect = {0, 0, image_width, image_height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    wchar_t *titleW = L"";
    if (title != NULL)
    {
      size_t length = strlen(title) + 1;
      titleW = (wchar_t *)malloc(sizeof(wchar_t) * length);
      mbstowcs(titleW, title, length);
    }

    img->window_handle = CreateWindowExW(
        0,
        WindowClass.lpszClassName,
        (wchar_t *)titleW,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        0,
        0,
        GetModuleHandle(NULL),
        0);
  }

  device_context = GetDC(img->window_handle);

  bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
  bitmap_info.bmiHeader.biWidth = image_width;
  bitmap_info.bmiHeader.biHeight = -image_height;
  bitmap_info.bmiHeader.biPlanes = 1;
  bitmap_info.bmiHeader.biBitCount = 32;
  bitmap_info.bmiHeader.biCompression = BI_RGB;

  RECT clientRect = {0};
  GetClientRect(img->window_handle, &clientRect);

  img->w = clientRect.right - clientRect.left;
  img->h = clientRect.bottom - clientRect.top;
}

void receive_msg(bool *is_running)
{
  MSG msg = {};
  if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      *is_running = false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void present(u32 background_color)
{
  StretchDIBits(device_context, 0, 0, img->w, img->h, 0, 0, img->w, img->h, img->pixels, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
  fill_image(background_color);
}

void fill_image(u32 color)
{
  u32 num_pixels = img->w * img->h;
  for (u32 i = 0; i < num_pixels; ++i)
  {
    img->pixels[i] = color;
  }
}

void set_pixel(i32 xp, i32 yp, u32 color)
{
  if (xp > 0 && yp > 0 && xp < img->w && yp < img->h)
    img->pixels[yp * img->w + xp] = color;
}

void swap_ints(i32 *a, i32 *b)
{
  i32 t = *a;
  *a = *b;
  *b = t;
}

void draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u32 color)
{
  i32 dx = abs(x2 - x1);
  i32 dy = abs(y2 - y1);
  i32 xi = x1 < x2 ? 1 : -1;
  i32 yi = y1 < y2 ? 1 : -1;

  i32 D = (dx > dy ? dx : -dy) / 2;
  i32 x = x1;
  i32 y = y1;

  for (;;)
  {
    set_pixel(x, y, color);

    if (x == x2 && y == y2)
      break;

    i32 pD = D;

    if (pD > -dx)
    {
      x += xi;
      D -= dy;
    }

    if (pD < dy)
    {
      y += yi;
      D += dx;
    }
  }
}

void draw_line_dotted(u32 x1, u32 y1, u32 x2, u32 y2, u32 spacing, u32 color)
{
  i32 dx = abs(x2 - x1);
  i32 dy = abs(y2 - y1);
  i32 xi = x1 < x2 ? 1 : -1;
  i32 yi = y1 < y2 ? 1 : -1;

  i32 D = (dx > dy ? dx : -dy) / 2;
  i32 x = x1;
  i32 y = y1;

  u32 spacing_count = 0;
  for (;;)
  {
    if (spacing_count > spacing)
    {
      set_pixel(x, y, color);
      spacing_count = 0;
    }

    if (x == x2 && y == y2)
      break;

    i32 pD = D;

    if (pD > -dx)
    {
      x += xi;
      D -= dy;
    }

    if (pD < dy)
    {
      y += yi;
      D += dx;
    }
    spacing_count++;
  }
}

void draw_rectangle(bool filled, i32 x1, i32 y1, i32 x2, i32 y2, u32 color, u32 fill_color)
{
  if (y1 == y2 && x1 == x2)
  {
    set_pixel(x1, y1, color);
    return;
  }

  if (y1 > y2)
    swap_ints(&y2, &y1);

  if (y1 > y2)
    swap_ints(&y2, &y1);

  u32 tlx, tly, blx, bly;

  tlx = x1;
  blx = x2;
  bly = y1;
  tly = y2;

  if (y1 == y2 || x1 == x2)
  {
    draw_line(x1, y1, x2, y2, color);
    return;
  }

  draw_line(x1, y1, tlx, tly, color);
  draw_line(tlx, tly, x2, y2, color);
  draw_line(x2, y2, blx, bly, color);
  draw_line(blx, bly, x1, y1, color);

  if (filled)
  { // avoid overlapping
    for (u32 y = y1 + 1; y < y2; y++)
    {
      draw_line(x1 + 1, y, x2 - 1, y, fill_color);
    }
  }
}

void draw_circle(bool filled, i32 radius, i32 x1, i32 y1, u32 color, u32 fill_color)
{
  u32 r = radius;
  i32 E = -r;
  i32 X = r;
  i32 Y = 0;

  for (;;)
  {
    if (Y > X)
      break;

    if (filled && Y < X + 1)
    {
      draw_line(x1 - X, y1 - Y, x1 + X, y1 - Y, fill_color);
      draw_line(x1 - X, y1 + Y, x1 + X, y1 + Y, fill_color);
      draw_line(x1 - Y, y1 + X, x1 - Y, y1 - X, fill_color);
      draw_line(x1 + Y, y1 + X, x1 + Y, y1 - X, fill_color);
    }

    set_pixel(x1 - X, y1 - Y, color);
    set_pixel(x1 + X, y1 - Y, color);
    set_pixel(x1 - X, y1 + Y, color);
    set_pixel(x1 + X, y1 + Y, color);

    set_pixel(x1 + Y, y1 + X, color);
    set_pixel(x1 - Y, y1 + X, color);
    set_pixel(x1 + Y, y1 - X, color);
    set_pixel(x1 - Y, y1 - X, color);

    E += 2 * Y + 1;
    Y++;

    if (E >= 0)
    {
      E -= 2 * X - 1;
      X--;
    }
  }
}

void draw_cubic_bezier(u32 *x, u32 *y, u32 num_points, u32 color)
{
  f32 xp, yp;
  u32 m;

  for (f32 u = 0.0f; u <= 1.0f; u += 0.0001f)
  {
    xp = 0.0f;
    yp = 0.0f;

    for (u32 i = 0; i < num_points; i++)
    {
      m = i == 0 ? 1 : 3;
      m = i == num_points - 1 ? 1 : m;

      f32 coefficient = m * pow(1 - u, num_points - i - 1) * pow(u, i);
      xp += coefficient * x[i];
      yp += coefficient * y[i];
    }

    set_pixel((u32)xp, (u32)yp, color);
  }
}

u32 save_image_to_ppm(const char *output_ppm)
{
  FILE *f = fopen(output_ppm, "wb");

  fprintf(f, "P6\n%i %i 255\n", (*img).w, (*img).h);
  if (ferror(f))
    goto exit;

  u32 num_pixels = (*img).w * (*img).h;

  for (u32 i = 0; i < num_pixels; ++i)
  {
    byte bytes[3] = {
        // RRGGBB
        ((*img).pixels[i] >> (8 * 2)) & 0xFF,
        ((*img).pixels[i] >> (8 * 1)) & 0xFF,
        ((*img).pixels[i] >> (8 * 0)) & 0xFF,
    };
    fwrite(bytes, sizeof(bytes), 1, f);
    if (ferror(f))
      goto exit;
  }

  fclose(f);
  return 0;
exit:
  if (f)
    fclose(f);
  return -1;
}

char *strtolower(char *p)
{
  for (; *p; p++)
  {
    *p = (char)tolower(*p);
  }
  return p;
}

bool button_rect(i32 x1, i32 y1, i32 x2, i32 y2)
{
  return mouse_pos.x >= x1 && mouse_pos.x <= x2 && mouse_pos.y >= y2 && mouse_pos.y <= y1;
}

bool button_circle(i32 radius, i32 x1, i32 y1)
{
  return x1 - radius <= mouse_pos.x && mouse_pos.x <= x1 + radius && mouse_pos.y <= radius + y1 && y1 - radius <= mouse_pos.y;
}

f32 ms_time_now(){
  static LARGE_INTEGER frequency;

  if(QueryPerformanceFrequency(&frequency)){
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return(1000 * now.QuadPart) / frequency.QuadPart;
  }else{
    return GetTickCount();
  }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_MOUSEMOVE:
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(hwnd, &point);

    mouse_pos.x = point.x;
    mouse_pos.y = point.y;
    break;
  case WM_KEYDOWN:
  case WM_KEYUP:
  {
    bool isDown = (uMsg == WM_KEYDOWN);
    if (wParam == '0')
      key_is_down[SGL_0] = isDown;
    else if (wParam == '1')
      key_is_down[SGL_1] = isDown;
    else if (wParam == '2')
      key_is_down[SGL_2] = isDown;
    else if (wParam == '3')
      key_is_down[SGL_3] = isDown;
    else if (wParam == '4')
      key_is_down[SGL_4] = isDown;
    else if (wParam == '5')
      key_is_down[SGL_5] = isDown;
    else if (wParam == '6')
      key_is_down[SGL_6] = isDown;
    else if (wParam == '7')
      key_is_down[SGL_7] = isDown;
    else if (wParam == '8')
      key_is_down[SGL_8] = isDown;
    else if (wParam == '9')
      key_is_down[SGL_9] = isDown;
    else if (wParam == 'A')
      key_is_down[SGL_A] = isDown;
    else if (wParam == 'B')
      key_is_down[SGL_B] = isDown;
    else if (wParam == 'C')
      key_is_down[SGL_C] = isDown;
    else if (wParam == 'D')
      key_is_down[SGL_D] = isDown;
    else if (wParam == 'E')
      key_is_down[SGL_E] = isDown;
    else if (wParam == 'F')
      key_is_down[SGL_F] = isDown;
    else if (wParam == 'G')
      key_is_down[SGL_G] = isDown;
    else if (wParam == 'H')
      key_is_down[SGL_H] = isDown;
    else if (wParam == 'I')
      key_is_down[SGL_I] = isDown;
    else if (wParam == 'J')
      key_is_down[SGL_J] = isDown;
    else if (wParam == 'K')
      key_is_down[SGL_K] = isDown;
    else if (wParam == 'L')
      key_is_down[SGL_L] = isDown;
    else if (wParam == 'M')
      key_is_down[SGL_M] = isDown;
    else if (wParam == 'N')
      key_is_down[SGL_N] = isDown;
    else if (wParam == 'O')
      key_is_down[SGL_O] = isDown;
    else if (wParam == 'P')
      key_is_down[SGL_P] = isDown;
    else if (wParam == 'Q')
      key_is_down[SGL_Q] = isDown;
    else if (wParam == 'R')
      key_is_down[SGL_R] = isDown;
    else if (wParam == 'S')
      key_is_down[SGL_S] = isDown;
    else if (wParam == 'T')
      key_is_down[SGL_T] = isDown;
    else if (wParam == 'U')
      key_is_down[SGL_U] = isDown;
    else if (wParam == 'V')
      key_is_down[SGL_V] = isDown;
    else if (wParam == 'W')
      key_is_down[SGL_W] = isDown;
    else if (wParam == 'X')
      key_is_down[SGL_X] = isDown;
    else if (wParam == 'Y')
      key_is_down[SGL_Y] = isDown;
    else if (wParam == 'Z')
      key_is_down[SGL_Z] = isDown;
    else if (wParam == VK_UP)
      key_is_down[SGL_UP_ARROW] = isDown;
    else if (wParam == VK_LEFT)
      key_is_down[SGL_LEFT_ARROW] = isDown;
    else if (wParam == VK_DOWN)
      key_is_down[SGL_DOWN_ARROW] = isDown;
    else if (wParam == VK_RIGHT)
      key_is_down[SGL_RIGHT_ARROW] = isDown;
    else if (wParam == VK_TAB)
      key_is_down[SGL_TAB] = isDown;
    else if (wParam == VK_BACK)
      key_is_down[SGL_BACKSPACE] = isDown;
    else if (wParam == VK_SHIFT)
      key_is_down[SGL_SHIFT] = isDown;
    else if (wParam == VK_CONTROL)
      key_is_down[SGL_CTRL] = isDown;
    else if (wParam == VK_ESCAPE)
      key_is_down[SGL_ESC] = isDown;
    else if (wParam == VK_RETURN)
      key_is_down[SGL_ENTER] = isDown;
    break;
  }
  case WM_LBUTTONDOWN:
    key_is_down[SGL_LMB] = true;
    break;
  case WM_LBUTTONUP:
    key_is_down[SGL_LMB] = false;
    break;
  case WM_RBUTTONDOWN:
    key_is_down[SGL_RMB] = true;
    break;
  case WM_RBUTTONUP:
    key_is_down[SGL_RMB] = false;
    break;
  case WM_MBUTTONDOWN:
    key_is_down[SGL_MMB] = true;
    break;
  case WM_MBUTTONUP:
    key_is_down[SGL_MMB] = false;
    break;
  case WM_DESTROY:
  {
    free(img);
    PostQuitMessage(0);
    break;
  }
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}