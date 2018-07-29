//
// Created by leon on 27.07.18.
//

#ifndef INFOSCREEN_THEME_H
#define INFOSCREEN_THEME_H


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class Theme {
public:
	Theme();
	virtual ~Theme();
	virtual void drawBackgroundTo(sf::RenderTarget* canvas) = 0;

	const sf::Color &getModuleBG() const;
	const sf::Color &getModuleOutline() const;
	const sf::Color &getTextPrimary() const;
	const sf::Color &getTextSecondary() const;
	const sf::Color &getTextTertiary() const;
	const sf::Color &getTextError() const;
	const sf::Color &getTextDisabled() const;
	const sf::Color &getDiagramFill() const;
	const sf::Color &getDiagramLine() const;
	virtual void updateDisplaySize(unsigned int w, unsigned int h);

protected:
	unsigned int w, h;
	sf::Color ModuleBG;
	sf::Color ModuleOutline;
	sf::Color TextPrimary;
	sf::Color TextSecondary;
	sf::Color TextTertiary;
	sf::Color TextError;
	sf::Color TextDisabled;
	sf::Color DiagramFill;
	sf::Color DiagramLine;
};


#endif //INFOSCREEN_THEME_H
