/*
 * =====================================================================================
 *
 *       Filename:  MainMenuState.hpp
 *
 *    Description:
 *
 *        Created:  09/01/2019 13:12:00
 *
 *         Author:  Sylvio Menubarbe, <sylvio.menubarbe@gmail.com>
 *
 * =====================================================================================
 */
#ifndef MAINMENUSTATE_HPP_
#define MAINMENUSTATE_HPP_

#include <gk/core/ApplicationState.hpp>
#include <gk/gl/Shader.hpp>
#include <gk/graphics/RectangleShape.hpp>
#include <gk/graphics/Image.hpp>

#include "MenuWidget.hpp"

class MainMenuState : public gk::ApplicationState {
	public:
		MainMenuState(gk::ApplicationState *parent = nullptr);

		void update() override;

	private:

		void draw(gk::RenderTarget &target, gk::RenderStates states) const override;

		MenuWidget m_menuWidget{1, 8};

		gk::Shader m_shader;

		glm::mat4 m_projectionMatrix;

		gk::Image m_backgroundImage;
};

#endif // MAINMENUSTATE_HPP_
