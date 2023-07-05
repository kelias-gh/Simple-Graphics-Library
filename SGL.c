#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SGL.h"

ivec2 mouse_pos = {0, 0};

bool key_is_down[KEY_ACTION_COUNT] = {false};

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct renderer
{
  BITMAPINFO BitmapInfo;
  HDC DeviceContext;
} renderer;

renderer *rendererContext;

image *initialize_SGL(u32 image_width, u32 image_height, const char *title)
{
  rendererContext = malloc(sizeof(renderer));
  if (rendererContext == NULL)
    return NULL;

  renderer *rdr = rendererContext;

  image *img = malloc(sizeof(image));
  if (img == NULL)
    return NULL;

  img->pixels = malloc(sizeof(u32) * (image_width * image_height));
  if (img->pixels == NULL)
    return NULL;

  WNDCLASS WindowClass = {0};
  WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass.lpfnWndProc = WndProc;
  WindowClass.hInstance = GetModuleHandle(NULL);
  WindowClass.lpszClassName = L"Simple Graphics Library";
  WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

  if (RegisterClass(&WindowClass))
  {
    RECT windowRect = {0, 0, image_width, image_height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    img->windowHandle = CreateWindowEx(
        0,
        WindowClass.lpszClassName,
        title,
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

  rdr->DeviceContext = GetDC(img->windowHandle);

  rdr->BitmapInfo.bmiHeader.biSize = sizeof(rdr->BitmapInfo.bmiHeader);
  rdr->BitmapInfo.bmiHeader.biWidth = image_width;
  // Negative biHeight makes top left as the coordinate system origin. Otherwise, it's bottom left.
  rdr->BitmapInfo.bmiHeader.biHeight = -image_height;
  rdr->BitmapInfo.bmiHeader.biPlanes = 1;
  rdr->BitmapInfo.bmiHeader.biBitCount = 32;
  rdr->BitmapInfo.bmiHeader.biCompression = BI_RGB;

  RECT clientRect = {0};
  GetClientRect(img->windowHandle, &clientRect);

  img->w = clientRect.right - clientRect.left;
  img->h = clientRect.bottom - clientRect.top;

  return img;
}

void present(image *img, u32 background_color)
{
  renderer *rdr = rendererContext;

  StretchDIBits(rdr->DeviceContext, 0, 0, img->w, img->h, 0, 0, img->w, img->h, img->pixels, &rdr->BitmapInfo, DIB_RGB_COLORS, SRCCOPY);

  fill_image(img, background_color);
}

void fill_image(image *img, u32 color)
{
  u32 num_pixels = img->w * img->h;
  for (u32 i = 0; i < num_pixels; ++i)
  {
    img->pixels[i] = color;
  }
}

void set_pixel(image *img, u32 xp, u32 yp, u32 color)
{
  if (xp < img->w && yp < img->h)
    img->pixels[yp * img->w + xp] = color;
}

void swap_ints(i32 *a, i32 *b)
{
  i32 t = *a;
  *a = *b;
  *b = t;
}

void draw_line(image *img, u32 x1, u32 y1, u32 x2, u32 y2, u32 color)
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
    set_pixel(img, x, y, color);

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

void draw_line_dotted(image *img, u32 x1, u32 y1, u32 x2, u32 y2, u32 spacing, u32 color)
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
      set_pixel(img, x, y, color);
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

void draw_rectangle(image *img, bool filled, u32 x1, u32 y1, u32 x2, u32 y2, u32 color, u32 fill_color)
{
  if (y1 == y2 && x1 == x2)
  {
    set_pixel(img, x1, y1, color);
    return;
  }

  u32 tlx, tly, blx, bly;

  tlx = x1;
  blx = x2;
  bly = y1;
  tly = y2;

  if (y1 == y2 || x1 == x2)
  {
    draw_line(img, x1, y1, x2, y2, color);
    return;
  }

  draw_line(img, x1, y1, tlx, tly, color);
  draw_line(img, tlx, tly, x2, y2, color);
  draw_line(img, x2, y2, blx, bly, color);
  draw_line(img, blx, bly, x1, y1, color);

  if (filled)
  { // avoid overlapping
    for (u32 y = y1 + 1; y < y2; y++)
    {
      draw_line(img, x1 + 1, y, x2 - 1, y, fill_color);
    }
  }
}

void draw_circle(image *img, bool filled, i32 radius, i32 x1, i32 y1, u32 color, u32 fill_color)
{
  if (x1 >= img->w - radius)
    x1 = img->w - radius;

  if (x1 < radius)
    x1 = radius;

  if (y1 >= img->h - radius)
    y1 = img->w - radius;

  if (y1 < radius)
    y1 = radius;

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
      draw_line(img, x1 - X, y1 - Y, x1 + X, y1 - Y, fill_color);
      draw_line(img, x1 - X, y1 + Y, x1 + X, y1 + Y, fill_color);
      draw_line(img, x1 - Y, y1 + X, x1 - Y, y1 - X, fill_color);
      draw_line(img, x1 + Y, y1 + X, x1 + Y, y1 - X, fill_color);
    }

    set_pixel(img, x1 - X, y1 - Y, color);
    set_pixel(img, x1 + X, y1 - Y, color);
    set_pixel(img, x1 - X, y1 + Y, color);
    set_pixel(img, x1 + X, y1 + Y, color);

    set_pixel(img, x1 + Y, y1 + X, color);
    set_pixel(img, x1 - Y, y1 + X, color);
    set_pixel(img, x1 + Y, y1 - X, color);
    set_pixel(img, x1 - Y, y1 - X, color);

    E += 2 * Y + 1;
    Y++;

    if (E >= 0)
    {
      E -= 2 * X - 1;
      X--;
    }
  }
}

void draw_cubic_bezier(image *img, u32 *x, u32 *y, u32 num_points, u32 color)
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

    set_pixel(img, (u32)xp, (u32)yp, color);
  }
}

u32 save_image_to_ppm(image img, const char *output_ppm)
{
  FILE *f = fopen(output_ppm, "wb");

  fprintf(f, "P6\n%i %i 255\n", img.w, img.h);
  if (ferror(f))
    goto exit;

  u32 num_pixels = img.w * img.h;

  for (u32 i = 0; i < num_pixels; ++i)
  {
    byte bytes[3] = {
        // RRGGBB
        (img.pixels[i] >> (8 * 2)) & 0xFF,
        (img.pixels[i] >> (8 * 1)) & 0xFF,
        (img.pixels[i] >> (8 * 0)) & 0xFF,
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

void receive_msg(MSG msg, bool *is_running)
{
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

void destroy(image *img)
{
  free(rendererContext);
  free(img);
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
    if (wParam == 'A')
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
    PostQuitMessage(0);
    break;
  }
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}