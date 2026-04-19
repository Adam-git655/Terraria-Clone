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
	float masterVolume = 100.0f;

	std::unordered_map<Sounds, sf::SoundBuffer> soundsData;
	std::unordered_map<Sounds, float> soundVolumes =
	{
		{Sounds::Footsteps, 20.0f},
		{Sounds::Jump, 95.0f},
		{Sounds::Swing, 65.0f},
		{Sounds::Break, 10.0f},
		{Sounds::Place, 10.0f}
	};

	std::unordered_map<Sounds, sf::Sound> loopingSounds; //sf::Sounds permanantly stay bound to their specific sound
	std::vector<sf::Sound> playingSounds; //For one shot sounds. Reassignment of sf::sounds.

	sf::Music BGMusic;

	void loadData();
	float getActualVolume(float soundVol) const;
};