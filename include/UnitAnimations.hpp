#pragma once

#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include <unordered_map>

class UnitAnimations : public sf::Drawable, public sf::Transformable
{
public:
	UnitAnimations();
	~UnitAnimations();
	//UnitAnimations(AnimatedSprite anim);
	UnitAnimations(sf::Texture& anim);
	void setAnimation(std::string key, Animation anim);
    void update(sf::Time deltaTime);
    void play(std::string key);
	void stop();
	AnimatedSprite* sprite;
	const sf::Texture& getTexture();

private:
	std::unordered_map<std::string, Animation> animations;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::RenderTexture* profileTex;

};
