#include "image_formats.h"

enum filter {
	EDGE,
	SHARPEN,
	BLUR,
	GAUSSIAN_BLUR,
	UNDIFINED
};

int round_r(double x)
{ // rounding function to minimize the error
	if (x < 0.0)
		return (int)(x - 0.5000001);
	else
		return (int)(x + 0.5000001);
}

unsigned char clamp(double aux)
{
	aux = (double)round_r(aux);

	if (aux < 0)
		aux = 0;
	if (aux > 255)
		aux = 255;
	return (unsigned char)aux;
}

void pixel_filter(struct image *imagine, struct color **color_pixels2,
				  int x, int y, double kernel[][3],
				  double *division_factor)
{
	if (x - 1 < 0 || y - 1 < 0 ||
		x + 1 >= imagine->size_x || y + 1 >= imagine->size_y) {
		color_pixels2[y][x].r =
		imagine->color_pixels[y][x].r;
		color_pixels2[y][x].g =
		imagine->color_pixels[y][x].g;
		color_pixels2[y][x].b =
		imagine->color_pixels[y][x].b;
		return;
	}
	// perform the needed operations to apply the filters
	double val;
	val = 0.0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			val +=
			((double)imagine->color_pixels[y - 1 + i][x - 1 + j].r)
			* kernel[i][j];
	val = val / (*division_factor);
	color_pixels2[y][x].r = clamp(val);

	val = 0.0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			val +=
			((double)imagine->color_pixels[y - 1 + i][x - 1 + j].g)
			* kernel[i][j];
	val = val / (*division_factor);
	color_pixels2[y][x].g = clamp(val);

	val = 0.0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			val +=
			((double)imagine->color_pixels[y - 1 + i][x - 1 + j].b)
			* kernel[i][j];
	val = val / (*division_factor);
	color_pixels2[y][x].b = clamp(val);
}

void calculate_kernel(enum filter filtru, double kernel[][3],
					  double *division_factor)
{	
    // calculate the kernel to apply the filters
	*division_factor = 1;
	switch (filtru) {
	case EDGE:
		kernel[0][0] = -1;
		kernel[0][1] = -1;
		kernel[0][2] = -1;
		kernel[1][0] = -1;
		kernel[1][1] = 8;
		kernel[1][2] = -1;
		kernel[2][0] = -1;
		kernel[2][1] = -1;
		kernel[2][2] = -1;
		break;

	case SHARPEN:
		kernel[0][0] = 0;
		kernel[0][1] = -1;
		kernel[0][2] = 0;
		kernel[1][0] = -1;
		kernel[1][1] = 5;
		kernel[1][2] = -1;
		kernel[2][0] = 0;
		kernel[2][1] = -1;
		kernel[2][2] = 0;
		break;

	case BLUR:
		kernel[0][0] = 1;
		kernel[0][1] = 1;
		kernel[0][2] = 1;
		kernel[1][0] = 1;
		kernel[1][1] = 1;
		kernel[1][2] = 1;
		kernel[2][0] = 1;
		kernel[2][1] = 1;
		kernel[2][2] = 1;
		*division_factor = 9;
		break;

	case GAUSSIAN_BLUR:
		kernel[0][0] = 1;
		kernel[0][1] = 2;
		kernel[0][2] = 1;
		kernel[1][0] = 2;
		kernel[1][1] = 4;
		kernel[1][2] = 2;
		kernel[2][0] = 1;
		kernel[2][1] = 2;
		kernel[2][2] = 1;
		*division_factor = 16;
		break;

	default:
		break;
	}
}

bool apply_filter(struct image *imagine, enum filter filtru)
{	
    // check if an image is loaded
	if (imagine->is_image_loaded == 0) {
		printf("No image loaded\n");
		return 0;
	}
	// if the images are grayscale
	if (imagine->type == P2 || imagine->type == P5) {
		printf("Easy, Charlie Chaplin\n");
		return 0;
	}
	// given parameter is invalid
	if (filtru == UNDIFINED) {
		printf("APPLY parameter invalid\n");
		return 0;
	}

	double kernel[3][3];
	double division_factor = 1;

	calculate_kernel(filtru, kernel, &division_factor);
	// use an auxiliary matrix
	struct color **matrice_aux =
	(struct color **)malloc(imagine->size_y * sizeof(struct color *));
	for (int i = 0; i < imagine->size_y; i++)
		matrice_aux[i] =
		(struct color *)malloc(imagine->size_x * sizeof(struct color));
	// apply the needed filters
	for (int i = imagine->y1; i < imagine->y2; i++)
		for (int j = imagine->x1; j < imagine->x2; j++)
			pixel_filter(imagine, matrice_aux, j, i, kernel,
						 &division_factor);
	// copy the auxiliary matrix into the original one
	for (int i = imagine->y1; i < imagine->y2; i++)
		for (int j = imagine->x1; j < imagine->x2; j++)
			imagine->color_pixels[i][j] = matrice_aux[i][j];
	// free the space used for the auxiliary matrix
	for (int i = 0; i < imagine->size_y; i++)
		free(matrice_aux[i]);
	free(matrice_aux);
	return 1;
}