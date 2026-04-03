#pragma once
#include "SFML/Graphics.hpp"

struct Animation
{
	int rectLeftFirst = 120;
	int rectLeftLast = 360;
	int rectLeftMove = 20;
	int rectTop = 0;
	int width = 20;
	int height = 26;
	float animationTime = 0.035f;
	bool pingPongAnim = false;
};

struct AnimationComponent
{
	std::unordered_map<std::string, Animation> animations;
	std::string currentAnim = "idle";
	sf::IntRect rectSourceSprite;
	sf::Clock animClock;
	bool goingRight = true;

	void addAnimation(const std::string& name, Animation anim)
	{
		animations[name] = anim;
	}

	void play(const std::string& name)
	{
		if (currentAnim == name)
			return;

		auto it = animations.find(name);
		if (it == animations.end())
			return;

		currentAnim = name;

		auto& anim = it->second;
		rectSourceSprite = { anim.rectLeftFirst, anim.rectTop, anim.width, anim.height };
		animClock.restart();
	}
};