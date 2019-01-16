/*
 * =====================================================================================
 *
 *       Filename:  MainMenuState.cpp
 *
 *    Description:
 *
 *        Created:  09/01/2019 13:43:02
 *
 *         Author:  Sylvio Menubarbe, <sylvio.menubarbe@gmail.com>
 *
 * =====================================================================================
 */
#include <gk/core/ApplicationStateStack.hpp>
#include <gk/core/Mouse.hpp>

#include "Config.hpp"
#include "MainMenuState.hpp"
#include "GameState.hpp"

MainMenuState::MainMenuState(gk::ApplicationState *parent) : ApplicationState(parent) {
	m_shader.createProgram();
	m_shader.addShader(GL_VERTEX_SHADER, "resources/shaders/basic.v.glsl");
	m_shader.addShader(GL_FRAGMENT_SHADER, "resources/shaders/basic.f.glsl");
	m_shader.linkProgram();

    m_projectionMatrix = glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f);

    gk::Mouse::setCursorGrabbed(false);
	gk::Mouse::setCursorVisible(true);
	gk::Mouse::resetToWindowCenter();

	m_backgroundImage.load("texture-blocks");
	m_backgroundImage.setPosition(0, 0);

    m_menuWidget.setScale(GUI_SCALE, GUI_SCALE, 1);
    m_menuWidget.addButton(0, 2, "Single player", [this] (TextButton &) { m_stateStack->push<GameState>(); });
	m_menuWidget.addButton(0, 3, "Multiplayers", nullptr);
	m_menuWidget.addButton(0, 4, "Mods", nullptr);
	m_menuWidget.addButton(0, 5, "Options", nullptr);
	m_menuWidget.addButton(0, 6, "Exit", [this] (TextButton &) { while(!m_stateStack->empty()) m_stateStack->pop(); });
}

void MainMenuState::update() {}

void MainMenuState::draw(gk::RenderTarget &target, gk::RenderStates states) const {
	if (m_parent)
		target.draw(*m_parent, states);

	states.transform *= getTransform();

	states.projectionMatrix = m_projectionMatrix;

	states.shader = &m_shader;
	states.vertexAttributes = gk::VertexAttribute::Only2d;

	target.draw(m_backgroundImage, states);
	target.draw(m_menuWidget, states);
}