#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#include <stdio.h>
#include <stdlib.h>

SDL_Surface* nextFrame = NULL;
int nextFrameNumber = 0;
const char* image_path;
int debugging = 0;

SDL_Surface* rotate180(SDL_Surface* src) { // custom rotate, rotozoom is very slow.
    if (debugging) {
        fprintf(stderr, "Entering rotate180\n");
    }

    if (src == NULL) {
        if (debugging) {
            fprintf(stderr, "Source surface is NULL!\n");
        }
        return NULL;
    }

    SDL_Surface* dest = SDL_CreateRGBSurface(0, src->w, src->h, 32, 0, 0, 0, 0);
    if (dest == NULL) {
        if (debugging) {
            fprintf(stderr, "Could not create destination surface! SDL Error: %s\n", SDL_GetError());
        }
        return NULL;
    }

    SDL_Surface* optimizedSrc = SDL_ConvertSurface(src, dest->format, 0);
    if (optimizedSrc == NULL) {
        if (debugging) {
            fprintf(stderr, "Could not optimize source surface! SDL Error: %s\n", SDL_GetError());
        }
        return NULL;
    }

    if (SDL_MUSTLOCK(optimizedSrc)) SDL_LockSurface(optimizedSrc);
    if (SDL_MUSTLOCK(dest)) SDL_LockSurface(dest);

    Uint32* srcPixels = (Uint32*)optimizedSrc->pixels;
    Uint32* destPixels = (Uint32*)dest->pixels;

    for (int y = 0; y < optimizedSrc->h; ++y) {
        for (int x = 0; x < optimizedSrc->w; ++x) {
            int destIndex = (optimizedSrc->h - y - 1) * optimizedSrc->w + (optimizedSrc->w - x - 1);
            int srcIndex = y * optimizedSrc->w + x;
            destPixels[destIndex] = srcPixels[srcIndex];
        }
    }

    if (SDL_MUSTLOCK(optimizedSrc)) SDL_UnlockSurface(optimizedSrc);
    if (SDL_MUSTLOCK(dest)) SDL_UnlockSurface(dest);

    SDL_FreeSurface(optimizedSrc);

    if (debugging) {
        fprintf(stderr, "Exiting rotate180\n");
    }
    return dest;
}


int load_next_frame(void* data) {
    if (debugging) {
        fprintf(stderr, "Entering load_next_frame\n");
    }
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/frame%04d.png", image_path, nextFrameNumber);
    SDL_Surface* frame = IMG_Load(filename);
    
    if (frame == NULL) {
        if (debugging) {
            fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
        }
        return 1;
    } else {
        if (debugging) {
            fprintf(stderr, "Image loaded successfully.\n");
        }
    }
    
    if (debugging) {
        fprintf(stderr, "Calling rotate180 from load_next_frame\n");
    }
    nextFrame = rotate180(frame);
    
    if (nextFrame == NULL) {
        if (debugging) {
            fprintf(stderr, "rotate180 failed. Exiting load_next_frame.\n");
        }
        SDL_FreeSurface(frame);
        return 1;
    }

    SDL_FreeSurface(frame);
    
    if (debugging) {
        fprintf(stderr, "Exiting load_next_frame\n");
    }
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc < 6) {
        printf("Usage: %s <image_path> <red> <green> <blue> <frame_duration> [debug]\n", argv[0]);
        return 1;
    }

    image_path = argv[1];
    int red = atoi(argv[2]);
    int green = atoi(argv[3]);
    int blue = atoi(argv[4]);
    int frameDuration = atoi(argv[5]);

    if (argc > 6 && atoi(argv[6]) == 1) {
        debugging = 1; // if you set 1 at the end of the args it'll enable debugging (uart output)
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (screen == NULL) {
        printf("Could not create screen! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    nextFrameNumber = 1;

    SDL_Thread* thread = SDL_CreateThread(load_next_frame, NULL);
    if (thread == NULL) {
        fprintf(stderr, "Failed to create thread: %s\n", SDL_GetError());
    } else {
        fprintf(stderr, "Thread created successfully.\n");
    }

    int i = 1;
    while (1) {
        if (debugging) {
            fprintf(stderr, "Waiting for thread to complete...\n");
        }
        SDL_WaitThread(thread, NULL);
        if (debugging) {
            fprintf(stderr, "Thread completed.\n");
        }

        if (nextFrame == NULL) {
            if (debugging) {
                fprintf(stderr, "No more frames to display or error occurred!\n");
            }
            break;
        }

        if (debugging) {
            fprintf(stderr, "Filling rect with color...\n");
        }
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, red, green, blue));

        SDL_Rect dstRect;
        dstRect.x = (screen->w - nextFrame->w) / 2;
        dstRect.y = (screen->h - nextFrame->h) / 2;

        if (debugging) {
            fprintf(stderr, "Blitting surface...\n");
        }
        SDL_BlitSurface(nextFrame, NULL, screen, &dstRect);
        if (debugging) {
            fprintf(stderr, "Flipping screen...\n");
        }
        SDL_Flip(screen);

        if (debugging) {
            fprintf(stderr, "Freeing nextFrame surface...\n");
        }
        SDL_FreeSurface(nextFrame);
        nextFrame = NULL;

        nextFrameNumber = ++i;

        if (debugging) {
            fprintf(stderr, "Creating new thread for next frame...\n");
        }
        thread = SDL_CreateThread(load_next_frame, NULL);

        if (debugging) {
            fprintf(stderr, "Delaying for frame duration...\n");
        }
        SDL_Delay(frameDuration);
        if (debugging) {
            fprintf(stderr, "Continuing to next iteration...\n");
        }
    }

    SDL_Quit();
    if (debugging) {
        fprintf(stderr, "Exiting main\n");
    }
    return 0;
}
