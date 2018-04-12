#include "Include/MenuState.hpp"
#include "Include/Button.hpp"
#include "Include/Utility.hpp"
#include "Include/ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#pragma region step 4
#include "Include/SettingsState.hpp"
#pragma endregion


MenuState::MenuState(StateStack& stack, Context context)
: State(stack, context)
, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);

	auto playButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	playButton->setPosition(650, 350);
	playButton->setText("Play");
	playButton->setCallback([this] ()
	{
		requestStackPop();
		requestStackPush(States::Game);
	});

#pragma region step 5
	auto settingsButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	settingsButton->setPosition(650, 460);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
	{
		requestStackPush(States::Settings);
	});
#pragma endregion

	auto exitButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	exitButton->setPosition(650, 570);
	exitButton->setText("Exit");
	exitButton->setCallback([this] ()
	{
		requestStackPop();
	});

	mGUIContainer.pack(playButton);

#pragma region step 6
	mGUIContainer.pack(settingsButton);
#pragma endregion

	mGUIContainer.pack(exitButton);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}