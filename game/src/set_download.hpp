#ifndef SETDOWNLOAD_HPP
#define SETDOWNLOAD_HPP

#include <GLFW/glfw3.h>
#include "DoonUI/doonui.hpp"

void set_download_enter(GLFWwindow* window, DNUIfont* font);
void set_download_exit();

int set_download_update(float dt, int windowW, int windowH);
void set_download_render();

void set_download_mouse_click();
void set_download_key_pressed(int key, int action);

#endif