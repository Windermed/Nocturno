#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"

// WIP husk code, it hasn't been completed but i hope it can work.
namespace HuskAI {
	static inline void SpawnHusk() 
	{
		auto SpawnLoc = Cores::PlayerPawn->K2_GetActorLocation();
		SpawnLoc.X = SpawnLoc.X + 500;
		auto NewController = reinterpret_cast<SDK::AFortAIController*>(Util::SpawnActor(SDK::AFortAIController::StaticClass(), SDK::FVector{ 1, 1, 1000 }, SDK::FRotator{ 0,0,0 }));
		auto NewPawn = reinterpret_cast<SDK::AMissionBotPawn_C*>(Util::SpawnActor(SDK::AMissionBotPawn_C::StaticClass(), SpawnLoc, SDK::FRotator{ 0,0,0 }));
		NewController->Possess(NewPawn);
		NewController->SetGoalActor(Cores::PlayerPawn, true);
		NewPawn->SetShouldStartSleeping(false);
		//NewPawn->AIType = SDK::EFortressAIType::FAT_DebugOnly;
	}
}