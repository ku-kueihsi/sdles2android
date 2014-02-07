#ifndef __GAME_H__
#define __GAME_H__

#include <cstdio>

void on_surface_created();
void on_surface_changed(int width, int height);
void on_touch();
void on_draw_frame();
void set_vertshader_file(FILE * pfile);
void set_fragshader_file(FILE * pfile);

#endif //__GAME_H__
