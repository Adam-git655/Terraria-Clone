#pragma once
#include <iostream>
#include <unordered_map>
#include "SFML/Audio.hpp"

enum class Sounds
{
	Footsteps,
	Jump,
	Swing,
	Break,
	Place
};

class SoundManager
{
public:
	SoundManager();

	void play(Sounds soundName);

	void playLooping(Sounds soundName);
	void stopLooping(Sounds soundName);
	
private:
	std::unordered_map<Sounds, sf::SoundBuffer> soundsData;
	std::unordered_map<Sounds, sf::Sound> loopingSounds; //sf::Sounds permanantly stay bound to their specific sound
	std::vector<sf::Sound> playingSounds; //For one shot sounds. Reassignment of sf::sounds.

	void loadData();
};