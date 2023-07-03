#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
									//ELSE USE DEFAULT VALUES
//main.exe InputValuesFile="myfile.txt" OutputGraphFile="myoutputfile.ppm" Width=500 Height=500  -Windowed

typedef float f32;
typedef unsigned int u32;
typedef int32_t i32;
typedef uint8_t byte;

#define BLACK 0x000000
#define RED 0xFF0000
#define	BLUE 0x0000FF
#define	GREEN 0x00FF00
#define	WHITE 0xFFFFFF
#define	GRAY 0x808080
#define	YELLOW 0xFFFF00
#define	PURPLE 0x800080
#define	ORANGE 0xFFA500
#define	TEAL 0x008080
#define	NAVY 0x000080
#define	OLIVE 0x808000

#define DEFAULT_IMAGE_HEIGHT 600
#define DEFAULT_IMAGE_WIDTH 600

u32 max(i32 a, i32 b){
	return a > b ? a : b;
}
u32 min(i32 a, i32 b){
	return a < b ? a : b;
}

typedef struct image{
	u32 w;
	u32 h;
	u32* pixels;
}image;

image* new_image(u32 image_width, u32 image_height){
	image* img = malloc(sizeof(image));
	if(img == NULL) return 0;
	img->w = image_width;
	img->h = image_height;

	img->pixels = malloc(sizeof(u32) * (image_width * image_height));
	if(img == NULL) return 0;	
	
	return img;
}

u32 fill_image(image* img, u32 color) {
    u32 num_pixels = img->w * img->h;
    for (u32 i = 0; i < num_pixels; ++i) {
        img->pixels[i] = color;
    }

    return 0; 
}

u32 set_pixel(image* img, u32 xp, u32 yp, u32 color){
	img->pixels[yp * img->w + xp] = color;

	return 0; 	
}

void swap_ints(i32 *a, i32 *b){
	i32 t = *a;
	*a = *b;
	*b = t;
}

void draw_line(image* img, u32 x1, u32 y1, u32 x2, u32 y2, u32 color) {
    i32 dx = abs(x2 - x1);
    i32 dy = abs(y2 - y1);
    i32 xi = x1 < x2 ? 1 : -1;
    i32 yi = y1 < y2 ? 1 : -1;
    
    i32 D = (dx > dy ? dx : -dy) / 2;
    i32 x = x1;
	i32 y = y1;
	
    for (;;) {
    	set_pixel(img, x, y, color);

		if(x == x2 && y == y2) break;

		i32 pD = D;
        
        if (pD > -dx) {
            x += xi;
            D -= dy;
        }

		if(pD < dy){
			y += yi;
			D += dx;
		}     	   
    }
}

void draw_line_dotted(image* img, u32 x1, u32 y1, u32 x2, u32 y2, u32 spacing, u32 color) {
    i32 dx = abs(x2 - x1);
    i32 dy = abs(y2 - y1);
    i32 xi = x1 < x2 ? 1 : -1;
    i32 yi = y1 < y2 ? 1 : -1;
    
    i32 D = (dx > dy ? dx : -dy) / 2;
    i32 x = x1;
	i32 y = y1;

	u32 spacing_count = 0;
	for (;;) {
    	if (spacing_count > spacing) {
    		set_pixel(img, x, y, color);
			spacing_count = 0;
    	}

		if(x == x2 && y == y2) break;

		i32 pD = D;
        
        if (pD > -dx) {
            x += xi;
            D -= dy;
        }

		if(pD < dy){
			y += yi;
			D += dx;
		}     	   
		spacing_count++;
    }
}

void draw_rectangle(image* img, bool filled, u32 x1, u32 y1, u32 x2, u32 y2, u32 color, u32 fill_color) {
	if(y1 == y2 && x1 == x2){
		set_pixel(img, x1, y1, color);
		return;
	}
	
	u32 tlx, tly, blx, bly;
	
	tlx = x1;
	blx = x2;
	bly = y1;	
	tly = y2;

	if(y1 == y2 || x1 == x2){
		draw_line(img, x1, y1, x2, y2, color);
		return;
	}
	
	draw_line(img, x1, y1, tlx, tly, color);
	draw_line(img, tlx, tly, x2, y2, color);
	draw_line(img, x2, y2, blx, bly, color);
	draw_line(img, blx, bly, x1, y1, color);

	if(filled){ //avoid overlapping
		for(u32 y = y1 + 1; y < y2; y++){
			draw_line(img, x1 + 1, y, x2 - 1, y, fill_color);
		}
	}
}

