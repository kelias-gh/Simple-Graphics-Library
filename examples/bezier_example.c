#include "SGL.h"
#include <stdio.h>
#include <time.h>

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

i32 main()
{
  initialize_SGL(600, 600, "SGL");

  bool is_running = true;
  bool clicked_circle_button[4] = {0};
  u32 control_points_x[4] = {0};
  u32 control_points_y[4] = {0};

  time_t t;
  srand((unsigned)time(&t));

  for (u32 i = 0; i < 4; ++i)
  {
    control_points_x[i] = rand() % 401 + 100 + (i * 10);
    control_points_y[i] = rand() % 401 + 100 + (i * 10);
  }

  const u32 radius = 10;

  while (is_running)
  {
    receive_msg(&is_running);

    draw_cubic_bezier(control_points_x, control_points_y, 4, RED);
    for (u32 i = 0; i < 4; ++i)
    {
      if (control_points_x[i] >= get_image()->w - radius)
        control_points_x[i] = get_image()->w - radius;

      if (control_points_x[i] < radius)
        control_points_x[i] = radius;

      if (control_points_y[i] >= get_image()->h - radius)
        control_points_y[i] = get_image()->w - radius;

      if (control_points_y[i] < radius)
        control_points_y[i] = radius;

      draw_circle(true, radius, control_points_x[i], control_points_y[i], RED, RED);
      clicked_circle_button[i] = button_circle(radius, control_points_x[i], control_points_y[i]);

      if (clicked_circle_button[i] && key_is_down[SGL_LMB])
      {
        control_points_x[i] = mouse_pos.x;
        control_points_y[i] = mouse_pos.y;
      }
    }

    if (key_is_down[SGL_S])
    {
      if (save_image_to_ppm("output.ppm") < 0)
      {
        printf("Failed creating .ppm");
      }
    }

    present(WHITE);
  }

  return 0;
}