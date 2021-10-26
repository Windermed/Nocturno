#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"
#include <iostream>

// code for the quickbars and inventory
struct AFortAsQuickBars
{
public:
    unsigned char                                      UnknownData00[0x1A88];
    class SDK::AFortQuickBars* QuickBars;
};
struct WorldInventory
{
    unsigned char UnknownData00[0x1D28];
    class SDK::AFortInventory* WorldInventory;
};
struct ParentInventory
{
    unsigned char UnknownData00[0x0098];
    class SDK::AFortInventory               ParentInventory;
};
struct OwnerInventory
{
    unsigned char UnknownData00[0x01D8];
    class SDK::AFortInventory* OwnerInventory;
};
struct AFortAsBuildPreview
{
public:
    unsigned char UnknownData00[0x1788];
    class SDK::ABuildingPlayerPrimitivePreview* BuildPreviewMarker;
};
struct AFortAsCurrentBuildable
{
public:
    unsigned char UnknownData00[0x1940];
    class SDK::UClass* CurrentBuildableClass;
};
struct AFortAsLastBuildable
{
public:
    unsigned char UnknownData00[0x1948];
    class SDK::UClass* PreviousBuildableClass;
};
struct AFortAsEditActor
{
public:
    unsigned char UnknownData00[0x1A48];
    class SDK::ABuildingSMActor* EditBuildingActor;
};
struct AFortAsBuildPreviewMID
{
public:
    unsigned char UnknownData00[0x1928];
    class SDK::UMaterialInstanceDynamic* BuildPreviewMarkerMID;
};

// SDK variables for building 
SDK::AFortQuickBars* QuickBars;
SDK::FGuid* m_pgEditToolDef;
SDK::FGuid* m_pgPickaxe;
SDK::FGuid WallGuid;
SDK::FGuid FloorGuid;
SDK::FGuid StairGuid;
SDK::FGuid RoofGuid;
SDK::UFortEditToolItemDefinition* m_pEditToolDef;
SDK::UFortWeaponMeleeItemDefinition* PickaxeDef;
SDK::UFortBuildingItemDefinition* m_pWallBuildDef;
SDK::UFortBuildingItemDefinition* m_pFloorBuildDef;
SDK::UFortBuildingItemDefinition* m_pStairBuildDef;
SDK::UFortBuildingItemDefinition* m_pRoofBuildDef;
SDK::ABuildingActor* m_pTrapC;
bool bTrapDone = false;
std::map<SDK::FGuid*, SDK::UFortWeaponItemDefinition*> m_mItems;
std::map<SDK::FGuid*, SDK::UFortTrapItemDefinition*> m_mTraps;
int iInventoryIteration = 0;
SDK::TArray<class SDK::UFortWorldItem*>* pItemInsts;
SDK::ABuildingPlayerPrimitivePreview* WallPreview;
SDK::ABuildingPlayerPrimitivePreview* FloorPreview;
SDK::ABuildingPlayerPrimitivePreview* StairPreview;
SDK::ABuildingPlayerPrimitivePreview* RoofPreview;
SDK::UStaticMesh* StaticWall;
SDK::UStaticMesh* StaticFloor;
SDK::UStaticMesh* StaticStair;
SDK::UStaticMesh* StaticRoof;
SDK::UBuildingEditModeMetadata_Wall* m_pMetadataWall;
SDK::UBuildingEditModeMetadata_Roof* m_pMetadataRoof;
SDK::UBuildingEditModeMetadata_Stair* m_pMetadataStair;
SDK::UBuildingEditModeMetadata_Floor* m_pMetadataFloor;
bool m_bHasCycledWall = false;
bool m_bHasCycledFloor = false;
bool m_bHasCycledStair = false;
bool m_bHasCycledRoof = false;
bool m_bHasCycledWallOnce = false;
bool m_bHasCycledFloorOnce = false;
bool m_bHasCycledStairOnce = false;
bool m_bHasCycledRoofOnce = false;
SDK::FString CurrentBuildingMat = TEXT("WOOD");
int m_iCurrentBuildPiece;
SDK::UClass* m_pLastBuildClassForWall;
SDK::UClass* m_pLastBuildClassForFloor;
SDK::UClass* m_pLastBuildClassForStair;
SDK::UClass* m_pLastBuildClassForRoof;
SDK::AFortInventory* FortInventory;

