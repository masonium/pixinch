#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cassert>
#include "cluster.h"
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <locale>
#include "arduino-serial-lib.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

SDL_Window* app_window;
SDL_Renderer* app_renderer;

int setup_SDL(int width, int height)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    cerr << "SDL_Init Error: " << SDL_GetError() << endl;
    return 1;
  }
  app_window = SDL_CreateWindow("DPAD Image Transformer", 100, 100,
                                width, height,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (app_window == NULL) {
    cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    return 1;
  }

  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  
  app_renderer = SDL_CreateRenderer(
    app_window, -1, 
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  
  if (app_renderer == NULL) {
    cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
    return 1;
  }

  return 0;
}

int setup_devil()
{
  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
    /* wrong DevIL version */
    return -1;
  }

  ilInit();
  iluInit();
  return 0;
}

void write_image_as_header(ILuint image, const ColorClusterer &clusterer)
{
  ilBindImage(image);
  int w = ilGetInteger(IL_IMAGE_WIDTH);
  int h = ilGetInteger(IL_IMAGE_HEIGHT);
  uchar* data = ilGetData();
  
  int nc = clusterer.clusters();
  printf("#pragma once\n");
  printf("#include <inttypes.h>\n");
  printf("\n");
  printf("#define PALETTE_SIZE %d\n", nc);
  printf("uint32_t palette_data[PALETTE_SIZE] = {");

  const int entries_per_row = 8;
  for (int i = 0, display_row = 0; i < nc; ++i, display_row = (display_row + 1) % entries_per_row)
  {
    if (display_row == 0)
      printf("\n/* %3d-%3d*/ ", i, std::min(i + entries_per_row-1, nc-1));
    ColorKey ck = clusterer[i];
    uchar r, g, b;
    ck.to_rgb(r, g, b);
    printf("0x%02x%02x%02x", r, g, b);
    if (i != nc - 1)
      printf(", ");
  }
  printf("\n};\n\n");
  
  printf("uint8_t pixel_data[%d][%d] = {", h, w);
  for (int y = h-1; y >= 0; --y)
  {
    printf("\n{");
    for (int x = 0; x < w; ++x)
    {
      int i = y * w + x;
      uchar r = data[3*i], g = data[3*i + 1], b = data[3*i + 2];
      
      int index = clusterer.get_index(r, g, b);
      
      printf("%d", index);
      if (x != w - 1)
        printf(", ");
    }
    printf("}");
    if (y != 0)
      printf(",");
  }
  printf("\n};\n");
}

ILuint load_image(const char* filename)
{
  ILuint tex;
  ilGenImages(1, &tex);
  ilBindImage(tex);

  bool success = ilLoadImage(filename);

  if (!success)
  {
    cerr << "Could not load image: " << filename << endl;
    return 0;
  }

  cerr << "Loaded " << filename << endl;
  
  success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
  if (!success)
  {
    cerr << "Could not convert image to R8G8B8" << endl;
  }

  cerr << "converted image" << endl;

  return tex;
}

ILuint resize_image(ILuint image, ColorClusterer& clusterer)
{
  ILuint new_image;
  ilGenImages(1, &new_image);
  ilBindImage(new_image);

  ilCopyImage(image);

  iluScale(32, 32, 8);
  
  uchar* pixels = ilGetData();

  int img_width = 32;
  for (int y = 0; y < 32; ++y) {
    for (int x = 0; x < 32; ++x) {
      uchar* offset = &pixels[3 * img_width * y + 3 * x];
      clusterer.add_pixel(offset[0], offset[1], offset[2]);
    }
  }
  
  clusterer.update_clusters();
  
  return new_image;
}

ILuint posterize_image(ILuint image, const ColorClusterer& clusterer)
{
  ILuint new_image;
  ilGenImages(1, &new_image);
  ilBindImage(new_image);

  ilCopyImage(image);

  iluScale(32, 32, 8);
  
  uchar* pixels = ilGetData();

  int img_width = 32;
  for (int y = 0; y < 32; ++y) {
    for (int x = 0; x < 32; ++x) {
      uchar* offset = &pixels[3 * img_width * y + 3 * x];

      int color_index = clusterer.get_index(offset[0], offset[1], offset[2]);
      clusterer[color_index].to_rgb(offset[0], offset[1], offset[2]);
    }
  }
  
  return new_image;
}

