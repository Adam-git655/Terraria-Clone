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
	SoundManager(float masterVolume = 100.0f);

	void play(Sounds soundName);

	void playLooping(Sounds soundName);
	void stopLooping(Sounds soundName);

	void setMasterVolume(float volume);
	
private:
	std::unordered_map<Sounds, sf::SoundBuffer> soundsData;
	std::unordered_map<Sounds, sf::Sound> loopingSounds; //sf::Sounds permanantly stay bound to their specific sound
	std::vector<sf::Sound> playingSounds; //For one shot sounds. Reassignment of sf::sounds.
	float masterVolume = 100.0f;

	void loadData();
};