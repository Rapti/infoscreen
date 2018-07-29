//
// Created by leon on 27.07.18.
//

#include "Theme.h"

Theme::Theme() {}

Theme::~Theme() {

}

const sf::Color &Theme::getModuleBG() const {
	return ModuleBG;
}

const sf::Color &Theme::getModuleOutline() const {
	return ModuleOutline;
}

const sf::Color &Theme::getTextPrimary() const {
	return TextPrimary;
}

const sf::Color &Theme::getTextSecondary() const {
	return TextSecondary;
}

const sf::Color &Theme::getTextTertiary() const {
	return TextTertiary;
}

const sf::Color &Theme::getTextDisabled() const {
	return TextDisabled;
}

const sf::Color &Theme::getTextError() const {
	return TextError;
}

const sf::Color &Theme::getDiagramFill() const {
	return DiagramFill;
}

const sf::Color &Theme::getDiagramLine() const {
	return DiagramLine;
}
