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

int main(int argc, char **Args)
{
  if (setup_SDL(512, 512))
  {
    cerr << "could not load SDL" << endl;
    return 1;
  }

  cout << "Loaded SDL" << endl;

  setup_devil();
  
  SDL_RenderClear(app_renderer);
  SDL_SetRenderDrawColor(app_renderer, 255, 0, 128, 0);
  //SDL_RenderCopy(app_renderer, tex, NULL, NULL);
  
  SDL_Texture* tex = load_texture("/home/mason/51415.jpg");
  if (tex == NULL)
  {
    cerr << "Could not load texture" << endl;
    return 1;
  }

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
      if (e.type == SDL_KEYDOWN)
        quit = true;
        
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
 
