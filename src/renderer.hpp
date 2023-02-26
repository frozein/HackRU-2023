#ifndef RENDERER_HPP
#define RENDERER_HPP

typedef int Texture;

bool renderer_init(int windowW, int windowH, int logicalW, int logicalH);
void renderer_quit();
void renderer_set_window_size(int w, int h);
void renderer_set_logical_size(int w, int h);

Texture renderer_load_texture(const char* path);
void renderer_free_texture(Texture tex);
void renderer_draw_texture(Texture tex, int x, int y, int w, int h, float angle);

#endif