#include <stdio.h>
#include <stdlib.h>
#include <gif_lib.h>
#include <png.h>

void write_png(const char *filename, const unsigned char *image, int width, int height) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_bytep row = (png_bytep) malloc(3 * width * sizeof(png_byte));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            row[x*3] = image[(y*width + x)*3];
            row[x*3 + 1] = image[(y*width + x)*3 + 1];
            row[x*3 + 2] = image[(y*width + x)*3 + 2];
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);
    if (png && info)
        png_destroy_write_struct(&png, &info);
    if (fp)
        fclose(fp);
    if (row)
        free(row);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <gif_file_path> <output_folder>\n", argv[0]);
        return 1;
    }

    const char *input_gif_path = argv[1];
    const char *output_folder = argv[2];

    int error = 0;
    GifFileType *gifFile = DGifOpenFileName(input_gif_path, &error);
    if (gifFile == NULL) {
        printf("Error opening GIF file: %s\n", GifErrorString(error));
        return 1;
    }

    if (DGifSlurp(gifFile) != GIF_OK) {
        printf("Error reading GIF file: %s\n", GifErrorString(gifFile->Error));
        return 1;
    }

    char mkdir_command[256];
    snprintf(mkdir_command, sizeof(mkdir_command), "mkdir -p %s", output_folder);
    system(mkdir_command);

    for (int i = 0; i < gifFile->ImageCount; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "%s/frame%04d.png", output_folder, i);

        SavedImage *image = &gifFile->SavedImages[i];
        ColorMapObject *colorMap = image->ImageDesc.ColorMap ? image->ImageDesc.ColorMap : gifFile->SColorMap;

        unsigned char *png_data = malloc(3 * gifFile->SWidth * gifFile->SHeight);
        for (int y = 0; y < gifFile->SHeight; y++) {
            for (int x = 0; x < gifFile->SWidth; x++) {
                int offset = y * gifFile->SWidth + x;
                GifColorType color = colorMap->Colors[image->RasterBits[offset]];
                png_data[offset*3] = color.Red;
                png_data[offset*3 + 1] = color.Green;
                png_data[offset*3 + 2] = color.Blue;
            }
        }

        write_png(filename, png_data, gifFile->SWidth, gifFile->SHeight);
        free(png_data);
    }

    DGifCloseFile(gifFile, &error);

    return 0;
}
