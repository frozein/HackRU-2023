#ifndef CACTUSJUMP_HPP
#define CACTUSJUMP_HPP

#include "DoonUI/doonui.hpp"
#include <vector>
#include <tuple>
#include <string>

void cactus_jump_enter(std::vector<std::pair<std::string, std::string>> cards, DNUIfont* font);
void cactus_jump_exit();

int cactus_jump_update(float dt, int windowW, int windowH);
void cactus_jump_render();

void cactus_jump_mouse_click();
void cactus_jump_key_pressed(int key, int action);

#endif