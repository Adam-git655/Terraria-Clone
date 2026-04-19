#include "SoundManager.h"

SoundManager::SoundManager(float masterVolume)
{
	setMasterVolume(masterVolume);
	loadData();
	BGMusic.setLoop(true);
	BGMusic.play();
}

void SoundManager::loadData()
{
	if (!soundsData[Sounds::Footsteps].loadFromFile(RESOURCES_PATH "footstep.wav"))
	{
		std::cout << "ERROR LOADING FOOTSTEP SOUND\n";
	}
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
	if (!soundsData[Sounds::PlayerHit].loadFromFile(RESOURCES_PATH "playerHurt.wav"))
	{
		std::cout << "ERROR LOADING PLAYER HIT SOUND\n";
	}
	if (!soundsData[Sounds::EnemyHit].loadFromFile(RESOURCES_PATH "enemyHurt.wav"))
	{
		std::cout << "ERROR LOADING ENEMY HIT SOUND\n";
	}

	if (!BGMusic.openFromFile(RESOURCES_PATH "BGM.wav"))
	{
		std::cout << "ERROR OPENING BACKGROUND MUSIC\n";
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
			sound.setVolume(getActualVolume(soundVolumes[soundName]));
			sound.play();
			return;
		}
	}

	//if all sound objects are playing, then add a new sound object
	playingSounds.emplace_back(sf::Sound());
	playingSounds.back().setBuffer(soundsData[soundName]);
	playingSounds.back().setVolume(getActualVolume(soundVolumes[soundName]));
	playingSounds.back().play();
}

void SoundManager::playLooping(Sounds soundName)
{
	//already playing
	if (loopingSounds.count(soundName) && loopingSounds[soundName].getStatus() == sf::Sound::Playing)
		return;

	loopingSounds[soundName].setBuffer(soundsData[soundName]);
	loopingSounds[soundName].setVolume(getActualVolume(soundVolumes[soundName]));
	loopingSounds[soundName].setLoop(true);
	loopingSounds[soundName].play();
}

void SoundManager::stopLooping(Sounds soundName)
{
	if (loopingSounds.count(soundName))
		loopingSounds[soundName].stop();
}

void SoundManager::setMasterVolume(float volume)
{
	masterVolume = std::clamp(volume, 0.0f, 100.0f);

	//update already playing looping sounds
	for (auto& [name, sound] : loopingSounds)
	{
		sound.setVolume(getActualVolume(soundVolumes[name]));
	}

	//update BGMusic
	BGMusic.setVolume(volume);
}

float SoundManager::getActualVolume(float soundVol) const
{
	return (soundVol / 100.0f) * masterVolume;
}