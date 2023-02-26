#ifndef GAMESELECTOR_HPP
#define GAMESELECTOR_HPP

#include "DoonUI/doonui.hpp"

void game_selector_enter(DNUIfont* font);
void game_selector_exit();

int game_selector_update(float dt, int windowW, int windowH);
void game_selector_render();

void game_selector_mouse_click();
void game_selector_key_pressed(int key, int action);
void game_selector_scroll(float dir);

#endif