void write_and_ack(int fd, uint8_t b)
{
  serialport_writebyte(fd, b);
  char buffer[256];
  serialport_read_until(fd, buffer, '\n', 255, 1000);
}

void send_image(ILuint image, const ColorClusterer& clusterer)
{
  int fd;
  if ((fd = serialport_init("/dev/ttyACM0", 9600)) < 0)
    cerr << "Could not open arduino port." << endl;

  
  cerr << "Opening. Waiting 1 seconds." << endl;
  sleep(1);

  uchar num_clusters = clusterer.clusters();
  write_and_ack(fd, num_clusters);
  
  for (int i = 0; i < num_clusters; ++i)
  {
    uchar r, g, b;
    clusterer[i].to_rgb(r, g, b);
    write_and_ack(fd, r);
    write_and_ack(fd, g);
    write_and_ack(fd, b);
    usleep(10000);
  }
  
  const uchar* data = ilGetData();
  for (int i = 0; i < 1024; ++i)
  {
    const uchar* offset = &data[3*i];
    uchar index = clusterer.get_index(offset[0], offset[1], offset[2]);
    write_and_ack(fd, index);
    usleep(1000);
  };

  cerr << "Wrote image." << endl;
  serialport_close(fd);
}

SDL_Texture* make_sdl_texture(ILuint image_tex)
{
  
  int img_width = ilGetInteger(IL_IMAGE_WIDTH);
  int img_height = ilGetInteger(IL_IMAGE_HEIGHT);
  
  SDL_Texture* sdl_tex = SDL_CreateTexture(app_renderer, SDL_PIXELFORMAT_RGB24,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           img_width, img_height);

  SDL_UpdateTexture(sdl_tex, NULL, ilGetData(), img_width * 3);
  
  return sdl_tex;
}

int main(int argc, char **args)
{
  setup_devil();
  string command("");
  if (argc > 1)
  {
    command = args[1];
    transform(command.begin(), command.end(), command.begin(), tolower);
  }

  // non-interactive c++ header generation
  if (command == "source")
  {
    ILuint image = load_image( args[2] );
    ColorClusterer clusterer(255);
    ILuint resized_image = resize_image( image, clusterer );

    write_image_as_header( resized_image, clusterer );
    
    ilDeleteImage(image);
    ilDeleteImage(resized_image);

    return 0;
  }

  if (command == "send")
  {
    ILuint image = load_image( args[2] );
    ColorClusterer clusterer(255);
    ILuint resized_image = resize_image( image, clusterer );

    send_image( resized_image, clusterer );
    
    ilDeleteImage(image);
    ilDeleteImage(resized_image);

    return 0;
    
  }
  
  if (setup_SDL(512, 512))
  {
    cerr << "could not load SDL" << endl;
    return 1;
  }

  cerr << "Loaded SDL" << endl;
  
  SDL_RenderClear(app_renderer);
  SDL_SetRenderDrawColor(app_renderer, 63, 63, 63, 0);
  
  SDL_Texture* tex = NULL;

  bool quit = false;

  ColorClusterer cluster(255);
  
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_DROPFILE)
      {
        const char* filename = e.drop.file;

        ILuint image = load_image( filename );
        cluster.clear();
        ILuint ri = resize_image( image, cluster );
        ILuint posterized_image = posterize_image( ri, cluster );
        
        SDL_Texture* new_tex = make_sdl_texture(posterized_image);

        ilDeleteImage(image);
        ilDeleteImage(ri);
        ilDeleteImage(posterized_image);
        
        if (new_tex) {
          SDL_DestroyTexture(tex);
          tex = new_tex;
          cerr << "changed texture" << endl;
        }
        else {
          cerr << "Could not load texture" << endl;
        }
      }
      
      if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          quit = true;
        }
      }
        
      if (e.type == SDL_QUIT)
        quit = true;
    }
  
    SDL_RenderClear(app_renderer);
    SDL_RenderCopy(app_renderer, tex, NULL, NULL);
    SDL_RenderPresent(app_renderer);
  }


  SDL_DestroyTexture(tex);

  SDL_DestroyRenderer(app_renderer);
  SDL_DestroyWindow(app_window);
  SDL_Quit();
}
 
