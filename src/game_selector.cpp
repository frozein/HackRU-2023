#include "game_selector.hpp"

#include <GLFW/glfw3.h>
#include "renderer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

static void _select_button_callback(int callbackID);

static dnui::Element* g_baseElement;

static int g_selectedGame;
static bool g_shouldExit;

static Texture g_backgroundTex;

//--------------------------------------------------------------------------------------------------------------------------------//

void game_selector_enter(DNUIfont* font)
{
	g_selectedGame = 0;
	g_shouldExit = false;

	g_baseElement = new dnui::Element();

	g_backgroundTex = renderer_load_texture("art/background.png");
	dnui::Box* background = new dnui::Box();
	background->m_texture = g_backgroundTex;
	g_baseElement->m_children.push_back(background);

	dnui::List* selectList = new dnui::List(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(), dnui::Dimension(), dnui::Dimension(dnui::Dimension::RELATIVE, 0.25f),
											dnui::Dimension(dnui::Dimension::ASPECT, 1.0f), dnui::Dimension(dnui::Dimension::RELATIVE, 0.15f), dnui::Dimension(dnui::Dimension::ASPECT, 1.25f),
											4, true);
	g_baseElement->m_children.push_back(selectList);

	dnui::Transition baseTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	baseTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.2f));
	baseTransition.set_target_float(0.0f, offsetof(dnui::Button, m_angle));
	dnui::Transition hoverTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	hoverTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.215f));
	hoverTransition.set_target_float(4.0f, offsetof(dnui::Button, m_angle));
	dnui::Transition holdTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	holdTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.21f));
	holdTransition.set_target_float(2.0f, offsetof(dnui::Button, m_angle));

	dnui::Button selectButton = dnui::Button(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(dnui::Dimension::RELATIVE, 0.2f), 
	                                         dnui::Dimension(dnui::Dimension::ASPECT, 1.0f), _select_button_callback, 0, -1, {1.0f, 1.0f, 1.0f, 1.0f},
											 20.0f, 0.0f, {0.72f, 0.55f, 0.0f, 1.0f}, 15.0f, baseTransition, hoverTransition, holdTransition);
	dnui::Text nameText = dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN), 
	                                 dnui::Dimension(dnui::Dimension::RELATIVE, 0.8f), "Game Title Here", font, {0.0f, 0.0f, 0.0f, 1.0f}, 0.0f, 0.0f, 0);

	for(int i = 0; i < 10; i++)
	{
		dnui::Button* newButton = new dnui::Button(selectButton);
		newButton->m_callbackID = i + 1;

		dnui::Text* newText = new dnui::Text(nameText);
		if(i == 0)
			newText->m_text = "Cactus Jump";
		newButton->m_children.push_back(newText);	
	
		selectList->add_item(newButton, i + 1);
	}

	dnui::Text* selectText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX), 
	                                        dnui::Dimension(dnui::Dimension::RELATIVE, 0.3f), "Select A Game", font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f,
											0, 0.65f, 0.5f, {0.72f, 0.55f, 0.0f, 1.0f}, 0.5f, 0.05f);
	g_baseElement->m_children.push_back(selectText);	
}

void game_selector_exit()
{
	delete g_baseElement;
}

//--------------------------------------------------------------------------------------------------------------------------------//

int game_selector_update(float dt, int windowW, int windowH)
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
	else
		return g_selectedGame;
}

void game_selector_render()
{
	g_baseElement->render(1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------------//

void game_selector_mouse_click()
{
	g_baseElement->handle_event(dnui::Event(dnui::Event::MOUSE_RELEASE));
}

void game_selector_key_pressed(int key, int action)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		g_shouldExit = true;
}

void game_selector_scroll(float dir)
{
	dnui::Event event(dnui::Event::SCROLL);
	event.scroll.dir = dir;
	g_baseElement->handle_event(event);
}

//--------------------------------------------------------------------------------------------------------------------------------//

static void _select_button_callback(int callbackID)
{
	if(callbackID == 1)
		g_selectedGame = 1;
}