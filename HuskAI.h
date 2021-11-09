#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"

namespace HuskAI {
	static inline void SpawnHusk() 
	{
		Cores::PlayerController->CheatManager->Summon(L"Huskpawn_C");
	}
}