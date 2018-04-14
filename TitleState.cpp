#include "Include/TitleState.hpp"
#include "Include/Utility.hpp"
#include "Include/ResourceHolder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


TitleState::TitleState(StateStack& stack, Context context)
: State(stack, context)
, mText()
, mShowText(true)
, mTextEffectTime(sf::Time::Zero)
{
	mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));

	mText.setFont(context.fonts->get(Fonts::Main));
	mText.setString("Press any key to start");
	centerOrigin(mText);
//    mText.setPosition(sf::Vector2f(context.window->getSize() / 2u));
    mText.setPosition(context.window->getSize().x/2.f - mText.getLocalBounds().width/2.f, context.window->getSize().y/2.f - mText.getLocalBounds().height/2.f);
    mText.setCharacterSize(65);
    mText.setFillColor(sf::Color(255, 230, 155));
}

void TitleState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.draw(mBackgroundSprite);
    
    sf::RectangleShape filter;
    filter.setSize(window.getView().getSize());
    filter.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(filter);

	if (mShowText)
		window.draw(mText);
}

bool TitleState::update(sf::Time dt)
{
	mTextEffectTime += dt;

	if (mTextEffectTime >= sf::seconds(0.5f))
	{
		mShowText = !mShowText;
		mTextEffectTime = sf::Time::Zero;
	}

	return true;
}

bool TitleState::handleEvent(const sf::Event& event)
{
	// If any key is pressed, trigger the next screen
	if (event.type == sf::Event::KeyReleased)
	{
		requestStackPop();
		requestStackPush(States::Menu);
	}

	return true;
}
