# animated-splash-miyoo
Animated SDL1.2 splash screen &amp; GIF Deconstructor sister app

# Example


https://github.com/XK9274/animated-splash-miyoo/assets/47260768/3dace5c3-8116-4076-b944-a97606b43588


## Remember to set your LD_LIBRARY_PATH to where you store libpng, libsdl, libgif etc.

# rapid-splash overview:

Will display a series of .png files as an animation, rotozoom isn't used as it's super slow for this application 

 ## Library requirements
```
libSDL_gfx.so.15
libSDL_image-1.2.so.0
libSDL-1.2.so.0
```

## Usage

Accepts the image path (path to where all your frames are stored, eg. frame0001, frame0002 (in this format, use the deconstructor)
Accepts a background image colour (incase your gif isn't 640x480, you can fill the bg with a colour)
Accepts the frame duration (per gif, you'll need to figure this out)

`Usage: %s <image_path> <red> <green> <blue> <frame_duration> [debug]`

### Example
`/mnt/SDCARD/App/ncspot/bin/rapid-splash /mnt/SDCARD/App/ncspot/bin/frames 0 0 0 50 `

# gif-deconstructor overview:

Will deconstruct a gif into it's frames, accepts a gif and a destination (will create the path & dir if it doesn't exist)

## Library requirements
```
libgif.so.7
libpng12.so.0
```

## Usage
Accepts a path to a gif
Accepts a path to output to 

`Usage: %s <gif_file_path> <output_folder>`

### Example
`# ./deconstructor ../logos/spotify-taiga.gif ./gifdump`

