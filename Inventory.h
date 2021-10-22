#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"
#include <iostream>

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
struct AFortAsBuildPreviewMID
{
public:
    unsigned char UnknownData00[0x1928];
    class SDK::UMaterialInstanceDynamic* BuildPreviewMarkerMID;
};
struct AFortAsBuildPreview
{
public:
    unsigned char UnknownData00[0x1788];
    class SDK::ABuildingPlayerPrimitivePreview* BuildPreviewMarker;
};

SDK::AFortQuickBars* QuickBars;
SDK::FGuid* m_pgEditToolDef;
SDK::FGuid* m_pgPickaxe;
SDK::FGuid* m_pgWallBuild;
SDK::FGuid* m_pgFloorBuild;
SDK::FGuid* m_pgStairBuild;
SDK::FGuid* m_pgRoofBuild;
SDK::UFortEditToolItemDefinition* m_pEditToolDef;
SDK::UFortWeaponMeleeItemDefinition* m_pPickaxeDef;
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
        m_pgEditToolDef = Util::GenerateGuidPtr();
        m_pgPickaxe = Util::GenerateGuidPtr();
        m_pgWallBuild = Util::GenerateGuidPtr();
        m_pgFloorBuild = Util::GenerateGuidPtr();
        m_pgStairBuild = Util::GenerateGuidPtr();
        m_pgRoofBuild = Util::GenerateGuidPtr();
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pWood);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pMetal);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pStone);
        m_mItems.insert_or_assign(Util::GenerateGuidPtr(), pRockets);
        m_mItems.insert_or_assign(m_pgPickaxe, m_pPickaxeDef);
        m_mItems.insert_or_assign(m_pgWallBuild, m_pWallBuildDef);
        m_mItems.insert_or_assign(m_pgFloorBuild, m_pFloorBuildDef);
        m_mItems.insert_or_assign(m_pgStairBuild, m_pStairBuildDef);
        m_mItems.insert_or_assign(m_pgRoofBuild, m_pRoofBuildDef);
        m_mItems.insert_or_assign(m_pgEditToolDef, m_pEditToolDef);
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
                if (pObject->GetFullName().rfind("FortMeleeItemDefinition", 0) == 0)
                {
                    m_mItems[Util::GenerateGuidPtr()] = reinterpret_cast<SDK::UFortWeaponMeleeItemDefinition*>(pObject);
                }
            }
        }

        auto controller = static_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        auto winventory = reinterpret_cast<WorldInventory*>(controller)->WorldInventory;
        if (winventory)
        {
            SDK::FFortItemList* inv = &winventory->Inventory;
            pItemInsts = &inv->ItemInstances;
            pItemInsts->Count = m_mItems.size();
            pItemInsts->Max = m_mItems.size();
            pItemInsts->Data = (class SDK::UFortWorldItem**)::malloc(pItemInsts->Count * sizeof(SDK::UFortWorldItem*));
            for (auto it = m_mItems.begin(); it != m_mItems.end(); it++) {
                auto pItemEntry = new SDK::FFortItemEntry;
                if (it->second->IsA(SDK::UFortWeaponItemDefinition::StaticClass()))
                    pItemEntry->Count = 1;
                else
                    pItemEntry->Count = 100;
                pItemEntry->ItemDefinition = it->second;
                pItemEntry->Durability = it->second->GetMaxDurability(it->second->MaxLevel);
                pItemEntry->Level = it->second->MaxLevel;
                pItemEntry->ItemGuid = (*it->first);
                pItemEntry->bIsDirty = false;
                pItemEntry->bIsReplicatedCopy = true;
                pItemEntry->LoadedAmmo = 0;

                auto pWorldItem = reinterpret_cast<SDK::UFortWorldItem*>(Cores::StaticConstructObject_Internal(SDK::UFortWorldItem::StaticClass(), winventory, SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
                pWorldItem->bTemporaryItemOwningController = true;
                pWorldItem->SetOwningControllerForTemporaryItem(controller);
                reinterpret_cast<OwnerInventory*>(pWorldItem)->OwnerInventory = winventory;
                pWorldItem->ItemEntry = *pItemEntry;
                pItemInsts->operator[](iInventoryIteration) = pWorldItem;
                auto statval = new SDK::FFortItemEntryStateValue;
                statval->IntValue = 1;
                statval->NameValue = SDK::FName("Item");
                statval->StateType = SDK::EFortItemEntryState::NewItemCount;
                controller->ServerSetInventoryStateValue((*it->first), (*statval));
                iInventoryIteration++;
            }
        }
	}

    static inline void SetupQuickbars() 
    {
        auto controller = static_cast<SDK::AFortPlayerControllerAthena*>(Cores::PlayerController);
        auto asfortquickbars = reinterpret_cast<AFortAsQuickBars*>(Cores::PlayerController);
        Cores::PlayerController->CheatManager->Summon(TEXT("FortQuickBars"));
        asfortquickbars->QuickBars = static_cast<SDK::AFortQuickBars*>(Util::FindActor(SDK::AFortQuickBars::StaticClass()));
        asfortquickbars->QuickBars->SetOwner(Cores::PlayerController);
        auto pQuickBars = static_cast<SDK::AFortQuickBars*>(Util::FindActor(SDK::AFortQuickBars::StaticClass()));
        QuickBars = pQuickBars;
        controller->Role = SDK::ENetRole::ROLE_None;
        controller->OnRep_QuickBar();
        controller->Role = SDK::ENetRole::ROLE_Authority;
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 0);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 1);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 2);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 3);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 4);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Secondary, 5);
        asfortquickbars->QuickBars->EnableSlot(SDK::EFortQuickBars::Primary, 0);
        pQuickBars->ServerAddItemInternal((*m_pgWallBuild), SDK::EFortQuickBars::Secondary, 0);
        pQuickBars->ServerAddItemInternal((*m_pgFloorBuild), SDK::EFortQuickBars::Secondary, 1);
        pQuickBars->ServerAddItemInternal((*m_pgStairBuild), SDK::EFortQuickBars::Secondary, 2);
        pQuickBars->ServerAddItemInternal((*m_pgRoofBuild), SDK::EFortQuickBars::Secondary, 3);
        pQuickBars->ServerAddItemInternal((*m_pgPickaxe), SDK::EFortQuickBars::Primary, 0);
        pQuickBars->ServerActivateSlotInternal(SDK::EFortQuickBars::Primary, 0, 0, true);
    }

    static inline void ShowBuildingPreview(SDK::EFortBuildingType);

    static inline void ExecuteInventoryItem(SDK::FGuid* Guid) 
    {
        for (auto it = m_mItems.begin(); it != m_mItems.end(); it++)
        {
            if (Util::AreGuidsTheSame((*it->first), (*Guid)))
            {
                Cores::PlayerPawn->EquipWeaponDefinition(it->second, (*it->first));
            }
        }
        for (auto it = m_mTraps.begin(); it != m_mTraps.end(); it++)
        {
            if (Util::AreGuidsTheSame((*it->first), (*Guid)))
            {
                if (!bTrapDone)
                {
                    m_pTrapC = nullptr;
                    bTrapDone = true;
                }
                Cores::PlayerPawn->PickUpActor(m_pTrapC, it->second);
                Cores::PlayerPawn->CurrentWeapon->ItemEntryGuid = (*it->first);
            }
        }
        if (Util::AreGuidsTheSame((*Guid), (*m_pgFloorBuild)) || Util::AreGuidsTheSame((*Guid), (*m_pgWallBuild)) || Util::AreGuidsTheSame((*Guid), (*m_pgRoofBuild)) || Util::AreGuidsTheSame((*Guid), (*m_pgStairBuild)))
        {
            auto CheatManager = reinterpret_cast<SDK::UFortCheatManager*>(Cores::PlayerController->CheatManager);
            CheatManager->BuildFree();
            CheatManager->BuildWith(L"Wood");
        }
    }

    static inline void CreateBuildPreviews() 
    {
        SDK::AFortPlayerController* playerController = static_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        RoofPreview = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(Util::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass()));
        auto pBuildingEditSupportRoof = reinterpret_cast<SDK::UBuildingEditModeSupport_Roof*>(Cores::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Roof::StaticClass(), (*Cores::World), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
        pBuildingEditSupportRoof->Outer = RoofPreview;

        RoofPreview->EditModeSupport = pBuildingEditSupportRoof;

        auto pComponent = RoofPreview->GetBuildingMeshComponent();

        if (!StaticRoof)
        {
            StaticRoof = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_RoofC.PBW_W1_RoofC");
        }

        pComponent->SetStaticMesh(StaticRoof);
        pComponent->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(Cores::PlayerController)->BuildPreviewMarkerMID);

        RoofPreview->BuildingType = SDK::EFortBuildingType::Roof;

        if (!m_pMetadataRoof)
        {
            m_pMetadataRoof = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Roof>("BuildingEditModeMetadata_Roof EMP_Roof_RoofC.EMP_Roof_RoofC");
        }

        RoofPreview->EditModePatternData = m_pMetadataRoof;
        RoofPreview->EditModeSupportClass = SDK::UBuildingEditModeSupport_Roof::StaticClass();
        RoofPreview->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);


        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        StairPreview = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(Util::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 1));
        auto pBuildingEditSupportStair = reinterpret_cast<SDK::UBuildingEditModeSupport_Stair*>(Cores::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Stair::StaticClass(), (*Cores::World), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
        pBuildingEditSupportStair->Outer = StairPreview;

        StairPreview->EditModeSupport = pBuildingEditSupportStair;

        auto pComponent1 = StairPreview->GetBuildingMeshComponent();

        if (!StaticStair)
        {
            StaticStair = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_StairW.PBW_W1_StairW");
        }

        pComponent1->SetStaticMesh(StaticStair);
        pComponent1->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(Cores::PlayerController)->BuildPreviewMarkerMID);

        StairPreview->BuildingType = SDK::EFortBuildingType::Stairs;

        if (!m_pMetadataStair)
        {
            m_pMetadataStair = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Stair>("BuildingEditModeMetadata_Stair EMP_Stair_StairW.EMP_Stair_StairW");
        }

        StairPreview->EditModePatternData = m_pMetadataStair;
        StairPreview->EditModeSupportClass = SDK::UBuildingEditModeSupport_Stair::StaticClass();
        StairPreview->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);


        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        FloorPreview = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(Util::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 2));
        auto pBuildingEditSupportFloor = reinterpret_cast<SDK::UBuildingEditModeSupport_Floor*>(Cores::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Floor::StaticClass(), (*Cores::World), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
        pBuildingEditSupportFloor->Outer = reinterpret_cast<AFortAsBuildPreview*>(Cores::PlayerController)->BuildPreviewMarker;

        FloorPreview->EditModeSupport = pBuildingEditSupportFloor;

        auto pComponent2 = FloorPreview->GetBuildingMeshComponent();

        if (!StaticFloor)
        {
            StaticFloor = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_Floor.PBW_W1_Floor");
        }

        pComponent2->SetStaticMesh(StaticFloor);
        pComponent2->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(Cores::PlayerController)->BuildPreviewMarkerMID);

        FloorPreview->BuildingType = SDK::EFortBuildingType::Floor;

        if (!m_pMetadataFloor)
        {
            m_pMetadataFloor = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Floor>("BuildingEditModeMetadata_Floor EMP_Floor_Floor.EMP_Floor_Floor");
        }

        FloorPreview->EditModePatternData = m_pMetadataFloor;
        FloorPreview->EditModeSupportClass = SDK::UBuildingEditModeSupport_Floor::StaticClass();
        FloorPreview->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);


        playerController->CheatManager->Summon(TEXT("BuildingPlayerPrimitivePreview"));
        WallPreview = static_cast<SDK::ABuildingPlayerPrimitivePreview*>(Util::FindActor(SDK::ABuildingPlayerPrimitivePreview::StaticClass(), 3));
        auto pBuildingEditSupportWall = reinterpret_cast<SDK::UBuildingEditModeSupport_Wall*>(Cores::StaticConstructObject_Internal(SDK::UBuildingEditModeSupport_Wall::StaticClass(), (*Cores::World), SDK::FName("None"), 0, SDK::FUObjectItem::ObjectFlags::None, NULL, false, NULL, false));
        pBuildingEditSupportWall->Outer = WallPreview;

        WallPreview->EditModeSupport = pBuildingEditSupportWall;

        auto pComponent3 = WallPreview->GetBuildingMeshComponent();

        if (!StaticWall)
        {
            StaticWall = SDK::UObject::FindObject<SDK::UStaticMesh>("StaticMesh PBW_W1_Solid.PBW_W1_Solid");
        }

        pComponent3->SetStaticMesh(StaticWall);
        pComponent3->SetMaterial(0, reinterpret_cast<AFortAsBuildPreviewMID*>(Cores::PlayerController)->BuildPreviewMarkerMID);

        WallPreview->BuildingType = SDK::EFortBuildingType::Wall;

        if (!m_pMetadataWall)
        {
            m_pMetadataWall = SDK::UObject::FindObject<SDK::UBuildingEditModeMetadata_Wall>("BuildingEditModeMetadata_Wall EMP_Wall_Solid.EMP_Wall_Solid");
        }

        WallPreview->EditModePatternData = m_pMetadataWall;
        WallPreview->EditModeSupportClass = SDK::UBuildingEditModeSupport_Wall::StaticClass();
        WallPreview->OnBuildingActorInitialized(SDK::EFortBuildingInitializationReason::PlacementTool, SDK::EFortBuildingPersistentState::New);

        WallPreview->SetActorHiddenInGame(true);
        StairPreview->SetActorHiddenInGame(true);
        FloorPreview->SetActorHiddenInGame(true);
        RoofPreview->SetActorHiddenInGame(true);
    }

    static inline void ShowBuildingPreview(SDK::EFortBuildingType type) 
    {
        switch(type) 
        {
        case SDK::EFortBuildingType::Wall:
            WallPreview->SetActorHiddenInGame(false);
            StairPreview->SetActorHiddenInGame(true);
            FloorPreview->SetActorHiddenInGame(true);
            RoofPreview->SetActorHiddenInGame(true);
            break;

        case SDK::EFortBuildingType::Floor:
            WallPreview->SetActorHiddenInGame(true);
            StairPreview->SetActorHiddenInGame(true);
            FloorPreview->SetActorHiddenInGame(false);
            RoofPreview->SetActorHiddenInGame(true);
            break;

        case SDK::EFortBuildingType::Roof:
            WallPreview->SetActorHiddenInGame(true);
            StairPreview->SetActorHiddenInGame(true);
            FloorPreview->SetActorHiddenInGame(true);
            RoofPreview->SetActorHiddenInGame(false);
            break;

        case SDK::EFortBuildingType::Stairs:
            WallPreview->SetActorHiddenInGame(true);
            StairPreview->SetActorHiddenInGame(false);
            FloorPreview->SetActorHiddenInGame(true);
            RoofPreview->SetActorHiddenInGame(true);
            break;

        case SDK::EFortBuildingType::None:
            WallPreview->SetActorHiddenInGame(true);
            StairPreview->SetActorHiddenInGame(true);
            FloorPreview->SetActorHiddenInGame(true);
            RoofPreview->SetActorHiddenInGame(true);
            break;
        }
    }
}