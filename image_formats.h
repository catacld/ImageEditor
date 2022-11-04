#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum image_type {
	P2,
	P3,
	P5,
	P6
};

struct color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct image {
	bool is_image_loaded;
	enum image_type type; // used to remember the image type
	int max_value;
	// used for grayscale images
	unsigned char **grey_pixels;
	struct color **color_pixels; // used for color images

	int size_x; // the size of the image
	int size_y;

	int x1; // variables used to remember the selected zone
	int y1;
	int x2;
	int y2;
};

void free_content(struct image *imagine)
{	// free the space if needed
	if (imagine->grey_pixels) {
		for (int i = 0; i < imagine->size_y; i++)
			if (imagine->grey_pixels[i])
				free(imagine->grey_pixels[i]);
		free(imagine->grey_pixels);
		imagine->grey_pixels = NULL;
	}

	if (imagine->color_pixels) {
		for (int i = 0; i < imagine->size_y; i++)
			if (imagine->color_pixels[i])
				free(imagine->color_pixels[i]);
		free(imagine->color_pixels);
		imagine->color_pixels = NULL;
	}
	imagine->is_image_loaded = false;
}

// function to load the read image
bool load_image(struct image *imagine, char *nume_fisier)
{
	free_content(imagine);
	// check if the file is valid
	FILE *fisier;
	if (!nume_fisier)
		return 0;
	fisier = fopen(nume_fisier, "r");
	if (!fisier)
		return 0;

	int val;

	// read magic word
	val = getc(fisier);
	if (val != 'P') {
		fclose(fisier);
		return 0;
	}

	val = getc(fisier);
	switch ((char)val) {
	case '2':
		imagine->type = P2;
		break;

	case '3':
		imagine->type = P3;
		break;

	case '5':
		imagine->type = P5;
		break;

	case '6':
		imagine->type = P6;
		break;

	default:
		fclose(fisier);
		return 0;
	}
	val = getc(fisier);

	// read size and max_value
	char read_line[255];
	fgets(read_line, 255, fisier);
	while (read_line[0] == '#')
		fgets(read_line, 255, fisier);
	if (sscanf(read_line, "%d %d", &imagine->size_x, &imagine->size_y) <= 0)
		return 0;

	fgets(read_line, 255, fisier);
	while (read_line[0] == '#')
		fgets(read_line, 255, fisier);
	if (sscanf(read_line, "%d", &imagine->max_value) <= 0)
		return 0;
	// read depending on the format of the file : binary/ascii
	switch (imagine->type) {
	// read for a binary file
	case P2:
		imagine->grey_pixels =
		(unsigned char **)malloc(imagine->size_y * sizeof(unsigned char *));
		for (int i = 0; i < imagine->size_y; i++)
			imagine->grey_pixels[i] =
			(unsigned char *)malloc(imagine->size_x * sizeof(unsigned char));

		for (int i = 0; i < imagine->size_y; i++)
			for (int j = 0; j < imagine->size_x; j++)
				if (fscanf(fisier, "%hhu", &imagine->grey_pixels[i][j])
				    == EOF) {
					fclose(fisier);
					return 0;
				}
		break;

	case P3:
		imagine->color_pixels =
		(struct color **)malloc(imagine->size_y * sizeof(struct color *));
		for (int i = 0; i < imagine->size_y; i++)
			imagine->color_pixels[i] =
			(struct color *)malloc(imagine->size_x * sizeof(struct color));

		for (int i = 0; i < imagine->size_y; i++)
			for (int j = 0; j < imagine->size_x; j++) {
				if (fscanf
				(fisier, "%hhu", &imagine->color_pixels[i][j].r)
				== EOF) {
					fclose(fisier);
					return 0;
				}
				if (fscanf(fisier, "%hhu", &imagine->color_pixels[i][j].g)
					== EOF) {
					fclose(fisier);
					return 0;
				}
				if (fscanf(fisier, "%hhu", &imagine->color_pixels[i][j].b) ==
					EOF) {
					fclose(fisier);
					return 0;
				}
			}
		break;

	case P5:
		imagine->grey_pixels =
		(unsigned char **)malloc(imagine->size_y * sizeof(unsigned char *));
		for (int i = 0; i < imagine->size_y; i++)
			imagine->grey_pixels[i] =
			(unsigned char *)malloc(imagine->size_x * sizeof(unsigned char));

		for (int i = 0; i < imagine->size_y; i++)
			for (int j = 0; j < imagine->size_x; j++) {
				val = getc(fisier);
				if (val == EOF) {
					fclose(fisier);
					return 0;
				}
				imagine->grey_pixels[i][j] = (unsigned char)val;
			}
		break;

	case P6:
		imagine->color_pixels =
		(struct color **)malloc(imagine->size_y * sizeof(struct color *));
		for (int i = 0; i < imagine->size_y; i++)
			imagine->color_pixels[i] =
			(struct color *)malloc(imagine->size_x * sizeof(struct color));
		for (int i = 0; i < imagine->size_y; i++)
			for (int j = 0; j < imagine->size_x; j++) {
				val = getc(fisier);
				if (val == EOF) {
					fclose(fisier);
					return 0;
				}
				imagine->color_pixels[i][j].r = (unsigned char)val;
				val = getc(fisier);
				if (val == EOF) {
					fclose(fisier);
					return 0;
				}
				imagine->color_pixels[i][j].g = (unsigned char)val;

				val = getc(fisier);
				if (val == EOF) {
					fclose(fisier);
					return 0;
				}
				imagine->color_pixels[i][j].b = (unsigned char)val;
			}
		break;
	}
	fclose(fisier);
	imagine->is_image_loaded = 1;
	imagine->x1 = 0;
	imagine->y1 = 0;
	imagine->x2 = imagine->size_x;
	imagine->y2 = imagine->size_y;
	return 1;
	}

