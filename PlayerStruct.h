#pragma once

#include <string>
#include "SDK.hpp"
#include "Util.h"

struct Player
{
	std::string DisplayName;
	int PlayerId;

	SDK::APlayerController* Controller;
	SDK::APlayerPawn_Generic_C* Pawn;
};

SDK::TArray<Player*>* Players;

static Player* FindPlayerById(int id)
{
	for (int i = 0; i < Players->Num(); i++)
	{
		auto player = Players->operator[](i);

		if (player->PlayerId == id)
		{
			return player;
		}
	}
}

static Player* FindPlayerByName(std::string name)
{
	for (int i = 0; i < Players->Num(); i++)
	{
		auto player = Players->operator[](i);

		if (player->DisplayName == name)
		{
			return player;
		}
	}
}