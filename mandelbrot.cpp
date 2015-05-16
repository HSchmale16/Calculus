/**\file   mandelbrot.cpp
 * \author Henry J Schmale
 * \date   May 9, 2015
 *
 * Draws a mandelbrot fractal on screen using SDL.
 *
 */

//#define NDEEBUG

#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <SDL/SDL.h>
#include <pthread.h>
#include <gflags/gflags.h>
#include <boost/multiprecision/mpfr.hpp>

using namespace boost::multiprecision;

mpfr_float XMIN = -2.5; 
mpfr_float XMAX = 1.0;
mpfr_float YMIN = -1.0;
mpfr_float YMAX = 1.0;

DEFINE_string(orgX, "-.75", "x-axis center point of the image");
DEFINE_string(orgY, "0", "y-axis center point of the image");
DEFINE_double(DX, 3.5, "x-axis diameter of the grid to display");
DEFINE_double(DY, 2, "y-axis diameter of grid to display");
DEFINE_double(ZOOM, .05, "Percent to zoom in each iteration");
DEFINE_int32(screen_width, 800, "The width of the screen");

#define DX FLAGS_DX
#define DY FLAGS_DY

const int THREADS  = 4;      //!< Concurrent threads to run
const int SCR_CD   = 32;     //!< Bits of color
const int MAX_ITER = 512;    //!< Max iterations for each point of the screen
const int FRAMES   = 2000;   //!< Frames to render before quiting

int64_t   SCR_WDTH = 0;      //!< Screen Width
int64_t   SCR_HGHT = 0;      //!< Screen Height

struct pixel{
    Uint8 r;       //!< Red componet
    Uint8 g;       //!< Green componet
    Uint8 b;       //!< Blue componet
    Uint8 alpha;   //!< Alpha componet

    pixel(){
        r     = 0;
        g     = 0;
        b     = 0;
        alpha = 255;
    }
}colorTable[MAX_ITER];

struct rendThrData{
    static uint32_t      next_id;
    const uint32_t       id;
    mpfr_float          xmin;
    mpfr_float          xmax;
    mpfr_float          ymin;
    mpfr_float          ymax;
    uint64_t*            img;

    rendThrData():id(next_id++){
        img = new uint64_t[SCR_WDTH * SCR_HGHT];
    }
    ~rendThrData(){
        delete[] img;
    }

    uint64_t& operator()(int64_t x, int64_t y){
        assert((x*SCR_HGHT + y) < (SCR_WDTH*SCR_HGHT));
        return this->img[x * SCR_HGHT + y];
    }
};
uint32_t rendThrData::next_id = 0;

/** Maps a value between 2 limits to some other value between 2 other
 * limits
 */
inline mpfr_float map(mpfr_float x, mpfr_float in_min, 
    mpfr_float in_max, mpfr_float out_min, mpfr_float out_max){
    return (x - in_min) * (out_max - out_min) / 
        (in_max - in_min) + out_min;
}

/** Initialize the color table with values for color coding images
*/
void generateColorTable(){
    for(int i = 1; i < MAX_ITER; i++){
        colorTable[i].r = i + 32 % i;
        colorTable[i].g = i + 64 % i;
        colorTable[i].b = i + 96;
    }
}

void put_px(SDL_Surface* scr, int x, int y, pixel* p){
    Uint32* p_screen = (Uint32*)scr->pixels;
    p_screen  += y * scr->w + x;
    *p_screen  = SDL_MapRGBA(scr->format, p->r, p->g, p->b,
            p->alpha);
}

uint64_t mandelbrot(mpfr_float x0, mpfr_float y0){
    uint64_t   itr = 0;
    mpfr_float x   = 0.0;
    mpfr_float y   = 0.0;
    while((x*x + y*y < 4.0) && (itr < MAX_ITER)){
        mpfr_float xtmp = x*x - y*y + x0;
        mpfr_float ytmp = 2*x*y + y0;
        if((x == xtmp) && (y == ytmp)){
            itr = MAX_ITER;
            break;
        }
        x = xtmp;
        y = ytmp;
        itr++;
    }
    return itr;
}

