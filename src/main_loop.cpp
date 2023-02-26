#include "main_loop.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "renderer.hpp"
#include "set_download.hpp"
#include "game_selector.hpp"
#include <vector>
#include <tuple>
#include <string>
#include <fstream>

#include "games/cactus_jump.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

static std::vector<std::pair<std::string, std::string>> parse_flashcards(const char* path);

static void _window_size_callback(GLFWwindow* window, int width, int height);
static void _mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void _key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void _scroll_callback(GLFWwindow* window, double offsetX, double offsetY);
static void GLAPIENTRY _gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

static GLFWwindow* g_window = NULL;
static int g_windowW, g_windowH;

enum class GameState
{
	SET_DOWNLOAD,
	GAME_SELECT,
	GAME
} g_state = GameState::SET_DOWNLOAD;
int g_activeGame;

DNUIfont* g_font = nullptr;

//--------------------------------------------------------------------------------------------------------------------------------//

bool hackru_init(int windowW, int windowH, const char* windowName)
{
	//init GLFW:
	//---------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	//create and init window:
	//---------------------------------
	g_window = glfwCreateWindow(windowW, windowH, windowName, NULL, NULL);
	if(!g_window)
	{
		std::cout << "FAILED TO CREATE WINDOW\n";
		return false;
	}

	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(g_window, _window_size_callback);
	glfwSetMouseButtonCallback(g_window, _mouse_button_callback);
	glfwSetKeyCallback(g_window, _key_callback);
	glfwSetScrollCallback(g_window, _scroll_callback);

	g_windowW = windowW;
	g_windowH = windowH;

	//load opengl functions:
	//---------------------------------
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "FAILED TO LOAD OPENGL FUNCTIONS\n";
		return false;
	}

	//set gl state:
	//---------------------------------
	glViewport(0, 0, windowW, windowH);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(_gl_message_callback, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//init dnui and renderer:
	//---------------------------------
	if(!DNUI_init(windowW, windowH))
		return false;
	if(!renderer_init(windowW, windowH, 1920, 1080))
		return false;
	
	//init font:
	//---------------------------------
	g_font = DNUI_load_font("fonts/arial.ttf", 72);
	set_download_enter(g_window, g_font);

	return true;
}

void hackru_main_loop()
{
	bool running = true;
	float lastTime = (float)glfwGetTime();
	while(running)
	{
		//close if needed:
		if(glfwWindowShouldClose(g_window))
			running = false;

		//get dt:
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime = lastTime;

		//update mouse position:
		double mouseX, mouseY;
		glfwGetCursorPos(g_window, &mouseX, &mouseY);
		dnui::Button::set_mouse_state({(float)mouseX - g_windowW * 0.5f, (float)mouseY - g_windowH * 0.5f}, glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

		//clear:
		glClearColor(0.021f, 0.1f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//update/render:
		switch(g_state)
		{
		case GameState::SET_DOWNLOAD:
		{
			int shouldSwitch = set_download_update(deltaTime, g_windowW, g_windowH);
			set_download_render();

			if(shouldSwitch == -1)
				running = false;
			else if(shouldSwitch == 1)
			{
				g_state = GameState::GAME_SELECT;
				set_download_exit();
				game_selector_enter(g_font);
			}
			break;
		}
		case GameState::GAME_SELECT:
		{
			int shouldSwitch = game_selector_update(deltaTime, g_windowW, g_windowH);
			game_selector_render();

			if(shouldSwitch == -1)
			{
				g_state = GameState::SET_DOWNLOAD;
				game_selector_exit();
				set_download_enter(g_window, g_font);
			}
			else if(shouldSwitch > 0)
			{
				g_state = GameState::GAME;
				g_activeGame = shouldSwitch;
				game_selector_exit();
				
				switch(g_activeGame)
				{
				case 1:
					cactus_jump_enter(parse_flashcards("flashcards.txt"), g_window, g_font);
					break;
				}
			}

			break;
		}
		case GameState::GAME:
		{
			int shouldSwitch = 0;

			switch(g_activeGame)
			{
			case 1:
			{
				shouldSwitch = cactus_jump_update(deltaTime, g_windowW, g_windowH);
				cactus_jump_render();
				break;
			}
			}

			if(shouldSwitch == 0)
				break;

			g_state = GameState::GAME_SELECT;
			switch(g_activeGame)
			{
			case 1:
				cactus_jump_exit();
				break;
			}
			game_selector_enter(g_font);
			break;
		}
		}

		//swap buffers:
		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}
}

void hackru_close()
{
	switch(g_state)
	{
	case GameState::SET_DOWNLOAD:
		set_download_exit();
		break;
	case GameState::GAME_SELECT:
		game_selector_exit();
		break;
	case GameState::GAME:
		switch(g_activeGame)
		{
		case 1:
			cactus_jump_exit();
			break;	
		}
		break;
	}

	DNUI_free_font(g_font);
	renderer_quit();
	glfwTerminate();
}

//--------------------------------------------------------------------------------------------------------------------------------//

static std::vector<std::pair<std::string, std::string>> parse_flashcards(const char* path)
{
	std::vector<std::pair<std::string, std::string>> result;

	std::ifstream file;
	file.open(path);

	int numCards;
	file >> numCards;

	for(int i = 0; i < numCards; i++)
	{
		std::pair<std::string, std::string> card;
		file >> card.first;
		file >> card.second;
		result.push_back(card);
	}

	file.close();
	return result;
}

//--------------------------------------------------------------------------------------------------------------------------------//

static void _window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
	renderer_set_window_size(width, height);
	g_windowW = width;
	g_windowH = height;
}

static void _mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_RELEASE)
		return;

	switch(g_state)
	{
	case GameState::SET_DOWNLOAD:
		set_download_mouse_click();
		break;
	case GameState::GAME_SELECT:
		game_selector_mouse_click();
		break;
	case GameState::GAME:
		switch(g_activeGame)
		{
		case 1:
			cactus_jump_mouse_click();
			break;	
		}
		break;
	}
}

static void _key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch(g_state)
	{
	case GameState::SET_DOWNLOAD:
		set_download_key_pressed(key, action);
		break;
	case GameState::GAME_SELECT:
		game_selector_key_pressed(key, action);
		break;
	case GameState::GAME:
		switch(g_activeGame)
		{
		case 1:
			cactus_jump_key_pressed(key, action);
			break;
		}
		break;
	}
}

static void _scroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
	if(g_state == GameState::GAME_SELECT)
		game_selector_scroll((float)offsetY / 10.0f);
}

static void GLAPIENTRY _gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if(severity == GL_DEBUG_SEVERITY_NOTIFICATION || type == 0x8250)
		return;

	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        	 (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        	  type, severity, message );
}