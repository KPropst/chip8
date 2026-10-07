#include "../headers/headers.h"
int main() {
   uint8_t memory[4096];
   /* ------------------------------------------------------------ */
   /* Registers */
   /* ------------------------------------------------------------ */
   uint8_t v[16];       // General-purpose
   uint16_t I = 0;      // Store Memory Addresses
   uint8_t delay;
   uint8_t sound;
   uint16_t pc = 0x200; // Program Counter

   uint16_t stack[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int keys[16] = { SDLK_0,
                    SDLK_1,
                    SDLK_2,
                    SDLK_3,
                    SDLK_4,
                    SDLK_5,
                    SDLK_6,
                    SDLK_7,
                    SDLK_8,
                    SDLK_9,
                    SDLK_a,
                    SDLK_b,
                    SDLK_c,
                    SDLK_d,
                    SDLK_e,
                    SDLK_f
                    };
   uint8_t sp;
   /* ------------------------------------------------------------ */
   /* Display */
   /* ------------------------------------------------------------ */
   uint8_t framebuffer[256] = {0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, }; // Each byte represents an 8 pixel wide row on the framebuffer
   /* ------------------------------------------------------------ */
   /* File Init */
   /* ------------------------------------------------------------ */
   FILE *file;
   u_char buffer[1];
   u_char buffer2[1];
   int16_t mode = -1;


   //file = fopen("1-chip8-logo.ch8", "rb");
   file = fopen("2-ibm-logo.ch8", "rb");
   //file = fopen("3-corax+.ch8", "rb");
   //file = fopen("4-flags.ch8", "rb");
   //file = fopen("6-keypad.ch8", "rb");
   //file = fopen("bad-apple-high-quality.ch8", "rb");
   if (file == NULL) {
       //printf("Error: Cannot open file\n");
       return 1;
   }



   /* ------------------------------------------------------------ */
   /* SDL Init */
   /* ------------------------------------------------------------ */
   SDL_Init(SDL_INIT_VIDEO);
   SDL_Window *window = SDL_CreateWindow("",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         WIDTH,
                                         HEIGHT,
                                         0);
   SDL_Surface *window_surface = SDL_GetWindowSurface(window);
   unsigned int *pixels = window_surface->pixels;
   int width = window_surface->w, height = window_surface->h;
   bool updatescreen = false;

   Uint32 pixel = SDL_MapRGBA(window_surface->format, 200, 130, 100, 255);
   Uint8 r, g, b, a;

   SDL_GetRGBA(pixel, window_surface->format, &r, &g, &b, &a);
   /* ------------------------------------------------------------ */
   /* Main Loop */
   /* ------------------------------------------------------------ */
      //if (event.type == SDL_KEYDOWN) {
      //  switch (event.key.keysym.sym) {
      //      default:
      //       break;
      //  }
      //}
      //if (event.type == SDL_KEYUP) {
      //  switch (event.key.keysym.sym) {
      //      default:
      //       break;
      //  }
      //}


   /* Load ROM into memory */
   while (fread(buffer, sizeof(buffer), 1, file)) {
       memory[pc] = buffer[0];
       pc++;
   }
   /* Read Memory */
   for (pc = 0x200; pc < 4095; pc += 0) {
        buffer[0] = memory[pc];
     SDL_Event event;
     /* Input Handling */
     while (SDL_PollEvent(&event))
     {
       if (event.type == SDL_QUIT) exit(0);
       if (event.type == SDL_WINDOWEVENT) { /* Redraw if resized */
         if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
           window_surface = SDL_GetWindowSurface(window);
           pixels = window_surface->pixels;
         }
       }

     }
       switch (mode) {
           case -1:
                mode = 0xF0 & buffer[0];
                buffer2[0] = buffer[0] - mode;
                pc++;
                break;
           case 0x00:
                if (buffer[0] == 0xE0) {
                    for (uint8_t i = 0; i < 255; i++)
                        framebuffer[i] = 0;
                    for (int i = 0; i < WIDTH*HEIGHT; i++)
                        pixels[i] = 0;
                    pc++;
                }
                else if (buffer[0] == 0xEE) {
                    //printf("RETURN FROM SUB\n");
                    sp--;
                    pc = stack[sp]+1;
                    stack[sp] = 0;
                }
                mode = -1;
                break;
            case 0x20:
                stack[sp] = pc;
                pc = ((buffer2[0]*0x100) + buffer[0]);
                sp++;
                mode = -1;
                break;
           case 0x60:
                v[buffer2[0]] = buffer[0];
                mode = -1;
                pc++;
                break;
           case 0xA0:
                I = (buffer2[0]*0x100) + buffer[0];
                mode = -1;
                pc++;
                break;
            case 0xD0:{ /* Dxyn */
                int8_t x = (0xF & buffer2[0]);
                int8_t y = (0xF0 & buffer[0]) / 16;
                int8_t N = 0xF & buffer[0];

                // Input to Chip-8 Framebuffer
                for (uint8_t i = v[y]; i < v[y]+N; i++) {
                    uint8_t framebufferpos = (8*i)+(v[x]/8);
                    framebuffer[framebufferpos]     = (memory[I] >> (v[x] % 8)) ^ framebuffer[framebufferpos];
                    framebufferpos++;
                    framebuffer[framebufferpos] = (memory[I] << (8 - (v[x] % 8))) ^ framebuffer[framebufferpos];
                    I++;
                }
                updatescreen = true;

                mode = -1;
                pc++;
                break;
           }
           case 0x10:
                pc = ((0xF & buffer2[0])*256) + buffer[0];
                mode = -1;
                break;
           case 0x70:
                v[buffer2[0]] += buffer[0];
                mode = -1;
                pc++;
                break;
           case 0x30:
                pc += ((v[buffer2[0] & 0xF]) == buffer[0]) ? 3 : 1;
                mode = -1;
                break;
            case 0x40:
                pc += ((v[buffer2[0] & 0xF]) != buffer[0]) ? 3 : 1;
                mode = -1;
                break;
            case 0x50:
                pc += ((v[buffer2[0] & 0xF]) == (v[(buffer[0] & 0xF0) / 16])) ? 3 : 1;
                mode = -1;
                break;
            case 0x80:{
                int x = buffer2[0] & 0xF;
                int y = (buffer[0] & 0xF0) / 16;
                uint8_t Vx_copy;
                switch (buffer[0] & 0xF) {
                    case 0x0:
                        v[x] = v[y];
                        break;
                    case 0x1:
                        v[x] |= v[y];
                        break;
                    case 0x2:
                        v[x] &= v[y];
                        break;
                    case 0x3:
                        v[x] ^= v[y];
                        break;
                    case 0x4 :{
                        Vx_copy = v[x];
                        v[x] += v[y];
                        v[0xF] = (v[x] < Vx_copy) ? 1 : 0;
                        break;
                    }
                    case 0x5:{
                        Vx_copy = v[x];
                        v[x] -= v[y];
                        v[0xF] = (Vx_copy < v[y]) ? 0 : 1;
                        break;
                    }
                    case 0x6:
                        Vx_copy = v[x];
                        v[x] = v[x] >> 1;
                        v[0xF] = Vx_copy & 1;
                        break;
                    case 0x7:
                        Vx_copy = v[x];
                        v[x] = v[y] - v[x];
                        v[0xF] = (v[y] >= Vx_copy) ? 1 : 0;
                        break;
                    case 0xE:
                        Vx_copy = v[x];
                        v[x] = v[x] << 1;
                        v[0xF] = Vx_copy >> 7;
                        break;
                }
                pc++;
                mode = -1;
                break;
            }
            case 0x90:
                pc += ((v[buffer2[0] & 0xF]) != (v[(buffer[0] & 0xF0) / 16])) ? 3 : 1;
                mode = -1;
                break;
            case 0xE0:
                if (buffer[0] == 0x9E) {
                    SDL_PollEvent(&event);
                    while (SDL_PollEvent(&event)) {
                        pc += (event.type == SDL_KEYDOWN && event.key.keysym.sym == keys[v[buffer2[0] & 0xF]]) ? 2 : 1;
                        break;
                    }
                    mode = -1;
                    break;
                }
                if (buffer[0] == 0xA1) {
                    while (SDL_PollEvent(&event)) {
                        pc += (event.type == SDL_KEYUP && event.key.keysym.sym == keys[v[buffer2[0] & 0xF]]) ? 2 : 1;
                        break;
                    }
                    mode = -1;
                    break;
                }
            case 0xF0:
                switch (buffer[0]) {
                   case 0x1E:
                        I += v[buffer2[0] & 0xF];
                        break;
                    case 0x33: /* Store Vx as BCD Value */
                        memory[I]   = v[(buffer2[0] & 0xF)] / 100;
                        memory[I+1] = (v[(buffer2[0] & 0xF)] - (memory[I]*100)) / 10;
                        memory[I+2] = v[(buffer2[0] & 0xF)] - (memory[I]*100) - (memory[I+1]*10);
                        break;
                    case 0x55:
                        for (int i = 0; i <= (buffer2[0] & 0xF); i++)
                            memory[I+i] = v[i] ;
                        break;
                    case 0x65:
                        for (int i = 0; i <= (buffer2[0] & 0xF); i++)
                            v[i] = memory[I+i];
                        break;
                }
                pc++;
                mode = -1;
                break;
           default:
                pc++;
                break;


       }

       // TODO: Still Wasting a lot of compute cycles by redrawing the framebuffer. When only portions are changed
       if (updatescreen) {
            for (uint16_t x = 0; x < (WIDTH); x++) {
                 uint16_t pixel;
                 for (uint16_t y = 0; y < HEIGHT; y++) {
                     if (y % SCALE == 0) /* Done to prevent duplicate calculations */
                         pixel = getpixel(framebuffer, (x / (8*SCALE))+((y / SCALE)*8), (x/SCALE) % 8) * 99999;
                     else if (x % SCALE == 0)
                         pixel = getpixel(framebuffer, (x / (8*SCALE))+((y / SCALE)*8), (x/SCALE) % 8) * 99999;
                     pixels[x+(y*WIDTH)] = pixel;
                 }
            }
            updatescreen = false;
       }

        SDL_UpdateWindowSurface(window);
        usleep(1000);
   }
   /* ------------------------------------------------------------ */
}

uint8_t getpixel(uint8_t array[], uint8_t x, uint8_t bit) {
    return (array[x] >> (7 - bit)) & 1;
}
