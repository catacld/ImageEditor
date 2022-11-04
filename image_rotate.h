#include "image_filters.h"

void rotate_all(struct image *imagine, int unghi)
{// apply "ROTATE" to the image
	while (unghi > 0) {
		int new_size_x = imagine->size_y;
		int new_size_y = imagine->size_x;
		unsigned char **grey_pixels2;
		struct color **color_pixels2;

		switch (imagine->type) {
		case P2:
		case P5:
			// build a new matrix
			grey_pixels2 =
			(unsigned char **)malloc(new_size_y * sizeof(unsigned char *));
			for (int i = 0; i < new_size_y; i++)
				grey_pixels2[i] =
				(unsigned char *)malloc(new_size_x * sizeof(unsigned char));
			// perform "ROTATE"
			for (int i = 0; i < imagine->size_y; i++)
				for (int j = 0; j < imagine->size_x; j++)
					grey_pixels2[j][new_size_x - 1 - i] =
					imagine->grey_pixels[i][j];
			// copy the new matrix into the old matrix
			free_content(imagine);
			imagine->is_image_loaded = 1;
			imagine->size_x = new_size_x;
			imagine->size_y = new_size_y;
			imagine->grey_pixels = grey_pixels2;
			break;

		case P3:
		case P6: // similar to case P5 but the image is color, modify r, g and b
			color_pixels2 =
			(struct color **)malloc(new_size_y * sizeof(struct color *));
			for (int i = 0; i < new_size_y; i++)
				color_pixels2[i] =
				(struct color *)malloc(new_size_x * sizeof(struct color));

			for (int i = 0; i < imagine->size_y; i++)
				for (int j = 0; j < imagine->size_x; j++) {
					color_pixels2[j][new_size_x - 1 - i].r =
					imagine->color_pixels[i][j].r;
					color_pixels2[j][new_size_x - 1 - i].g =
					imagine->color_pixels[i][j].g;
					color_pixels2[j][new_size_x - 1 - i].b =
					imagine->color_pixels[i][j].b;
				}

			free_content(imagine);
			imagine->is_image_loaded = 1;
			imagine->size_x = new_size_x;
			imagine->size_y = new_size_y;
			imagine->color_pixels = color_pixels2;

			break;
		}
		// the new coordinates after applying "ROTATE"
		imagine->x1 = 0;
		imagine->y1 = 0;
		imagine->x2 = imagine->size_x;
		imagine->y2 = imagine->size_y;
		unghi -= 90; // apply repeated rotations by 90 degrees
	}
}

void rotate_selected(struct image *imagine, int unghi)
{	 // apply "ROTATE" to a specific area
	int transform_size = imagine->x2 - imagine->x1;
	unsigned char **grey_pixels2 = NULL;
	struct color **color_pixels2 = NULL;

	while (unghi > 0) {
		switch (imagine->type) {
		case P2:
		case P5:
			if (!grey_pixels2) { // if there is not one already, build a new matrix
				grey_pixels2 =
				(unsigned char **)
				malloc(transform_size * sizeof(unsigned char *));
				for (int i = 0; i < transform_size; i++)
					grey_pixels2[i] =
					(unsigned char *)
					malloc(transform_size * sizeof(unsigned char));
			}
			// perform the needed operations
			for (int i = 0; i < transform_size; i++)
				for (int j = 0; j < transform_size; j++)
					grey_pixels2[j][transform_size - 1 - i] =
					imagine->grey_pixels[i + imagine->y1][j + imagine->x1];

			for (int i = 0; i < transform_size; i++)
				for (int j = 0; j < transform_size; j++)
					imagine->grey_pixels[i + imagine->y1][j + imagine->x1] =
					grey_pixels2[i][j];

			break;

		case P3:
		case P6:
		// similar to P5 but the image is color, modify r, g and b
			if (!color_pixels2) {
				color_pixels2 =
				(struct color **)
				malloc(transform_size * sizeof(struct color *));
				for (int i = 0; i < transform_size; i++)
					color_pixels2[i] =
					(struct color *)
					malloc(transform_size * sizeof(struct color));
			}

			for (int i = 0; i < transform_size; i++)
				for (int j = 0; j < transform_size; j++) {
					color_pixels2[j][transform_size - 1 - i].r =
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].r;
					color_pixels2[j][transform_size - 1 - i].g =
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].g;
					color_pixels2[j][transform_size - 1 - i].b =
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].b;
				}

			for (int i = 0; i < transform_size; i++)
				for (int j = 0; j < transform_size; j++) {
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].r =
					color_pixels2[i][j].r;
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].g =
					color_pixels2[i][j].g;
					imagine->color_pixels[i + imagine->y1][j + imagine->x1].b =
					color_pixels2[i][j].b;
				}

			break;
		}
		unghi -= 90;
	}
	// free the memory if needed
	if (grey_pixels2) {
		for (int i = 0; i < transform_size; i++)
			free(grey_pixels2[i]);
		free(grey_pixels2);
	}

	if (color_pixels2) {
		for (int i = 0; i < transform_size; i++)
			free(color_pixels2[i]);
		free(color_pixels2);
	}
}

void rotate(struct image *imagine, int unghi)
{
	int input_angle = unghi;
	if (imagine->is_image_loaded == 0) {// check if the image is loaded
		printf("No image loaded\n");
		return;
	}
	if (unghi % 90 != 0) {// check if the angle is valid
		printf("Unsupported rotation angle\n");
		return;
	}
	while (unghi <= 0) // convert a negative angle to a positive one
		unghi += 360;
	// check if the whole image needs to be rotated
	if (imagine->x1 == 0 &&
		imagine->y1 == 0 &&
		imagine->x2 == imagine->size_x &&
		imagine->y2 == imagine->size_y) {
		if (unghi == 360) { // if the angle is of 360 degrees the image will not be rotated
			printf("Rotated %d\n", input_angle);
			return;
		}
		rotate_all(imagine, unghi);
		printf("Rotated %d\n", input_angle);
	} else { // check if we rotate only a specific area
		if ((imagine->x1 - imagine->x2) != (imagine->y1 - imagine->y2)) {
			printf("The selection must be square\n");
			return;
		}
		rotate_selected(imagine, unghi);
		printf("Rotated %d\n", input_angle);
	}
}