namespace Inventory {
    static inline void SetupInventory()
    {
        m_pEditToolDef = SDK::UObject::FindObject<SDK::UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");
        m_pWallBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
        m_pFloorBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");
        m_pStairBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        m_pRoofBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");
        auto pWood = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");
        auto pMetal = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition MetalItemData.MetalItemData");
        auto pStone = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition StoneItemData.StoneItemData");
        auto pRockets = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortAmmoItemDefinition AmmoDataRockets.AmmoDataRockets");
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pWood);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pMetal);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pStone);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pRockets);

        for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
        {
            auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
            if (pObject != nullptr && pObject->GetFullName().find("FortniteGame") == std::string::npos)
            {
                if (pObject->GetFullName().rfind("FortAmmoItemDefinition", 0) == 0)
                {
                    m_mItems[Util::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                }
                if (pObject->GetFullName().rfind("FortWeaponRangedItemDefinition", 0) == 0)
                {
                    m_mItems[Util::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponItemDefinition*>(pObject);
                }
                if (pObject->GetFullName().rfind("FortTrapItemDefinition", 0) == 0)
                {
                    auto guid = Util::GenerateGuidPtr();
                    m_mTraps[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                    m_mItems[guid] = reinterpret_cast<SDK::UFortTrapItemDefinition*>(pObject);
                }
                if (pObject->GetFullName().rfind("FortWeaponMeleeItemDefinition", 0) == 0)
                {
                    m_mItems[Util::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponMeleeItemDefinition*>(pObject);
                }
            }
        }

        auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        FortInventory = reinterpret_cast<WorldInventory*>(Cores::PlayerController)->WorldInventory;

        for (auto it = m_mItems.begin(); it != m_mItems.end(); it++)
        {
            SDK::UFortItem* Item;
            SDK::UFortWorldItem* WorldItem;

            if (it->second->IsA(SDK::UFortWeaponItemDefinition::StaticClass())) {
                Item = it->second->CreateTemporaryItemInstanceBP(1, 0);
                WorldItem = reinterpret_cast<SDK::UFortWorldItem*>(Item);
                WorldItem->ItemEntry.Count = 1;
            } else {
                Item = it->second->CreateTemporaryItemInstanceBP(999, 0);
                WorldItem = reinterpret_cast<SDK::UFortWorldItem*>(Item);
                WorldItem->ItemEntry.Count = 999;
            }

            WorldItem->SetOwningControllerForTemporaryItem(FortController);

            FortInventory->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
            FortInventory->Inventory.ItemInstances.Add(WorldItem);

            FortInventory->HandleInventoryLocalUpdate();
            static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->HandleWorldInventoryLocalUpdate();
        }

        auto PickaxeItem = PickaxeDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WorldPickaxeItem = reinterpret_cast<SDK::UFortWorldItem*>(PickaxeItem);
        WorldPickaxeItem->ItemEntry.Count = 1;
        FortInventory->Inventory.ReplicatedEntries.Add(WorldPickaxeItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WorldPickaxeItem);
        QuickBars->ServerAddItemInternal(WorldPickaxeItem->GetItemGuid(), SDK::EFortQuickBars::Primary, 0);

        auto WallBuildItem = m_pWallBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WallWorldBuildItem = reinterpret_cast<SDK::UFortWorldItem*>(WallBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(WallWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WallWorldBuildItem);
        QuickBars->ServerAddItemInternal(WallWorldBuildItem->GetItemGuid(), SDK::EFortQuickBars::Secondary, 0);
        WallGuid = WallWorldBuildItem->GetItemGuid();

        auto FloorBuildItem = m_pFloorBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto FloorWorldBuildItem = reinterpret_cast<SDK::UFortWorldItem*>(FloorBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(FloorWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(FloorWorldBuildItem);
        QuickBars->ServerAddItemInternal(FloorWorldBuildItem->GetItemGuid(), SDK::EFortQuickBars::Secondary, 1);
        FloorGuid = FloorWorldBuildItem->GetItemGuid();

        auto StairBuildItem = m_pStairBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto StairWorldBuildItem = reinterpret_cast<SDK::UFortWorldItem*>(StairBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(StairWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(StairWorldBuildItem);
        QuickBars->ServerAddItemInternal(StairWorldBuildItem->GetItemGuid(), SDK::EFortQuickBars::Secondary, 2);
        StairGuid = StairBuildItem->GetItemGuid();

        auto RoofBuildItem = m_pRoofBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto RoofWorldBuildItem = reinterpret_cast<SDK::UFortWorldItem*>(RoofBuildItem);
        FortInventory->Inventory.ReplicatedEntries.Add(RoofWorldBuildItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(RoofWorldBuildItem);
        QuickBars->ServerAddItemInternal(RoofWorldBuildItem->GetItemGuid(), SDK::EFortQuickBars::Secondary, 3);
        RoofGuid = RoofWorldBuildItem->GetItemGuid();

        FortInventory->HandleInventoryLocalUpdate();
        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->HandleWorldInventoryLocalUpdate();
    }

    static inline void SetupQuickbars() 
    {
        auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        QuickBars = reinterpret_cast<SDK::AFortQuickBars*>(Util::SpawnActor(SDK::AFortQuickBars::StaticClass(), SDK::FVector{ 0,0,3029 }, SDK::FRotator()));
        reinterpret_cast<AFortAsQuickBars*>(Cores::PlayerController)->QuickBars = QuickBars;
        QuickBars->SetOwner(Cores::PlayerController);

        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->OnRep_QuickBar();
        QuickBars->OnRep_PrimaryQuickBar();
        QuickBars->OnRep_SecondaryQuickBar();
    }

    static inline void UpdateInventory()
    {
        FortInventory->HandleInventoryLocalUpdate();
        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->HandleWorldInventoryLocalUpdate();
        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->OnRep_QuickBar();
        QuickBars->OnRep_PrimaryQuickBar();
        QuickBars->OnRep_SecondaryQuickBar();
    }

    static inline void ExecuteInventoryItem(SDK::FGuid* Guid)
    {
        auto ItemInstances = FortInventory->Inventory.ItemInstances;

        for (int i = 0; i < ItemInstances.Num(); i++)
        {
            auto ItemInstance = ItemInstances.operator[](i);

            if (Util::AreGuidsTheSame(ItemInstance->GetItemGuid(), (*Guid))) 
            {
                Cores::PlayerPawn->EquipWeaponDefinition((SDK::UFortWeaponItemDefinition*)ItemInstance->GetItemDefinitionBP(), (*Guid));
            }
        }
    }

    static void DropPickupAtLocation(SDK::UFortItemDefinition* ItemDef, int Count) 
    {
        auto FortPickup = reinterpret_cast<SDK::AFortPickupBackpack*>(Util::SpawnActor(SDK::AFortPickupBackpack::StaticClass(), Cores::PlayerPawn->K2_GetActorLocation(), SDK::FRotator()));
        FortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;
        FortPickup->PrimaryPickupItemEntry.Count = Count;
        FortPickup->OnRep_PrimaryPickupItemEntry();
        FortPickup->TossPickup(Cores::PlayerPawn->K2_GetActorLocation(), Cores::PlayerPawn, 999, true);
    }
}