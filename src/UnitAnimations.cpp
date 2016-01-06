#include "UnitAnimations.hpp"
#include "SFML\Graphics\RenderTexture.hpp"

static const std::string default_moves[4] = { "moveDown", "moveLeft", "moveRight", "moveUp" };



UnitAnimations::UnitAnimations()
{
	profileTex = new sf::RenderTexture();
	sprite = new AnimatedSprite();
}

// This crashes if you create UnitAnimations as a rhv, only works if you manually create it
UnitAnimations::~UnitAnimations()
{
//	delete sprite;
//	delete profileTex;
}

UnitAnimations::UnitAnimations(sf::Texture& tex, bool isStatic)
{
	profileTex = new sf::RenderTexture();
	if (!isStatic)
	{
		for (int i = 0; i < 4; i++)
		{
			Animation anim = Animation();
			anim.setSpriteSheet(tex);
			anim.addFrame(sf::IntRect(128, i * 128, 128, 128));
			anim.addFrame(sf::IntRect(0, i * 128, 128, 128));
			anim.addFrame(sf::IntRect(128, i * 128, 128, 128));
			anim.addFrame(sf::IntRect(256, i * 128, 128, 128));
			animations.insert({ default_moves[i], anim });
		}
		sprite = new AnimatedSprite();
		sprite->setAnimation(animations["moveDown"]);
	}
	else
	{
		Animation anim = Animation();
		anim.setSpriteSheet(tex);
		anim.addFrame(sf::IntRect(0, 0, 128, 128));
		animations.insert({ "static",anim });
		sprite = new AnimatedSprite();
		sprite->setAnimation(animations["static"]);
	}
	sprite->play();
	//sprite->update(sf::Time::Time());
}

void UnitAnimations::setAnimation(std::string key, Animation anim)
{
	animations.insert({ key, anim });
}


void UnitAnimations::update(sf::Time deltaTime)
{
	sprite->update(deltaTime);
}

void UnitAnimations::play(std::string key)
{
	if (sprite->getAnimation() == &animations[key])
	{
		sprite->play();
		return;
	}
	sprite->play(animations[key]);
}

void UnitAnimations::stop()
{
	sprite->stop();
}

void UnitAnimations::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*sprite);
}

const sf::Texture& UnitAnimations::getTexture()
{
	profileTex->create(128, 128);
	profileTex->draw(*sprite);
	profileTex->display();
	return profileTex->getTexture();
}