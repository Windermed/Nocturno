#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"
#include <iostream>

namespace Building
{
	SDK::UBuildingEditModeMetadata* RoofMD = nullptr;
	SDK::UBuildingEditModeMetadata* StairsMD = nullptr;
	SDK::UBuildingEditModeMetadata* WallMD = nullptr;
	SDK::UBuildingEditModeMetadata* FloorMD = nullptr;

	SDK::UFortBuildingItemDefinition* RoofDef = nullptr;
	SDK::UFortBuildingItemDefinition* FloorDef = nullptr;
	SDK::UFortBuildingItemDefinition* WallDef = nullptr;
	SDK::UFortBuildingItemDefinition* StairDef = nullptr;

	static void Initialize()
	{
		RoofDef = LoadObject<SDK::UFortBuildingItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
		FloorDef = LoadObject<SDK::UFortBuildingItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
		WallDef = LoadObject<SDK::UFortBuildingItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
		StairDef = LoadObject<SDK::UFortBuildingItemDefinition>(L"/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");

		RoofMD = LoadObject<SDK::UBuildingEditModeMetadata>(L"/Game/Building/EditModePatterns/Roof/EMP_Roof_RoofC.EMP_Roof_RoofC");
		StairsMD = LoadObject<SDK::UBuildingEditModeMetadata>(L"/Game/Building/EditModePatterns/Stair/EMP_Stair_StairW.EMP_Stair_StairW");
		WallMD = LoadObject<SDK::UBuildingEditModeMetadata>(L"/Game/Building/EditModePatterns/Wall/EMP_Wall_Solid.EMP_Wall_Solid");
		FloorMD = LoadObject<SDK::UBuildingEditModeMetadata>(L"/Game/Building/EditModePatterns/Floor/EMP_Floor_Floor.EMP_Floor_Floor");
	}

	static void ShowBuildPreviews(SDK::AFortWeap_BuildingTool* BuildingTool, SDK::UFortBuildingItemDefinition* WeaponData)
	{
		if (!WeaponData || !BuildingTool) return;

		SDK::UBuildingEditModeMetadata* MetaData = nullptr;

		if (WeaponData == RoofDef)
			MetaData = RoofMD;
		else if (WeaponData == StairDef)
			MetaData = StairsMD;
		else if (WeaponData == WallDef)
			MetaData = WallMD;
		else if (WeaponData == FloorDef)
			MetaData = FloorMD;

		if (MetaData)
		{
			BuildingTool->DefaultMetadata = MetaData;
			BuildingTool->OnRep_DefaultMetadata();
		}

	}

}