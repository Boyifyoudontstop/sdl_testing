#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class SDLApp {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Mix_Music* music;
    Mix_Chunk* soundEffect;
    SDL_Texture* image;
    bool running;

public:
    SDLApp() : window(nullptr), renderer(nullptr), font(nullptr), 
               music(nullptr), soundEffect(nullptr), image(nullptr), running(true) {}

    bool init() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            printf("SDL initialization failed: %s\n", SDL_GetError());
            return false;
        }

        // Initialize SDL_image
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            printf("SDL_image initialization failed: %s\n", IMG_GetError());
            return false;
        }

        // Initialize SDL_mixer
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
            return false;
        }

        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            printf("SDL_ttf initialization failed: %s\n", TTF_GetError());
            return false;
        }

        // Create window
        window = SDL_CreateWindow("SDL Multimedia Example", 
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            printf("Window creation failed: %s\n", SDL_GetError());
            return false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            printf("Renderer creation failed: %s\n", SDL_GetError());
            return false;
        }

        return loadMedia();
    }

    bool loadMedia() {
        // Load font
        font = TTF_OpenFont("resources/Times New Roman.ttf", 28);
        if (!font) {
            printf("Failed to load font: %s\n", TTF_GetError());
            return false;
        }

        // Load music
        music = Mix_LoadMUS("resources/monkdev.mp3");
        if (!music) {
            printf("Failed to load music: %s\n", Mix_GetError());
            return false;
        }

        // Load sound effect
        soundEffect = Mix_LoadWAV("resources/btn.wav");
        if (!soundEffect) {
            printf("Failed to load sound effect: %s\n", Mix_GetError());
            return false;
        }

        // Load image
        SDL_Surface* loadedSurface = IMG_Load("resources/rev.jpg");
        if (!loadedSurface) {
            printf("Failed to load image: %s\n", IMG_GetError());
            return false;
        }
        
        image = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
        if (!image) {
            printf("Failed to create texture: %s\n", SDL_GetError());
            return false;
        }

        return true;
    }

    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_p:
                        // Play/Pause music
                        if (!Mix_PlayingMusic()) {
                            Mix_PlayMusic(music, -1);
                        }
                        else {
                            if (Mix_PausedMusic()) {
                                Mix_ResumeMusic();
                            }
                            else {
                                Mix_PauseMusic();
                            }
                        }
                        break;
                    case SDLK_s:
                        // Play sound effect
                        Mix_PlayChannel(-1, soundEffect, 0);
                        break;
                }
            }
        }
    }

    void render() {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Render image
        SDL_Rect imageRect = {50, 50, 200, 200};
        SDL_RenderCopy(renderer, image, NULL, &imageRect);

        // Render text
        SDL_Color textColor = {0, 0, 0};
        std::string text = "Press P to play/pause music, S for sound effect";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        
        SDL_Rect textRect = {50, SCREEN_HEIGHT - 100, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    void run() {
        while (running) {
            handleEvents();
            render();
            SDL_Delay(16); // Cap at ~60 FPS
        }
    }

    void cleanup() {
        SDL_DestroyTexture(image);
        Mix_FreeChunk(soundEffect);
        Mix_FreeMusic(music);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }
};

int main(int argc, char* args[]) {
    SDLApp app;
    
    if (!app.init()) {
        printf("Failed to initialize!\n");
        return 1;
    }
    
    app.run();
    app.cleanup();
    
    return 0;
}
