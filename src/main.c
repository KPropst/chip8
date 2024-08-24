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
   uint8_t screen[256] = {0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, }; // Each bit represents an 8 pixel wide row on the screen
   /* ------------------------------------------------------------ */
   /* File Init */
   /* ------------------------------------------------------------ */
   FILE *file;
   u_char buffer[1];
   u_char buffer2[1];
   int16_t mode = -1;

   //file = fopen("1-chip8-logo.ch8", "rb");
   //file = fopen("2-ibm-logo.ch8", "rb");
   file = fopen("3-corax+.ch8", "rb");
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

   int new_pixel;

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
                if (buffer[0] == 0x00)                              {mode = 0x00;}
                else if (buffer[0] >= 0x60 && buffer[0] <= 0x6F)    {mode = 0x60; buffer2[0] = buffer[0] - 0x60;}
                else if (buffer[0] >= 0xA0 && buffer[0] <= 0xAF)    {mode = 0xA0; buffer2[0] = buffer[0] - 0xA0;}
                else if (buffer[0] >= 0xD0 && buffer[0] <= 0xDF)    {mode = 0xD0; buffer2[0] = buffer[0] - 0xD0;}
                else if (buffer[0] >= 0x10 && buffer[0] <= 0x1F)    {mode = 0x10; buffer2[0] = buffer[0] - 0x10;}
                else if (buffer[0] >= 0x30 && buffer[0] <= 0x3F)    {mode = 0x30; buffer2[0] = buffer[0] - 0x30;}
                else if (buffer[0] >= 0x70 && buffer[0] <= 0x7F)    {mode = 0x70; buffer2[0] = buffer[0] - 0x70;}
                pc++;
                break;
           case 0x00: 
                if (buffer[0] == 0xE0) {
                printf("Clear Screen\n");
                    for (int i = 0; i < 256; i++) {
                        screen[i] = 0;
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
            case 0xD0: /* Dxyn */
                // Start at Coordinates Vx, Vy
                x = (0xF & buffer2[0]) / 8;
                y = (0xF0 & buffer[0]) / 16;
                N = 0xF & buffer[0];
                printf("Draw sprite at %d and %d at height %d\n", v[x], v[y], N);
                for (int i = v[Y]; i < v[Y]+N; i++) {
                    int screenpos = (8*i)+(v[x]/8);  
                    screen[screenpos]     = (memory[I] >> (v[x] % 8)) ^ screen[screenpos];
                    screenpos = (8*i)+((v[x]+8)/8);
                    screen[screenpos] = (memory[I] << (8 - (v[x] % 8))) ^ screen[screenpos];
                    I++;
                }
                mode = -1;
                pc++;
                break;
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
       int j = 7;
       int height_offset = 0;
       int last_border = 0;
       for (int i = 0; i < (WIDTH*HEIGHT); i+=SCALE) {
            if (j == -1) j = 7;
            if (last_border == WIDTH) {
                height_offset += WIDTH;
                i += WIDTH;
                last_border = 0;
            }
            new_pixel = ((screen[(i-height_offset) / (8*SCALE)] >> j) & 1)*99999;
            pixels[i] = new_pixel;
            pixels[i+1] = new_pixel;
            pixels[i+1+WIDTH] = new_pixel;
            pixels[i+WIDTH] = new_pixel;
            j--;
            last_border+=SCALE;
       }

        SDL_UpdateWindowSurface(window);
        usleep(50000);
   }
   /* ------------------------------------------------------------ */
}
