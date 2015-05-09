#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <SDL/SDL.h>
#include <pthread.h>
#include <gflags/gflags.h>

#define MAND_DX (MAND_XMAX - MAND_XMIN)
#define MAND_DY (MAND_YMAX - MAND_YMIN)

const double MAND_XMIN = -2.5;
const double MAND_XMAX = 1.0;
const double MAND_YMIN = -1.0;
const double MAND_YMAX = 1.0;

const int THREADS  = 4;
const int SCR_WDTH = MAND_DX * 300;
const int SCR_HGHT = MAND_DY * 300;
const int SCR_CD   = 32;
const int ITERATS  = 15000;
const int MAX_ITER = 1024; //!< Maximum number of iter for mandelbrot
const int FRAMES   = 100;  //!< Frames to render before quiting

DEFINE_double(xmin, -2.5, "The minimum point on the x-axis");
DEFINE_double(xmax,  1.0, "The maximum point on the x-axis");
DEFINE_double(ymin, -1.0, "The minimum point on the y-axis");
DEFINE_double(ymax,  1.0, "The maximum point on the y-axis");

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
} colorTable[MAX_ITER];

struct rendThrData{
    static uint32_t next_id;
    const uint32_t  id;
    double          xmin;
    double          xmax;
    double          ymin;
    double          ymax;
    uint64_t        img[SCR_WDTH][SCR_HGHT];

    rendThrData():id(next_id++){}
    ~rendThrData(){}
};
uint32_t rendThrData::next_id = 0;

/** Maps a value between 2 limits to some other value between 2 other
 * limits
 */
inline double map(double x, double in_min, double in_max, 
        double out_min, double out_max){
    return (x - in_min) * (out_max - out_min) / 
        (in_max - in_min) + out_min;
}

void generateColorTable(){
    srand(time(0));
    for(int i = 0; i < MAX_ITER; i++){
        colorTable[i].r = rand() % 255;
        colorTable[i].g = rand() % 255;
        colorTable[i].b = rand() % 255;
    }
}

void put_px(SDL_Surface* scr, int x, int y, pixel* p){
    Uint32* p_screen = (Uint32*)scr->pixels;
    p_screen  += y * scr->w + x;
    *p_screen  = SDL_MapRGBA(scr->format, p->r, p->g, p->b,
            p->alpha);
}

void* renderThread(void *data){
    rendThrData* d = (rendThrData*)data;
    double x0, y0, x, y, xtmp;
    int py, px, itr;
    for(py = 0; py < SCR_HGHT; py++){
        for(px = 0; px < SCR_WDTH; px++){
            x0 = map(px, 0, SCR_WDTH, d->xmin, d->xmax);
            y0 = map(py, 0, SCR_HGHT, d->ymin, d->ymax);
            x = 0.0;
            y = 0.0;
            itr = 0;
            while((x*x + y*y < 2*2) and (itr < MAX_ITER)){
                xtmp = x*x - y*y + x0;
                y = 2*x*y + y0;
                x = xtmp;
                itr++;
            }
            d->img[px][py] = itr;
        }
    }
    pthread_exit(NULL);
}

void setScale(rendThrData* d){
    // define local macros for calculating delta
    #define dx (xmax-xmin)
    #define dy (ymax-ymin)
    static double xmin = FLAGS_xmin;
    static double xmax = FLAGS_xmax;
    static double ymin = FLAGS_ymin;
    static double ymax = FLAGS_ymax;
    double xsca = (dx*.05)/2.0;
    double ysca = (dy*.05)/2.0;
    xmin += xsca;
    xmax -= xsca;
    ymin += ysca;
    ymax -= ysca;
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
    int i,rc;

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    SDL_Init(SDL_INIT_EVERYTHING); 
    generateColorTable();
    screen = SDL_SetVideoMode(SCR_WDTH, SCR_HGHT, SCR_CD, SDL_SWSURFACE);
    data = new rendThrData[THREADS];
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
        for(int x = 0; x < SCR_WDTH; x++){
            for(int y = 0; y < SCR_HGHT; y++){
                put_px(screen, x, y, &colorTable[data[i%THREADS].img[x][y]]);
            }
        }
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