bool save_image(struct image *imagine, char *nume_fisier, bool is_ascii)
{	// check if an image is loaded
	if (imagine->is_image_loaded == 0) {
		printf("No image loaded\n");
		return 0;
	}
	// check if the file is valid
	FILE *fisier;
	if (!nume_fisier)
		return 0;
	fisier = fopen(nume_fisier, "w");
	if (!fisier)
		return 0;

	switch (imagine->type) {
	// read for an ascii file
	case P2:
	case P5:
		if (is_ascii == 1)
			fprintf(fisier, "P2\n");
		else
			fprintf(fisier, "P5\n");

		fprintf(fisier, "%d %d\n%d\n",
				imagine->size_x, imagine->size_y, imagine->max_value);

		if (is_ascii == 0) {
			for (int i = 0; i < imagine->size_y; i++)
				for (int j = 0; j < imagine->size_x; j++)
					putc(imagine->grey_pixels[i][j], fisier);
		} else {
			for (int i = 0; i < imagine->size_y; i++) {
				for (int j = 0; j < imagine->size_x; j++)
					fprintf(fisier, "%hhu ", imagine->grey_pixels[i][j]);
				fprintf(fisier, "\n");
			}
		}
		break;
	case P3:
	case P6:
		if (is_ascii == 1)
			fprintf(fisier, "P3\n");
		else
			fprintf(fisier, "P6\n");

		fprintf(fisier, "%d %d\n%d\n",
				imagine->size_x, imagine->size_y, imagine->max_value);

		if (is_ascii == 0) {
			for (int i = 0; i < imagine->size_y; i++) {
				for (int j = 0; j < imagine->size_x; j++) {
					putc(imagine->color_pixels[i][j].r, fisier);
					putc(imagine->color_pixels[i][j].g, fisier);
					putc(imagine->color_pixels[i][j].b, fisier);
				}
			}
		} else {
			for (int i = 0; i < imagine->size_y; i++) {
				for (int j = 0; j < imagine->size_x; j++)
					fprintf(fisier, "%hhu %hhu %hhu ",
							imagine->color_pixels[i][j].r,
							imagine->color_pixels[i][j].g,
							imagine->color_pixels[i][j].b);
				fprintf(fisier, "\n");
			}
		}
		break;
	}
	printf("Saved %s\n", nume_fisier);
	fclose(fisier);
	return 1;
}