void draw_circle(image* img, bool filled, u32 radius, u32 x1, u32 y1, u32 color, u32 fill_color){
	u32 r = radius;
	i32 E = -r;
	i32 X = radius;
	i32 Y = 0;

	for(;;){		
		if(Y > X) break;
		
		if(filled && Y < X + 1){ // Y == X (Y < X + 1) stop drawing lines to avoid overlapping with edge pixels
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

		if(E >= 0) {
			E -= 2 * X - 1;
			X--;
		}
	}
}

void draw_cubic_bezier(image* img, u32* x, u32* y, u32 num_points, u32 color) {
    f32 xp, yp;
	u32 m;
	 
    for (f32 u = 0.0f; u <= 1.0f; u += 0.0001f) {
        xp = 0.0f;
        yp = 0.0f;
	
        for (u32 i = 0; i < num_points; i++) {
			m = i == 0 ? 1 : 3;
			m = i == num_points - 1 ? 1 : m;
        
            f32 coefficient = m * pow(1 - u, num_points - i - 1) * pow(u, i);
            xp += coefficient * x[i];
            yp += coefficient * y[i];
        }

        set_pixel(img, (u32)xp, (u32)yp, color);
    }
}

u32 save_image_to_ppm(image img, const char* output_ppm){
    FILE *f = fopen(output_ppm, "wb");

    fprintf(f, "P6\n%i %i 255\n", img.w, img.h);
	if(ferror(f)) goto exit; 

	u32 num_pixels = img.w * img.h;

        for(u32 i = 0; i < num_pixels; ++i){       
	        byte bytes[3] = {
	        		//RRGGBB
					(img.pixels[i] >> (8*2)) & 0xFF,
            		(img.pixels[i] >> (8*1)) & 0xFF,
                    (img.pixels[i] >> (8*0)) & 0xFF,
                };
		fwrite(bytes, sizeof(bytes), 1, f);
		if(ferror(f)) goto exit;
        }

        fclose(f);
	return 0;
exit:
	if(f) fclose(f);
	return -1;
}

char* strtolower(char* p) {
    for (; *p; p++) {
        *p = (char)tolower(*p);
    }
    return p;
}

u32 image_width, image_height;

u32 main(u32 argc, char *argv[], char *envp[]){
	for(u32 i = 1; i < argc; ++i){
		strtolower(argv[i]);
		if(strncasecmp(argv[i], "width=", 6) == 0){
			sscanf(argv[i], "width=%d", &image_width);
			printf("Width: %i\n", image_width);
		}
		if(strncasecmp(argv[i], "height=", 6) == 0){
			sscanf(argv[i], "height=%d", &image_height);
			printf("Height: %i\n", image_height);
		}  
	}

	if(image_width == 0) image_width = DEFAULT_IMAGE_WIDTH;	
	if(image_height == 0) image_height = DEFAULT_IMAGE_HEIGHT;

	image *img = new_image(image_width, image_height);

	fill_image(img, WHITE);
	
    draw_line(img, 250, 250, 400, 450, RED);
    draw_line(img, 249, 250, 400, 451, RED);
    draw_line(img, 250, 250, 450, 0, RED);
    draw_line(img, 250, 250, 0, 50, RED);
	
	draw_line_dotted(img, 250, 250, 300, 450, 1, RED);
	draw_line_dotted(img, 250, 250, 10, 450, 5, RED);

	draw_circle(img, false, 4, 50, 50, RED, RED);
	draw_circle(img, true, 10, 100, 100, RED, BLUE);
	draw_circle(img, true, 20, 150, 150, RED, YELLOW);
	draw_circle(img, true, 20, 100, 450, RED, OLIVE);

	draw_rectangle(img, false, 3, 7, 9, 3, RED, RED);
	draw_rectangle(img, true, 10, 10, 25, 30, RED, YELLOW);

	u32 x_points[4] = {
		50, 35, 75, 85
	};

	u32 y_points[4] = {
		500, 400, 450, 500
	};

	draw_cubic_bezier(img, x_points, y_points, 4, YELLOW);
			
	if(save_image_to_ppm(*img, "output.ppm") < 0) printf("Failed creating .ppm");
	
	free(img);
	return 0;
}
