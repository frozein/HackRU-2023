#include "cactus_jump.hpp"

#include <random>
#include "renderer.hpp"

//--------------------------------------------------------------------------------------------------------------------------------//

static void _answer_callback(int callbackID);
static void _setup_question(bool bonus);
static void _death_screen();

//--------------------------------------------------------------------------------------------------------------------------------//

struct Hittable
{
	Texture tex;
	float pos;
	bool hitPlayer;
};

static GLFWwindow* g_window;
static DNUIfont* g_font;

static dnui::Element* g_baseElement;
static dnui::Text* g_scoreText;

static dnui::Transition g_questionInTransition;
static dnui::Transition g_questionOutTransition;
static dnui::Element* g_questionElement;

static std::vector<std::pair<std::string, std::string>> g_cards;

static Texture g_skyTexture;
static Texture g_playerTex;
static Texture g_heartTex;
static Texture g_groundTex;
static Texture g_cactiiTextures[3];
static Texture g_questionTextures[2];

static int g_score;

static float g_groundPos1;
static float g_groundPos2;
bool g_forwardGround;

static int g_numLives;
static float g_playerPos;
static float g_playerVel;
static float g_lastSpacePress;

static std::vector<Hittable> g_cactii;
static std::vector<Hittable> g_questions;
static float g_nextHittableSpawn;

static bool g_died;
static bool g_inQuestion;
static bool g_questionAnswered;
static int g_correctAnswer;

static bool g_shouldClose;

//--------------------------------------------------------------------------------------------------------------------------------//

void cactus_jump_enter(std::vector<std::pair<std::string, std::string>> cards, GLFWwindow* window, DNUIfont* font)
{
	g_window = window;
	g_font = font;

	g_shouldClose = false;
	g_score = 0;

	g_groundPos1 = 1024;
	g_groundPos2 = 3072;
	g_forwardGround = true;

	g_numLives = 3;
	g_playerPos = 0.0f;
	g_playerVel = 0.0f;
	g_lastSpacePress = 0.0f;
	g_nextHittableSpawn = 0.0f;

	g_died = false;
	g_inQuestion = false;

	g_cactii.clear();
	g_questions.clear();

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

	g_questionElement = new dnui::Element();
	g_questionElement->m_alphaMult = 0.0f;
	g_baseElement->m_children.push_back(g_questionElement);

	dnui::Box* coverBox = new dnui::Box(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(), dnui::Dimension(), -1, {0.0f, 0.0f, 0.0f, 0.25f});
	g_questionElement->m_children.push_back(coverBox);

	dnui::Text* promptText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.8f, dnui::Coordinate::CENTER_CENTER), dnui::Dimension(dnui::Dimension::RELATIVE, 0.7f),
	                                        "Select the Matching Card", font, {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0, 0.7f, 0.05f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);
	g_questionElement->m_children.push_back(promptText);

	dnui::Box* questionBox = new dnui::Box(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.55f, dnui::Coordinate::CENTER_CENTER), dnui::Dimension(dnui::Dimension::RELATIVE, 0.3f), dnui::Dimension(dnui::Dimension::ASPECT, 0.7f),
	                                       -1, {1.0f, 1.0f, 1.0f, 1.0f}, 25.0f, 0.0f, {0.0f, 0.0f, 0.0f, 1.0f}, 15.0f);
	dnui::Text* questionText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(dnui::Dimension::RELATIVE, 0.9f), "question", font, {0.0f, 0.0f, 0.0f, 1.0f}, 0.8f,
	                                          200.0f, 2);
	questionBox->m_children.push_back(questionText);
	g_questionElement->m_children.push_back(questionBox);

	dnui::Transition baseTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	baseTransition.set_target_y(dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN));
	dnui::Transition hoverTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	hoverTransition.set_target_y(dnui::Coordinate(dnui::Coordinate::PIXELS, 40.0f, dnui::Coordinate::CENTER_MIN));
	dnui::Transition holdTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	holdTransition.set_target_y(dnui::Coordinate(dnui::Coordinate::PIXELS, 30.0f, dnui::Coordinate::CENTER_MIN));

	dnui::Button answerButton = dnui::Button(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN), dnui::Dimension(dnui::Dimension::RELATIVE, 0.2f),
	                                         dnui::Dimension(dnui::Dimension::ASPECT, 0.7f), _answer_callback, 0, -1, {1.0f, 1.0f, 1.0f, 1.0f}, 20.0f, 0.0f, {0.0f, 0.0f, 0.0f, 1.0f}, 15.0f,
											 baseTransition, hoverTransition, holdTransition);
	for(int i = 0; i < 4; i++)
	{
		dnui::Button* newButton = new dnui::Button(answerButton);
		newButton->m_xPos = dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.5f + ((float)i - 1.5f) * 0.25f, dnui::Coordinate::CENTER_CENTER);
		newButton->m_callbackID = i;

		dnui::Text* buttonText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(), dnui::Dimension(dnui::Dimension::RELATIVE, 0.9f), "answer", font, {0.0f, 0.0f, 0.0f, 1.0f}, 0.8f,
		                                        200.0f, 2);
		newButton->m_children.push_back(buttonText);

		g_questionElement->m_children.push_back(newButton);
	}

	g_questionInTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	g_questionInTransition.set_target_alphamult(1.0f);
	g_questionOutTransition = dnui::Transition(250.0f, dnui::Transition::EXPONENTIAL);
	g_questionOutTransition.set_target_alphamult(0.0f);

	g_skyTexture = renderer_load_texture("art/sky.png");
	g_playerTex = renderer_load_texture("art/player.png");
	g_heartTex = renderer_load_texture("art/heart.png");
	g_groundTex = renderer_load_texture("art/ground.png");
	g_cactiiTextures[0] = renderer_load_texture("art/cactus1.png");
	g_cactiiTextures[1] = renderer_load_texture("art/cactus2.png");
	g_cactiiTextures[2] = renderer_load_texture("art/cactus3.png");
	g_questionTextures[0] = renderer_load_texture("art/question1.png");
	g_questionTextures[1] = renderer_load_texture("art/question2.png");
}

