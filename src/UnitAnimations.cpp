#include "UnitAnimations.hpp"
#include "SFML\Graphics\RenderTexture.hpp"

static const std::string default_moves[4] = { "moveDown", "moveLeft", "moveRight", "moveUp" };



UnitAnimations::UnitAnimations()
{
	profileTex = new sf::RenderTexture();
	sprite = new AnimatedSprite();
}


UnitAnimations::~UnitAnimations()
{
	delete sprite;
	delete profileTex;
}

UnitAnimations::UnitAnimations(sf::Texture& tex)
{
	profileTex = new sf::RenderTexture();
	for (int i = 0; i < 4; i++)
	{
		Animation anim = Animation();
		anim.setSpriteSheet(tex);
		anim.addFrame(sf::IntRect(128, i*128, 128, 128));
		anim.addFrame(sf::IntRect(  0, i*128, 128, 128));
		anim.addFrame(sf::IntRect(128, i*128, 128, 128));
		anim.addFrame(sf::IntRect(256, i*128, 128, 128));
		animations.insert({ default_moves[i], anim});
	}
	sprite = new AnimatedSprite();
	sprite->setAnimation(animations["moveDown"]);
	sprite->play();
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
		sprite->play();
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