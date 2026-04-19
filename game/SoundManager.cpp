#include "SoundManager.h"

SoundManager::SoundManager()
{
	loadData();
}

void SoundManager::loadData()
{
	if (!soundsData[Sounds::Jump].loadFromFile(RESOURCES_PATH "jump.wav"))
	{
		std::cout << "ERROR LOADING JUMP SOUND\n";
	};
	if (!soundsData[Sounds::Swing].loadFromFile(RESOURCES_PATH "swing.wav"))
	{
		std::cout << "ERROR LOADING SWING SOUND\n";
	}
	if (!soundsData[Sounds::Break].loadFromFile(RESOURCES_PATH "break.wav"))
	{
		std::cout << "ERROR LOADING BREAK SOUND\n";
	}
	if (!soundsData[Sounds::Place].loadFromFile(RESOURCES_PATH "place.wav"))
	{
		std::cout << "ERROR LOADING PLACE SOUND\n";
	}
}

void SoundManager::play(Sounds soundName)
{
	//play from an already stopped sound object
	for (auto& sound : playingSounds)
	{
		if (sound.getStatus() != sf::Sound::Playing)
		{
			sound.setBuffer(soundsData[soundName]);
			sound.play();
			return;
		}
	}

	//if all sound objects are playing, then add a new sound object
	playingSounds.emplace_back(sf::Sound());
	playingSounds.back().setBuffer(soundsData[soundName]);
	playingSounds.back().play();
}

void SoundManager::playLooping(Sounds soundName)
{
	//already playing
	if (loopingSounds.count(soundName) && loopingSounds[soundName].getStatus() == sf::Sound::Playing)
		return;

	loopingSounds[soundName].setBuffer(soundsData[soundName]);
	loopingSounds[soundName].setLoop(true);
	loopingSounds[soundName].play();
}

void SoundManager::stopLooping(Sounds soundName)
{
	if (loopingSounds.count(soundName))
		loopingSounds[soundName].stop();
}