void cactus_jump_exit()
{
	renderer_free_texture(g_skyTexture);
	renderer_free_texture(g_playerTex);
	renderer_free_texture(g_heartTex);
	renderer_free_texture(g_groundTex);
	for(int i = 0; i < 3; i++)
		renderer_free_texture(g_cactiiTextures[i]);
	for(int i = 0; i < 2; i++)
		renderer_free_texture(g_questionTextures[i]);

	delete g_baseElement;
}

//--------------------------------------------------------------------------------------------------------------------------------//

int cactus_jump_update(float dt, int windowW, int windowH)
{
	((dnui::Text*)g_baseElement->m_children[0])->m_text = "SCORE: " + std::to_string(g_score);

	g_baseElement->update(dt, {0.0f, 0.0f}, {(float)windowW, (float)windowH});

	if(g_inQuestion || g_died)
		if(g_shouldClose)
			return -1;
		else
			return 0;

	dt *= ((float)g_score / 1000.0f) + 1.0f;

	if(g_forwardGround)
	{
		g_groundPos1 -= 2.0f * dt;
		g_groundPos2 = g_groundPos1 + 2048.0f;
		if(g_groundPos1 < -1024.0f)
			g_forwardGround = false;
	}
	else
	{
		g_groundPos2 -= 2.0f * dt;
		g_groundPos1 = g_groundPos2 + 2048.0f;
		if(g_groundPos2 < -1024.0f)
			g_forwardGround = true;
	}

	g_playerPos += g_playerVel * dt;
	g_playerVel -= dt * 0.08f;
	if(g_playerPos <= 0.0f)
	{
		g_playerPos = 0.0f;
		g_playerVel = 0.0f;
	}

	if(glfwGetKey(g_window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetMouseButton(g_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		g_lastSpacePress = (float)glfwGetTime();

	if(g_playerPos <= 0.0f && (float)glfwGetTime() - g_lastSpacePress <= 0.005f)
		g_playerVel = 7.5f;

	for(int i = 0; i < g_cactii.size(); i++)
	{
		g_cactii[i].pos -= 2.0f * dt;
		if(g_cactii[i].pos < 320.0f && g_cactii[i].pos > 192.0f && g_playerPos < 128.0f && !g_cactii[i].hitPlayer)
		{
			g_cactii[i].hitPlayer = true;
			g_numLives--;
		}

		if(g_cactii[i].pos < -64.0f)
		{
			g_cactii.erase(g_cactii.begin() + i);
			i--;
			g_score += 10;
		}
	}

	for(int i = 0; i < g_questions.size(); i++)
	{
		g_questions[i].pos -= 2.0f * dt;
		if(g_questions[i].pos < 320.0f && g_questions[i].pos > 192.0f && !g_questions[i].hitPlayer && (g_playerPos > 128.0f || g_questions[i].tex == g_questionTextures[0]))
		{
			g_questions[i].hitPlayer = true;
			_setup_question(g_questions[i].tex != g_questionTextures[0]);
		}

		if(g_questions[i].pos < -64.0f)
		{
			g_questions.erase(g_questions.begin() + i);
			i--;
		}
	}

	if((float)glfwGetTime() > g_nextHittableSpawn)
	{
		if(rand() % 5 == 1)
		{
			Hittable newQuestion;
			newQuestion.pos = 2000.0f;
			newQuestion.tex = g_questionTextures[rand() % 2];
			newQuestion.hitPlayer = false;
			g_questions.push_back(newQuestion);
		}
		else
		{
			Hittable newCactus;
			newCactus.pos = 2000.0f;
			newCactus.tex = g_cactiiTextures[rand() % 3];
			newCactus.hitPlayer = false;
			g_cactii.push_back(newCactus);
		}

		g_nextHittableSpawn = (float)glfwGetTime() + ((float)rand() / RAND_MAX * 2.0f + 1.0f) / ((float)g_score / 1000.0f + 1.0f);
	}

	if(g_numLives < 1)
		_death_screen();

	if(g_shouldClose)
		return -1;
	else
		return 0;
}

void cactus_jump_render()
{
	renderer_draw_texture(g_skyTexture, 960, 540, 1920, 1080, 0.0f);

	renderer_draw_texture(g_playerTex, 256, 520 - (int)g_playerPos, 128, 128, 0.0f);

	for(int i = 0; i < g_cactii.size(); i++)
		renderer_draw_texture(g_cactii[i].tex, (int)g_cactii[i].pos, 520, 128, 128, 0.0f);

	for(int i = 0; i < g_questions.size(); i++)
		renderer_draw_texture(g_questions[i].tex, (int)g_questions[i].pos, 392, 128, 384, 0.0f);

	renderer_draw_texture(g_groundTex, (int)g_groundPos1, 824, 2048, 512, 0.0f);
	renderer_draw_texture(g_groundTex, (int)g_groundPos2, 824, 2048, 512, 0.0f);

	for(int i = 0; i < g_numLives; i++)
		renderer_draw_texture(g_heartTex, 84 + 140 * i, 150, 128, 128, 0.0f);

	g_baseElement->render(1.0f);
}

//--------------------------------------------------------------------------------------------------------------------------------//

void cactus_jump_mouse_click()
{
	if(g_inQuestion && g_questionAnswered)
	{
		g_questionElement->set_transition(g_questionOutTransition, 0.0f);
		g_inQuestion = false;
		g_nextHittableSpawn = (float)glfwGetTime() + 1.0f;
	}

	if(g_died)
	{
		cactus_jump_exit();
		cactus_jump_enter(g_cards, g_window, g_font);
	}

	g_baseElement->handle_event(dnui::Event(dnui::Event::MOUSE_RELEASE));
}

void cactus_jump_key_pressed(int key, int action)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		g_shouldClose = true;
}

//--------------------------------------------------------------------------------------------------------------------------------//

static void _answer_callback(int callbackID)
{
	if(!g_inQuestion || g_questionAnswered)
		return;
	
	((dnui::Box*)g_questionElement->m_children[3 + g_correctAnswer])->m_outlineColor = {0.0f, 1.0f, 0.0f, 1.0f};
	if(callbackID != g_correctAnswer)
	{
		((dnui::Box*)g_questionElement->m_children[3 + callbackID])->m_outlineColor = {1.0f, 0.0f, 0.0f, 1.0f};
		g_numLives--;
	}
	else
		g_score += 100;

	((dnui::Text*)g_questionElement->m_children[1])->m_text = "Click Anywhere to Continue";
	g_questionAnswered = true;
}

static void _setup_question(bool bonus)
{
	g_inQuestion = true;
	g_questionAnswered = false;
	g_questionElement->set_transition(g_questionInTransition, 0.0f);

	if(bonus)
		((dnui::Text*)g_questionElement->m_children[1])->m_text = "BONUS QUESTION: Select the Matching Card";
	else
		((dnui::Text*)g_questionElement->m_children[1])->m_text = "Select the Matching Card";

	((dnui::Text*)g_questionElement->m_children[2]->m_children[0])->m_text = g_cards[0].first;
	g_correctAnswer = rand() % 3;

	for(int i = 0; i < 4; i++)
	{
		((dnui::Button*)g_questionElement->m_children[3 + i])->m_outlineColor = {0.0f, 0.0f, 0.0f, 1.0f};

		if(i == g_correctAnswer)
			((dnui::Text*)g_questionElement->m_children[3 + i]->m_children[0])->m_text = g_cards[0].second;
		else
			((dnui::Text*)g_questionElement->m_children[3 + i]->m_children[0])->m_text = g_cards[rand() % (g_cards.size() - 1) + 1].second;
	}

	std::pair<std::string, std::string> oldCard = g_cards[0];
	g_cards.erase(g_cards.begin());
	g_cards.push_back(oldCard);
}

static void _death_screen()
{
	g_died = true;

	dnui::Box* infoBox = new dnui::Box(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.6f, dnui::Coordinate::CENTER_CENTER),
	                                   dnui::Dimension(dnui::Dimension::RELATIVE, 0.5f), dnui::Dimension(dnui::Dimension::ASPECT, 0.7f), -1, {1.0f, 1.0f, 1.0f, 1.0f},
									   25.0f, 0.0f, {0.0f, 0.0f, 0.0f, 1.0f}, 15.0f);
	dnui::Text* gameOverText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.85f, dnui::Coordinate::CENTER_CENTER), dnui::Dimension(dnui::Dimension::RELATIVE, 0.85f),
	                                          "Game Over", g_font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 0.7f, 0.05f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);
	dnui::Text* scoreText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.4f, dnui::Coordinate::CENTER_CENTER), dnui::Dimension(dnui::Dimension::RELATIVE, 0.9f),
	                                          "Final Score: " + std::to_string(g_score), g_font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 0.7f, 0.05f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);
	dnui::Text* instructionText = new dnui::Text(dnui::Coordinate(), dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.7f, dnui::Coordinate::CENTER_CENTER), dnui::Dimension(dnui::Dimension::RELATIVE, 0.9f),
	                                             "Click to Play Again, Press Esc to Exit", g_font, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 0.7f, 0.05f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);

	infoBox->m_children.push_back(gameOverText);								  
	infoBox->m_children.push_back(scoreText);
	infoBox->m_children.push_back(instructionText);

	g_baseElement->m_children.push_back(infoBox);
}