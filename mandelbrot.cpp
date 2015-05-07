#include <cstdio>
#include <cstdint>
#include <SDL/SDL.h>
#include <pthread.h>

const int THREADS  = 4;
const int SCR_WDTH = 800;
const int SCR_HGHT = 600;
const int SCR_CD   = 24;
const int ITERATS  = 1000;

struct pixel{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 alpha;

    pixel(){
        r     = 0;
        g     = 0;
        b     = 0;
        alpha = 255;
    }

    template<typename TYP>
        pixel(TYP red, TYP gre, TYP blu, TYP alp){
            r     = red % 255;
            g     = gre % 255;
            b     = blu % 255;
            alpha = alp % 255;
        }
};

pixel colorTable[256];
void generateColorTable(){
    srand(time(0));
    for(int i = 0; i < 256; i++){
        colorTable[i].r = rand() % 255;
        colorTable[i].g = rand() % 255;
        colorTable[i].b = rand() % 255;
    }
}

struct rendThrData{
    static uint8_t next_id;
    const uint8_t id;
    double        xmin;
    double        xmax;
    double        ymin;
    double        ymax;
    SDL_Surface*  img;

    rendThrData()
        :id(next_id++){}
    ~rendThrData(){
        SDL_FreeSurface(img);
    }
} thrdata[THREADS];
uint8_t rendThrData::next_id = 0;

void put_px(SDL_Surface* scr, int x, int y, pixel* p){
    Uint32* p_screen = (Uint32*)scr->pixels;
    p_screen  += y * scr->w + x;
    *p_screen  = SDL_MapRGBA(scr->format, p->r, p->g, p->b,
            p->alpha);
}



int main(){
    pthread_t thrds[THREADS];
    SDL_Surface* screen;

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(SCR_WDTH, SCR_HGHT, SCR_CD, SDL_SWSURFACE);
    for(int i = 0; i < ITERATS; i++){
        SDL_LockSurface(screen);
        SDL_UnlockSurface(screen);
        SDL_Flip(screen);
    }
    SDL_Quit();
}
