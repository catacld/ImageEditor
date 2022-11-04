#include "image_rotate.h"

void select_all(struct image *imagine) // select the entire image
{
	if (imagine->is_image_loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	imagine->x1 = 0;
	imagine->y1 = 0;
	imagine->x2 = imagine->size_x;
	imagine->y2 = imagine->size_y;
	printf("Selected ALL\n");
}

int MIN(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

int MAX(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

void select_coords(struct image *imagine, int x1_, int x2_, int y1_, int y2_)
{
	// check if there is an image loaded
	if (imagine->is_image_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	//v check if the coordinates are valid
	if (x1_ < 0 || x1_ > imagine->size_x ||
		x2_ < 0 || x2_ > imagine->size_x ||
		y1_ < 0 || y1_ > imagine->size_y ||
		y2_ < 0 || y2_ > imagine->size_y ||
		x1_ == x2_ || y1_ == y2_) {
		printf("Invalid set of coordinates\n");
		return;
	}
	// store the coordinates
	imagine->x1 = MIN(x1_, x2_);
	imagine->y1 = MIN(y1_, y2_);
	imagine->x2 = MAX(x1_, x2_);
	imagine->y2 = MAX(y1_, y2_);
	printf("Selected %d %d %d %d\n",
		   imagine->x1, imagine->y1, imagine->x2, imagine->y2);
}

void crop(struct image *imagine)
{	// check if we have an image loaded
	if (imagine->is_image_loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int new_size_x = imagine->x2 - imagine->x1;
	int new_size_y = imagine->y2 - imagine->y1;
	unsigned char **grey_pixels2;
	struct color **color_pixels2;

	switch (imagine->type) {
	case P2:
	case P5:
		// create a new matrix
		grey_pixels2 =
		(unsigned char **)malloc(new_size_y * sizeof(unsigned char *));
		for (int i = 0; i < new_size_y; i++)
			grey_pixels2[i] =
			(unsigned char *)malloc(new_size_x * sizeof(unsigned char));
		// make the required adjustments
		for (int i = 0; i < new_size_y; i++)
			for (int j = 0; j < new_size_x; j++)
				grey_pixels2[i][j] =
				imagine->grey_pixels[i + imagine->y1][j + imagine->x1];
		// copy the old matrix into the new matrix
		free_content(imagine);
		imagine->is_image_loaded = 1;
		imagine->size_x = new_size_x;
		imagine->size_y = new_size_y;
		imagine->grey_pixels = grey_pixels2;
		break;

	case P3:
	case P6:
		// similar to the case P5 but the image is color
		// modify r, g and b
		color_pixels2 =
		(struct color **)malloc(new_size_y * sizeof(struct color *));
		for (int i = 0; i < new_size_y; i++)
			color_pixels2[i] =
			(struct color *)malloc(new_size_x * sizeof(struct color));

		for (int i = 0; i < new_size_y; i++)
			for (int j = 0; j < new_size_x; j++) {
				color_pixels2[i][j].r =
				imagine->color_pixels[i + imagine->y1][j + imagine->x1].r;
				color_pixels2[i][j].g =
				imagine->color_pixels[i + imagine->y1][j + imagine->x1].g;
				color_pixels2[i][j].b =
				imagine->color_pixels[i + imagine->y1][j + imagine->x1].b;
			}

		free_content(imagine);
		imagine->is_image_loaded = 1;
		imagine->size_x = new_size_x;
		imagine->size_y = new_size_y;
		imagine->color_pixels = color_pixels2;

		break;
	}
	imagine->x1 = 0;
	imagine->y1 = 0;
	imagine->x2 = imagine->size_x;
	imagine->y2 = imagine->size_y;
	printf("Image cropped\n");
}