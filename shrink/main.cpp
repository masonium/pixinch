#include <iostream>
#include <SDL2/SDL.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <cassert>

using std::cerr;
using std::cout;
using std::endl;

SDL_Window* app_window;
SDL_Renderer* app_renderer;

int setup_SDL(int width, int height)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    cerr << "SDL_Init Error: " << SDL_GetError() << endl;
    return 1;
  }
  app_window = SDL_CreateWindow("Hello World!", 100, 100,
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
  return 0;
}


SDL_Texture* load_texture(const char * filename)
{
  ILuint tex;
  ilGenImages(1, &tex);
  ilBindImage(tex);

  cout << "in load texture" << endl;
  
  bool success = ilLoadImage(filename);

  if (!success)
  {
    cerr << "Could not load image: " << filename << endl;
    return NULL;
  }

  cout << "Loaded image" << endl;
  
  success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
  if (!success)
  {
    cerr << "Could not convert image to R8G8B8" << endl;
  }

  cerr << "converted image" << endl;

  iluScale(32, 32, 8);
  
  int img_width = ilGetInteger(IL_IMAGE_WIDTH);
  int img_height = ilGetInteger(IL_IMAGE_HEIGHT);

  cout << img_width << ", " << img_height << endl;
  
  SDL_Texture* sdl_tex = SDL_CreateTexture(app_renderer, SDL_PIXELFORMAT_RGB24,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           img_width, img_height);

  SDL_UpdateTexture(sdl_tex, NULL, ilGetData(), img_width * 3);

  ilDeleteImages(1, &tex);
  
  return sdl_tex;
}

void write_image(SDL_Texture* tex)
{
  if (tex == NULL)
    return;

  int w, h;
  SDL_QueryTexture(tex, NULL, NULL, &w, &h);
  
  char* pixels;
  int pixel_pitch;
  SDL_LockTexture(tex, NULL, reinterpret_cast<void**>(&pixels), &pixel_pitch);

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
    }
  }      
}

int main(int argc, char **args)
{
  bool write_images = false;
  if (argc > 1)
  {
    write_images = true;
  }
  
  if (setup_SDL(512, 512))
  {
    cerr << "could not load SDL" << endl;
    return 1;
  }

  cerr << "Loaded SDL" << endl;

  setup_devil();
  
  SDL_RenderClear(app_renderer);
  SDL_SetRenderDrawColor(app_renderer, 63, 63, 63, 0);
  
  SDL_Texture* tex = NULL;

  bool quit = false;

  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_DROPFILE)
      {
        const char* filename = e.drop.file;
        SDL_Texture* new_tex = load_texture(filename);
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
        if (e.key.keysym.sym == SDLK_w && write_images) {
          write_image(tex);
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
 
