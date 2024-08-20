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
   // bool screen[64*32];
   uint8_t screen[256];
   /* ------------------------------------------------------------ */
   /* File Init */
   /* ------------------------------------------------------------ */
   FILE *file;
   u_char buffer[1];
   u_char buffer2[1];
   int16_t mode = -1;

   file = fopen("1-chip8-logo.ch8", "rb");
   //file = fopen("2-ibm-logo.ch8", "rb");
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
   
   int x, y;
   int N;

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

   int pixel_old;

   /* Load ROM into memory */
   while (fread(buffer, sizeof(buffer), 1, file)) {
       memory[pc] = buffer[0];
       pc++;      
   }
   /* Read Memory */
   for (pc = 0x200; pc < 4095; pc++) {
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
                if (buffer[0] == 0x00)                              {mode = 0x00;}
                else if (buffer[0] >= 0x60 && buffer[0] <= 0x6F)    {mode = 0x60; buffer2[0] = buffer[0] - 0x60;}
                else if (buffer[0] >= 0xA0 && buffer[0] <= 0xAF)    {mode = 0xA0; buffer2[0] = buffer[0] - 0xA0;}
                else if (buffer[0] >= 0xD0 && buffer[0] <= 0xDF)    {mode = 0xD0; buffer2[0] = buffer[0] - 0xD0;}
                break;
           case 0x00: 
                printf("Clear Screen\n");
                if (buffer[0] == 0xE0) {
                    for (int i = 0; i < 64*32; i++) {
                        pixels[i] = 0;
                    }
                }
                else if (buffer[0] == 0xEE) printf("RETURN FROM SUB\n");
                mode = -1;
                break;
           case 0x60:
                printf("v[%d] set to %02X\n", buffer2[0], buffer[0]);
                v[buffer2[0]] = buffer[0];
                mode = -1;
                break;
           case 0xA0: 
                printf("I set to %02X\n",(buffer2[0]*0x100) + buffer[0]);
                I = (buffer2[0]*0x100) + buffer[0];
                mode = -1;
                break;
            case 0xD0: /* Dxyn */
                // Start at Coordinates Vx, Vy
                x = 0xF & buffer2[0];
                y = (0xF0 & buffer[0]) / 16;
                N = 0xF & buffer[0];
                printf("Draw sprite at v[%d] and v[%d] at height %d\n", x, y, N);
                printf("Sprite: %d", v[I]);
                for (int j = v[y]; j < v[y]+N; j++) {
                    for (int i = v[x]; i < v[x]+8; i++) {
                        pixel_old = pixels[(j*width)+i];          
                        pixels[(j*width)+i] = ((pixel_old/99999) ^ (((memory[I] >> (7 - (i - v[x]))) & 1)))*99999;
                    }
                    I++;
                }
                mode = -1;
                break;
           default: 
                break;

        
       }
        SDL_UpdateWindowSurface(window);
        usleep(50000);
   }
   /* ------------------------------------------------------------ */
}
