play_music.e: flip_cet4.c file_read.h
	gcc $^ -o $@ -lSDL -lSDL_mixer -lSDL_ttf -lSDL_ttf -lSDL_gfx -std=c99
flip_cet4: flip_cet4.c file_read.h
	mipsel-linux-gcc $^ -o $@ -lSDL -lSDLmain -lSDL_mixer -lSDL_ttf -lSDL_gfx
