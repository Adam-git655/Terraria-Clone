#pragma once

enum class Faction
{
	Player,
	Enemy
};

struct FactionComponent
{
	Faction faction = Faction::Player;
};