void* renderThread(void *data){
    int itr = 0;
    rendThrData* d = (rendThrData*)data;
    for(int py = 0; py < SCR_HGHT; py++){
        for(int px = 0; px < SCR_WDTH; px++){
            mpfr_float x0 = map(px, 0, SCR_WDTH, d->xmin, d->xmax);
            mpfr_float y0 = map(py, 0, SCR_HGHT, d->ymin, d->ymax);
            (*d)(px, py) = mandelbrot(x0, y0);
        }
    }
    pthread_exit(NULL);
}

void setScale(rendThrData* d){
    // define local macros for calculating delta
#define dx (xmax-xmin)
#define dy (ymax-ymin)
    static int    count = 0; // times this function was called also an id
    static mpfr_float xmin  = XMIN;
    static mpfr_float xmax  = XMAX;
    static mpfr_float ymin  = YMIN;
    static mpfr_float ymax  = YMAX;
    static mpfr_float zoom  = FLAGS_ZOOM / 2.0;
    mpfr_float xsca         = (dx*zoom)/2.0;
    mpfr_float ysca         = (dy*zoom)/2.0;
    xmin += xsca;
    xmax -= xsca;
    ymin += ysca;
    ymax -= ysca;
    count++;
    d->xmin = xmin;
    d->xmax = xmax;
    d->ymin = ymin;
    d->ymax = ymax;
    // Undefine local macros
#undef dx
#undef dy
}

int main(int argc, char*argv[]){
    pthread_t    thrds[THREADS];
    rendThrData* data;
    SDL_Surface* screen;
    int i, rc, x, y;
    
    mpfr_float::default_precision(20);
    // Handle command line args
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    assert(XMIN < XMAX);
    assert(YMIN < YMAX);
    SCR_WDTH = FLAGS_screen_width;
    SCR_HGHT = ((double)SCR_WDTH / DX) * DY;
    XMIN = static_cast<mpfr_float>(FLAGS_orgX) - DX / 2.0;
    XMAX = static_cast<mpfr_float>(FLAGS_orgX) + DX / 2.0;
    YMIN = static_cast<mpfr_float>(FLAGS_orgY) - DY / 2.0;
    YMAX = static_cast<mpfr_float>(FLAGS_orgY) + DY / 2.0;
    fprintf(stderr, "WND SZ = %d by %d\n", SCR_WDTH, SCR_HGHT);

    SDL_Init(SDL_INIT_EVERYTHING); 
    generateColorTable();
    screen = SDL_SetVideoMode(SCR_WDTH, SCR_HGHT, SCR_CD, SDL_SWSURFACE);
    data   = new rendThrData[THREADS];
    // initialize threads
    for(i = 0; i < THREADS; i++){
        setScale(&data[i]);
        rc = pthread_create(&thrds[i], NULL, renderThread, (void*)&data[i]);
        if(rc){
            fprintf(stderr, "Couldn't create thread: %d\n", rc);
        }
    }
    for(i = 0; i < FRAMES; i++){
        pthread_join(thrds[i % THREADS], NULL); // Join current thread
        SDL_LockSurface(screen);
        // Draw to the screen
        for(x = 0; x < SCR_WDTH; x++){
            for(y = 0; y < SCR_HGHT; y++){
                // update pixel on screen for the data gotten from the
                // thread workload that just ran
                put_px(screen, x, y,
                        &colorTable[data[i%THREADS](x, y)% MAX_ITER]);
            }
        }
        printf("Drew Frame %d\n", i);
        SDL_UnlockSurface(screen);
        if(SDL_Flip(screen) == -1){
            fprintf(stderr, "SDL_Flip Failed");
            return 1;
        }
        // Recreate the thread
        setScale(&data[i%THREADS]); 
        rc = pthread_create(&thrds[i % THREADS], NULL, renderThread, 
                (void*)&data[i % THREADS]);
        if(rc){
            fprintf(stderr, "Couldn't create thread: %d\n", rc);
        }
    }
    for(i = 0; i < THREADS; i++){
        pthread_join(thrds[i], NULL);
    }
    delete[] data;
    SDL_Quit();
}
