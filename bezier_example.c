#include "SGL.h"
#include <time.h>

u32 main()
{
  image *img = initialize_SGL(600, 600, L"SGL");
  MSG msg = {0};

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

  while (is_running)
  {
    present(img, WHITE);
    receive_msg(msg, &is_running);

    draw_cubic_bezier(img, control_points_x, control_points_y, 4, RED);
    for (u32 i = 0; i < 4; ++i)
    {
      draw_circle(img, true, 10, control_points_x[i], control_points_y[i], RED, RED);
      clicked_circle_button[i] = button_circle(10, control_points_x[i], control_points_y[i]);

      if (clicked_circle_button[i] && key_is_down[SGL_LMB])
      {
        control_points_x[i] = mouse_pos.x;
        control_points_y[i] = mouse_pos.y;
      }
    }
  }

  if (save_image_to_ppm(*img, "../bezier_curve_example.ppm") < 0)
  {
    printf("Failed creating .ppm");
  }

  destroy(img);

  return 0;
}