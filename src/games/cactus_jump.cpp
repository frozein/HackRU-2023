#include "cactus_jump.hpp"

#include <random>
#include <GLFW/glfw3.h>
#include "renderer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

static dnui::Element* g_baseElement;
static dnui::Text* g_scoreText;

static std::vector<std::pair<std::string, std::string>> g_cards;

static Texture g_playerTex;
static Texture g_groundTex;

static int g_score;

static float g_groundPos1;
static float g_groundPos2;
bool g_forwardGround;

static bool g_shouldClose;

//--------------------------------------------------------------------------------------------------------------------------------//

void cactus_jump_enter(std::vector<std::pair<std::string, std::string>> cards, DNUIfont* font)
{
	g_shouldClose = false;
	g_score = 0;

	g_groundPos1 = 1024;
	g_groundPos2 = 3072;
	g_forwardGround = true;

	while(cards.size() > 0)
	{
		int i = rand() % cards.size();
		g_cards.push_back(cards[i]);
		cards.erase(cards.begin() + i);
	}

	g_baseElement = new dnui::Element();

	g_scoreText = new dnui::Text(dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN), dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX),
	                             dnui::Dimension(dnui::Dimension::RELATIVE, 0.15f), "SCORE: 0", font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 0.7f, 0.05f,
								 {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);
	g_baseElement->m_children.push_back(g_scoreText);

	g_playerTex = renderer_load_texture("art/player.png");
	g_groundTex = renderer_load_texture("art/ground.png");
}

void cactus_jump_exit()
{
	renderer_free_texture(g_playerTex);
	renderer_free_texture(g_groundTex);

	delete g_baseElement;
}

//--------------------------------------------------------------------------------------------------------------------------------//

int cactus_jump_update(float dt, int windowW, int windowH)
{
	if(g_forwardGround)
	{
		g_groundPos1 -= dt;
		g_groundPos2 = g_groundPos1 + 2048.0f;
		if(g_groundPos1 < -1024.0f)
			g_forwardGround = false;
	}
	else
	{
		g_groundPos2 -= dt;
		g_groundPos1 = g_groundPos2 + 2048.0f;
		if(g_groundPos2 < -1024.0f)
			g_forwardGround = true;
	}

	g_baseElement->update(dt, {0.0f, 0.0f}, {(float)windowW, (float)windowH});

	if(g_shouldClose)
		return -1;
	else
		return 0;
}

void cactus_jump_render()
{
	renderer_draw_texture(g_groundTex, (int)g_groundPos1, 824, 2048, 512, 0.0f);
	renderer_draw_texture(g_groundTex, (int)g_groundPos2, 824, 2048, 512, 0.0f);

	g_baseElement->render(1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------------//

void cactus_jump_mouse_click()
{

}

void cactus_jump_key_pressed(int key, int action)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		g_shouldClose = true;

	
}