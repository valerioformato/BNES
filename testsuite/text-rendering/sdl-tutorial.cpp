//
// Created by vformato on 12/20/25.
//
/*This source code copyrighted by Lazy Foo' Productions 2004-2025
and may not be redistributed without written permission.*/

/* Headers */
// Using SDL, SDL_image, SDL_ttf, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// #include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

/* Constants */
// Screen dimension constants
constexpr int kScreenWidth{640};
constexpr int kScreenHeight{480};

/* Class Prototypes */
class LTexture {
public:
  // Symbolic constant
  static constexpr float kOriginalSize = -1.f;

  // Initializes texture variables
  LTexture();

  // Cleans up texture variables
  ~LTexture();

  // Loads texture from disk
  bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
  // Creates texture from text
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

  // Cleans up texture
  void destroy();

  // Sets color modulation
  void setColor(Uint8 r, Uint8 g, Uint8 b);

  // Sets opacity
  void setAlpha(Uint8 alpha);

  // Sets blend mode
  void setBlending(SDL_BlendMode blendMode);

  // Draws texture
  void render(float x, float y, SDL_FRect *clip = nullptr, float width = kOriginalSize, float height = kOriginalSize,
              double degrees = 0.0, SDL_FPoint *center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

  // Gets texture attributes
  int getWidth();
  int getHeight();
  bool isLoaded();

private:
  // Contains texture data
  SDL_Texture *mTexture;

  // Texture dimensions
  int mWidth;
  int mHeight;
};

/* Function Prototypes */
// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

/* Global Variables */
// The window we'll be rendering to
SDL_Window *gWindow{nullptr};

// The renderer used to draw to the window
SDL_Renderer *gRenderer{nullptr};

// Global font
TTF_Font *gFont{nullptr};

// The texture we're going to render text to
LTexture gTextTexture;

/* Class Implementations */
// LTexture Implementation
LTexture::LTexture()
    : // Initialize texture variables
      mTexture{nullptr}, mWidth{0}, mHeight{0} {}

LTexture::~LTexture() {
  // Clean up texture
  destroy();
}

// bool LTexture::loadFromFile(std::string path) {
//   // Clean up texture if it already exists
//   destroy();
//
//   // Load surface
//   if (SDL_Surface *loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr) {
//     SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
//   } else {
//     // Color key image
//     if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x00, 0xFF, 0xFF)) == false) {
//       SDL_Log("Unable to color key! SDL error: %s", SDL_GetError());
//     } else {
//       // Create texture from surface
//       if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr) {
//         SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
//       } else {
//         // Get image dimensions
//         mWidth = loadedSurface->w;
//         mHeight = loadedSurface->h;
//       }
//     }
//
//     // Clean up loaded surface
//     SDL_DestroySurface(loadedSurface);
//   }
//
//   // Return success if texture loaded
//   return mTexture != nullptr;
// }

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
  // Clean up existing texture
  destroy();

  // Load text surface
  if (SDL_Surface *textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), 0, textColor);
      textSurface == nullptr) {
    SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
  } else {
    // Create texture from surface
    if (mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface); mTexture == nullptr) {
      SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    } else {
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    // Free temp surface
    SDL_DestroySurface(textSurface);
  }

  // Return success if texture loaded
  return mTexture != nullptr;
}
#endif

void LTexture::destroy() {
  // Clean up texture
  SDL_DestroyTexture(mTexture);
  mTexture = nullptr;
  mWidth = 0;
  mHeight = 0;
}

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b) { SDL_SetTextureColorMod(mTexture, r, g, b); }

void LTexture::setAlpha(Uint8 alpha) { SDL_SetTextureAlphaMod(mTexture, alpha); }

void LTexture::setBlending(SDL_BlendMode blendMode) { SDL_SetTextureBlendMode(mTexture, blendMode); }

void LTexture::render(float x, float y, SDL_FRect *clip, float width, float height, double degrees, SDL_FPoint *center,
                      SDL_FlipMode flipMode) {
  // Set texture position
  SDL_FRect dstRect{x, y, static_cast<float>(mWidth), static_cast<float>(mHeight)};

  // Default to clip dimensions if clip is given
  if (clip != nullptr) {
    dstRect.w = clip->w;
    dstRect.h = clip->h;
  }

  // Resize if new dimensions are given
  if (width > 0) {
    dstRect.w = width;
  }
  if (height > 0) {
    dstRect.h = height;
  }

  // Render texture
  SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }

bool LTexture::isLoaded() { return mTexture != nullptr; }

/* Function Implementations */
bool init() {
  // Initialization flag
  bool success{true};

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    success = false;
  } else {
    // Create window with renderer
    if (SDL_CreateWindowAndRenderer("SDL3 Tutorial: True Type Fonts", kScreenWidth, kScreenHeight, 0, &gWindow,
                                    &gRenderer) == false) {
      SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
      success = false;
    } else {
      // Initialize font loading
      if (TTF_Init() == false) {
        SDL_Log("SDL_ttf could not initialize! SDL_ttf error: %s\n", SDL_GetError());
        success = false;
      }
    }
  }

  return success;
}

bool loadMedia() {
  // File loading flag
  bool success{true};

  // Load scene font
  std::string fontPath{"08-true-type-fonts/lazy.ttf"};
  if (gFont = TTF_OpenFont(fontPath.c_str(), 28); gFont == nullptr) {
    SDL_Log("Could not load %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
    success = false;
  } else {
    // Load text
    SDL_Color textColor{0x00, 0x00, 0x00, 0xFF};
    if (gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor) == false) {
      SDL_Log("Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
      success = false;
    }
  }

  return success;
}

void close() {
  // Clean up textures
  gTextTexture.destroy();

  // Free font
  TTF_CloseFont(gFont);
  gFont = nullptr;

  // Destroy window
  SDL_DestroyRenderer(gRenderer);
  gRenderer = nullptr;
  SDL_DestroyWindow(gWindow);
  gWindow = nullptr;

  // Quit SDL subsystems
  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char *args[]) {
  // Final exit code
  int exitCode{0};

  // Initialize
  if (init() == false) {
    SDL_Log("Unable to initialize program!\n");
    exitCode = 1;
  } else {
    // Load media
    if (loadMedia() == false) {
      SDL_Log("Unable to load media!\n");
      exitCode = 2;
    } else {
      // The quit flag
      bool quit{false};

      // The event data
      SDL_Event e;
      SDL_zero(e);

      // The main loop
      while (quit == false) {
        // Get event data
        while (SDL_PollEvent(&e) == true) {
          // If event is quit type
          if (e.type == SDL_EVENT_QUIT) {
            // End the main loop
            quit = true;
          }
        }

        // Fill the background
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render text
        gTextTexture.render((kScreenWidth - gTextTexture.getWidth()) / 2.f,
                            (kScreenHeight - gTextTexture.getHeight()) / 2.f);

        // Update screen
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  // Clean up
  close();

  return exitCode;
}
