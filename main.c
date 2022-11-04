#include "image_transform.h"

int main(void)
{
	struct image imagine;
	imagine.color_pixels = NULL;
	imagine.grey_pixels = NULL;
	imagine.is_image_loaded = 0;

	char argument[255];
	memset(argument, 0, 255);

	while (fgets(argument, 255, stdin)) {
		// use strstr to get the input command
		if (strstr(argument, "LOAD")) {
			char nume_fisier[255];
			// check if the command was successful
			if (sscanf(argument + 5, "%s", nume_fisier) > 0) {
				if (load_image(&imagine, nume_fisier) == 0)
					printf("Failed to load %s\n", nume_fisier);
				else
					printf("Loaded %s\n", nume_fisier);
			}
		} else if (strstr(argument, "SELECT")) {
			// case "ALL" - select the entire image
			if (strstr(argument, "ALL")) {
				select_all(&imagine);
			} else {
				int x1, x2, y1, y2;
				// select only an area of the image
				// only if the coordinates are valid
				if (sscanf(argument + 7, "%d %d %d %d", &x1, &y1, &x2, &y2)
					!= 4)
					printf("Invalid command\n");
				else
					select_coords(&imagine, x1, x2, y1, y2);
			}
		} else if (strstr(argument, "ROTATE")) { // apply "ROTATE"
			int unghi;
			if (sscanf(argument + 7, "%d", &unghi) > 0)
				rotate(&imagine, unghi);
		} else if (strstr(argument, "CROP")) { // apply "CROP"
			crop(&imagine);
		} else if (strstr(argument, "SAVE")) { // apply "SAVE"
			char nume_fisier[255];
			if (sscanf(argument + 5, "%s", nume_fisier) > 0)
				save_image(&imagine, nume_fisier, (strstr(argument, "ascii")));
		} else if (strstr(argument, "APPLY")) { // apply "APPLY"
			char filtru_text[255];
			memset(filtru_text, 0, 255);
			enum filter filtru = UNDIFINED;
			// select the filter for "APPLY"
			if (sscanf(argument + 6, "%s", filtru_text) > 0) {
				if (strstr(filtru_text, "EDGE"))
					filtru = EDGE;
				else if (strstr(filtru_text, "SHARPEN"))
					filtru = SHARPEN;
				else if (strstr(filtru_text, "GAUSSIAN_BLUR"))
					filtru = GAUSSIAN_BLUR;
				else if (strstr(filtru_text, "BLUR"))
					filtru = BLUR;
			} // check if "APPLY" has been successfully executed
			if (apply_filter(&imagine, filtru) == 1)
				printf("APPLY %s done\n", filtru_text);
		} else if (strstr(argument, "EXIT")) { // apply "EXIT"
			if (imagine.is_image_loaded == 0)
				printf("No image loaded\n");
			free_content(&imagine); // free the memory
			return 0;
		} else if (strlen(argument) > 1) { // invalid command as input
			printf("Invalid command\n");
		}
		memset(argument, 0, 255);
	}

	return 0;
}