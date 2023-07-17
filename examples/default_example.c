#include "SGL.h"
#include <stdio.h>

i32 main()
{
  initialize_SGL(600, 600, "SGL");
  u32 a = 0;
  u32 inc = 0;

  bool is_running = true;

  while (is_running)
  {
    receive_msg(&is_running);

    if (a == 600)
      inc = -1;
    if (a == 0)
      inc = 1;
    a += inc;

    draw_line(250, 250, 400, a, RED);
    draw_line(249, 250, 400, 451, RED);
    draw_line(250, 250, 450, 0, RED);
    draw_line(250, 250, 0, 50, RED);

    draw_line_dotted(250, 250, 300, 450, 1, RED);
    draw_line_dotted(250, 250, 10, 450, 5, RED);

    draw_circle(false, 4, 50, 50, RED, RED);
    draw_circle(true, 10, 100, 100, RED, BLUE);
    draw_circle(true, 20, 150, 150, RED, YELLOW);
    draw_circle(true, 20, 100, 450, RED, OLIVE);

    draw_rectangle(true, 3, 7, 9, 3, RED, RED);
    draw_rectangle(true, 10, 10, 25, 30, RED, YELLOW);
    draw_line(-5, 5, 5, 5, RED);
    draw_circle(true, 10, -5, 5, RED, RED);
    u32 x_points[4] = {
        35, 50, 75, 85};

    u32 y_points[4] = {
        500, 400, 450, 500};

    draw_cubic_bezier(x_points, y_points, 4, YELLOW);

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