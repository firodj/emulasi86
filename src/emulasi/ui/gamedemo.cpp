#include "emulasi/ui/gamedemo.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace emulasi {

GameDemo::GameDemo(GameBaseConfig config): GameBase(config)
{
}

GameDemo::~GameDemo()
{
}

int GameDemo::PreRun() {
	m_plainbox.Init();
	return 0;
}

int GameDemo::OnRun() {
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768, 0.1f, 100.0f);
	glm::mat4 view(1.0);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 model(1.0);
	model = glm::rotate(model, (float)SDL_GetTicks64()/1000.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (m_shader.IsOk()) {
		m_shader.Activate();
		m_shader.SetMatrix4("projection", false, proj);
		m_shader.SetMatrix4("view", false, view);
		m_shader.SetMatrix4("model", false, model);

		m_shader.SetVec3("iResolution", glm::vec3(m_screenWidth, m_screenHeight, 1.0f));
		m_shader.SetFloat("iTime", SDL_GetTicks()/1000.0f);
	}

	m_plainbox.Draw();

	SwapBuffer();

	return 0;
}

