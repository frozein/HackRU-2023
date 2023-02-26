#include "set_download.hpp"

#include <GLFW/glfw3.h>
#include <windows.h>
#include <cstdio>
#include <iostream>
#include "renderer.hpp"

#undef RELATIVE

#pragma comment(lib, "urlmon.lib")

//--------------------------------------------------------------------------------------------------------------------------------//

static void _download_button_callback(int callbackID);

static GLFWwindow* g_window;
static dnui::Element* g_baseElement;

static bool g_shouldPlay;
static bool g_shouldExit;

static Texture g_backgroundTex;

//--------------------------------------------------------------------------------------------------------------------------------//

void set_download_enter(GLFWwindow* window, DNUIfont* font)
{
	g_window = window;
	g_shouldPlay = false;
	g_shouldExit = false;

	g_baseElement = new dnui::Element();

	g_backgroundTex = renderer_load_texture("art/background.png");
	dnui::Box* background = new dnui::Box();
	background->m_texture = g_backgroundTex;
	g_baseElement->m_children.push_back(background);

	dnui::Text* titleText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX),
	                                       dnui::Dimension(dnui::Dimension::RELATIVE, 0.5F), "FunnCards.com", font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 
										   0.65f, 0.5f, {0.72f, 0.55f, 0.0f, 1.0f}, 0.5f, 0.05f);
	g_baseElement->m_children.push_back(titleText);

	dnui::Transition baseTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	baseTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.75f));
	dnui::Transition hoverTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	hoverTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.775f));
	dnui::Transition holdTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	holdTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.77f));

	dnui::Button* downloadButton = new dnui::Button(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(dnui::Dimension::RELATIVE, 0.75f), 
	                                               dnui::Dimension(dnui::Dimension::ASPECT, 0.15f), _download_button_callback, 0, -1, {1.0f, 1.0f, 1.0f, 1.0f},
												   20.0f, 0.0f, {0.72f, 0.55f, 0.0f, 1.0f}, 15.0f, baseTransition, hoverTransition, holdTransition);
	dnui::Text* downloadText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(dnui::Dimension(dnui::Dimension::RELATIVE, 0.8f)),
	                                          "Download Flashcards From Clipboard", font, {0.0f, 0.0f, 0.0f, 1.0f});
	downloadButton->m_children.push_back(downloadText);
	g_baseElement->m_children.push_back(downloadButton);
}

void set_download_exit()
{
	renderer_free_texture(g_backgroundTex);
	delete g_baseElement;
}

//--------------------------------------------------------------------------------------------------------------------------------//

int set_download_update(float dt, int windowW, int windowH)
{
	if(windowW > windowH)
	{
		g_baseElement->m_children[0]->m_width = dnui::Dimension(dnui::Dimension::RELATIVE, 1.0f);
		g_baseElement->m_children[0]->m_height = dnui::Dimension(dnui::Dimension::ASPECT, 1.0f);
	}
	else
	{
		g_baseElement->m_children[0]->m_height = dnui::Dimension(dnui::Dimension::RELATIVE, 1.0f);
		g_baseElement->m_children[0]->m_width = dnui::Dimension(dnui::Dimension::ASPECT, 1.0f);
	}

	g_baseElement->update(dt, {0.0f, 0.0f}, {(float)windowW, (float)windowH});

	if(g_shouldExit)
		return -1;
	else if(g_shouldPlay)
		return 1;
	else
		return 0;
}

void set_download_render()
{
	g_baseElement->render(1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------------//

void set_download_mouse_click()
{
	g_baseElement->handle_event(dnui::Event(dnui::Event::MOUSE_RELEASE));
}

void set_download_key_pressed(int key, int action)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		g_shouldExit = true;
}

//--------------------------------------------------------------------------------------------------------------------------------//

static void _download_button_callback(int callbackID)
{
	const char* clipboardString = glfwGetClipboardString(g_window);

	//convert to wchar:
	size_t len = mbstowcs(nullptr, clipboardString, 0);
	std::wstring wClipboardString(len, 0);
	mbstowcs(&wClipboardString[0], clipboardString, wClipboardString.size());

	if(S_OK != URLDownloadToFileW(NULL, wClipboardString.c_str(), L"flashcards.txt", 0, NULL))
		std::cout << "FAILED TO DOWNLOAD FILE\n";
	else
		g_shouldPlay = true;
}

#define RELATIVE 2