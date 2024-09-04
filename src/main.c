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
                          0, 0, 0, 0, 0, 0, 0, 0, }; // Each bit represents an 8 pixel wide row on the framebuffer
   /* ------------------------------------------------------------ */
   /* File Init */
   /* ------------------------------------------------------------ */
   FILE *file;
   u_char buffer[1];
   u_char buffer2[1];
   int16_t mode = -1;

   file = fopen("1-chip8-logo.ch8", "rb");
   //file = fopen("2-ibm-logo.ch8", "rb");
   //file = fopen("3-corax+.ch8", "rb");
   if (file == NULL) {
       printf("Error: Cannot open file\n");
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
   
   //int x, y;
   //int N;

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
       }}
       switch (mode) {
           case -1:
                mode = 0xF0 & buffer[0];
                buffer2[0] = buffer[0] - mode;
                pc++;
                break;
           case 0x00: 
                if (buffer[0] == 0xE0) {
                printf("Clear Screen\n");
                    for (uint8_t i = 0; i < 255; i++) {
                        framebuffer[i] = 0;
                    }
                    for (int i = 0; i < WIDTH*HEIGHT; i++) {
                        pixels[i] = 0;
                    }
                }
                else if (buffer[0] == 0xEE) printf("RETURN FROM SUB\n");
                mode = -1;
                pc++;
                break;
           case 0x60:
                printf("v[%d] set to %02X\n", buffer2[0], buffer[0]);
                v[buffer2[0]] = buffer[0];
                mode = -1;
                pc++;
                break;
           case 0xA0: 
                printf("I set to %02X\n",(buffer2[0]*0x100) + buffer[0]);
                I = (buffer2[0]*0x100) + buffer[0];
                mode = -1;
                pc++;
                break;
            case 0xD0:{ /* Dxyn */
                int8_t x = (0xF & buffer2[0]) / 8;
                int8_t y = (0xF0 & buffer[0]) / 16;
                int8_t N = 0xF & buffer[0];
                printf("Draw sprite at %d and %d at height %d\n", v[x], v[y], N);

                // Input to Chip-8 Framebuffer
                for (uint8_t i = v[Y]; i < v[Y]+N; i++) {
                    uint8_t framebufferpos = (8*i)+(v[x]/8);  
                    framebuffer[framebufferpos]     = (memory[I] >> (v[x] % 8)) ^ framebuffer[framebufferpos];
                    framebufferpos = (8*i)+((v[x]+8)/8);
                    framebuffer[framebufferpos] = (memory[I] << (8 - (v[x] % 8))) ^ framebuffer[framebufferpos];
                    I++;
                }
                updatescreen = true;

                // Input to Window Framebuffer
                //for (uint16_t x2 = x*SCALE; x2 < (x*SCALE)+(8*SCALE); x++) {
                //     uint16_t pixel;
                //     for (uint16_t y2 = y*SCALE; y2 < (y*SCALE)+(N*SCALE); y++) {
                //         if (y2 % SCALE == 0) /* Done to prevent duplicate calculations */
                //             pixel = getpixel(framebuffer, (x2 / (8*SCALE))+((y2 / SCALE)*8), (x2/SCALE) % 8) * 99999;
                //         else if (x2 % SCALE == 0)
                //             pixel = getpixel(framebuffer, (x2 / (8*SCALE))+((y2 / SCALE)*8), (x2/SCALE) % 8) * 99999;
                //         pixels[x2+(y2*WIDTH)] = pixel;
                //     }
                //}
                mode = -1;
                pc++;
                break;
           }
           case 0x10:
                printf("Jump to address %02X\n", ((0xF & buffer2[0])*256) + buffer[0]);
                pc = ((0xF & buffer2[0])*256) + buffer[0];
                mode = -1;
                break;
           case 0x70:
                printf("Add %d to v[%d]\n", buffer[0], buffer2[0]);
                v[buffer2[0]] += buffer[0];
                mode = -1;
                pc++;
                break;
           case 0x30:
                printf("If v[%d] == %02X, skip next instruction\n", buffer[0] & 15, buffer2[0]);
                pc += ((v[buffer[0]] & 15) == buffer2[0]) ? 2 : 1;
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
        usleep(50000);
   }
   /* ------------------------------------------------------------ */
}

uint8_t getpixel(uint8_t array[], uint8_t x, uint8_t bit) {
    return (array[x] >> (7 - bit)) & 1;
}
