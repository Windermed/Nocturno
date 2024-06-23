#pragma once

#include "SDK.hpp"
#include <map>
#include "Util.h"
#include "Building.h"
#include <iostream>

//Credit: Polaris
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

SDK::AFortQuickBars* QuickBars;
SDK::FGuid EditToolGuid;
SDK::FGuid PickaxeGuid;
SDK::FGuid WallGuid;
SDK::FGuid FloorGuid;
SDK::FGuid StairGuid;
SDK::FGuid RoofGuid;
SDK::UFortWeaponMeleeItemDefinition* PickaxeDef;
std::map<SDK::FGuid*, SDK::UFortWeaponItemDefinition*> Items;
std::vector<SDK::UFortItemDefinition*> AddedItems;
std::map<SDK::FGuid*, SDK::UFortTrapItemDefinition*> Traps;
SDK::AFortInventory* FortInventory;

namespace Inventory {

    //setups the Inventory.

    //Function is to be used whenever an inventory change is made.
    static inline void UpdateInventory()
    {
        FortInventory->HandleInventoryLocalUpdate();
        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->HandleWorldInventoryLocalUpdate();
        static_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->OnRep_QuickBar();
        QuickBars->OnRep_PrimaryQuickBar();
        QuickBars->OnRep_SecondaryQuickBar();
    }

    void AddItem(SDK::UFortItemDefinition* ItemDefinition, SDK::EFortQuickBars Quickbar, int Amount, int Slot)
    {
        printf("AddItem was called!\n");
        //LogInfo("Setting info...")
        auto Item = ItemDefinition->CreateTemporaryItemInstanceBP(1, 60);

        if (!Item) 
        {
            printf("Item was unable to be added!\n");
            //return;

        }

        std::cout << "[INVENTORY] Adding " << ItemDefinition->GetName() << " To the Inventory..\n";
        // LogInfo("Setting Temporary Owner to Item")
        auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        Item->SetOwningControllerForTemporaryItem(FortController);
        printf("Set Item to Controller!\n");
        auto WorldItem = reinterpret_cast<SDK::UFortWorldItem*>(Item);
        WorldItem->ItemEntry.Count = Amount;
        WorldItem->ItemEntry.Level = 60;
        WorldItem->ItemEntry.LoadedAmmo = Item->GetLoadedAmmo();
        WorldItem->ItemEntry.Durability = 300;
        FortInventory->Inventory.ReplicatedEntries.Add(WorldItem->ItemEntry);
        FortInventory->Inventory.ItemInstances.Add(WorldItem);
        QuickBars->ServerAddItemInternal(WorldItem->GetItemGuid(), Quickbar, Slot);
        std::cout << "[INVENTORY] Successfully Added " << ItemDefinition->GetName() << " To the Inventory!\n";
    }

    static void SetupBuilds()
    {
        auto EditToolDef = SDK::UObject::FindObject<SDK::UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");
        auto WallBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
        auto FloorBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");
        auto StairBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        auto RoofBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");

        auto WallBuildItem = WallBuildDef->CreateTemporaryItemInstanceBP(1, 0);
        auto WallWorldBuildItem = reinterpret_cast<SDK::UFortWorldItem*>(WallBuildItem);
        AddItem(EditToolDef, EFortQuickBars::Primary, 1, 1);
        AddItem(WallBuildDef, EFortQuickBars::Secondary, 1, 0);
        AddItem(FloorBuildDef, EFortQuickBars::Secondary, 1, 1);
        AddItem(StairBuildDef, EFortQuickBars::Secondary, 1, 2);
        AddItem(RoofBuildDef, EFortQuickBars::Secondary, 1, 3);
    }


    //starts up the QuickBars
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

    static inline void SetupInventory()
    {
        auto EditToolDef = SDK::UObject::FindObject<SDK::UFortEditToolItemDefinition>("FortEditToolItemDefinition EditTool.EditTool");
        auto WallBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall");
        auto FloorBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor");
        auto StairBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        auto RoofBuildDef = SDK::UObject::FindObject<SDK::UFortBuildingItemDefinition>("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS");
        auto pWood = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition WoodItemData.WoodItemData");
        auto pMetal = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition MetalItemData.MetalItemData");
        auto pStone = SDK::UObject::FindObject<SDK::UFortWeaponItemDefinition>("FortResourceItemDefinition StoneItemData.StoneItemData");
        auto Nocturno = UObject::FindObject<UFortWeaponRangedItemDefinition>("FortWeaponRangedItemDefinition WID_Assault_Auto_Founders_SR_Ore_T05.WID_Assault_Auto_Founders_SR_Ore_T05");
        auto Stormblade = UObject::FindObject<UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Edged_Sword_Medium_Laser_SR_Crystal_T06.WID_Edged_Sword_Medium_Laser_SR_Crystal_T06");
        auto pRockets = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortAmmoItemDefinition AmmoDataRockets.AmmoDataRockets");
        //john fortress is real

        auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
        FortInventory = reinterpret_cast<WorldInventory*>(Cores::PlayerController)->WorldInventory;

        AddItem(PickaxeDef, SDK::EFortQuickBars::Primary, 1, 0);
        AddItem(Nocturno, SDK::EFortQuickBars::Primary, 1, 1);
        AddItem(Stormblade, SDK::EFortQuickBars::Primary, 1, 2);
        AddItem(pWood, SDK::EFortQuickBars::Primary, 999, 0);
        AddItem(pMetal, SDK::EFortQuickBars::Primary, 999, 0);
        AddItem(pStone, SDK::EFortQuickBars::Primary, 999, 0);
        //AddItem(PickaxeDef, SDK::EFortQuickBars::Primary, 1, 0);

        SetupBuilds();

        UpdateInventory();
    }

    //allows the equipment of items.
    static inline void ExecuteInventoryItem(SDK::FGuid* Guid)
    {
        auto ItemInstances = FortInventory->Inventory.ItemInstances;

        for (int i = 0; i < ItemInstances.Num(); i++)
        {
            auto ItemInstance = ItemInstances.operator[](i);

            if (Util::AreGuidsTheSame(ItemInstance->GetItemGuid(), (*Guid))) 
            {
                auto Weapon = Cores::PlayerPawn->EquipWeaponDefinition((SDK::UFortWeaponItemDefinition*)ItemInstance->GetItemDefinitionBP(), (*Guid));
                auto ItemDefinition = ItemInstance->GetItemDefinitionBP();

                if (Weapon && Weapon->Class && Weapon->IsA(SDK::AFortWeap_BuildingTool::StaticClass())) {
                    SDK::AFortWeap_BuildingTool* BuildingTool = (SDK::AFortWeap_BuildingTool*)(Weapon);
                    if (Weapon->WeaponData)
                    {
                        Building::ShowBuildPreviews(BuildingTool, (SDK::UFortBuildingItemDefinition*)Weapon->WeaponData);
                    }
                }
                else if (ItemDefinition->IsA(UFortDecoItemDefinition::StaticClass())) // Trap Preview
                {
                    UFortDecoItemDefinition* DecoItemDefinition = (UFortDecoItemDefinition*)ItemDefinition;

                    if (!DecoItemDefinition)
                    {
                        printf("No decos? as they say!");
                        //printf("[AFortPlayerController::ServerExecuteInventoryItem] Failed to get DecoItemDefinition for PlayerController [%s].", PlayerController->GetName().c_str());
                        //return false;
                    }

                    Cores::PlayerPawn->PickUpActor(nullptr, DecoItemDefinition);
                    Cores::PlayerPawn->CurrentWeapon->ItemEntryGuid = ItemInstance->GetItemGuid();
                }
            }
        }
    }

    //drops items to the ground.
    static void DropPickupAtLocation(SDK::UFortItemDefinition* ItemDef, int Count) 
    {
        auto FortPickup = reinterpret_cast<SDK::AFortPickupBackpack*>(Util::SpawnActor(SDK::AFortPickupBackpack::StaticClass(), Cores::PlayerPawn->K2_GetActorLocation(), SDK::FRotator()));
        FortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;
        FortPickup->PrimaryPickupItemEntry.Count = Count;
        FortPickup->OnRep_PrimaryPickupItemEntry();
        FortPickup->TossPickup(Cores::PlayerPawn->K2_GetActorLocation(), Cores::PlayerPawn, 999, true);
    }

    static void AddItems(AFortPlayerController* InPC, int AmountOfItems = 10)
    {
        for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); i++)
        {
            auto obj = SDK::UObject::GetGlobalObjects().GetByIndex(i);
            if (obj)
            {
                if (obj->Class == UFortWeaponRangedItemDefinition::StaticClass() || obj->Class == UFortWeaponMeleeItemDefinition::StaticClass())
                {
                    AddedItems.push_back((UFortItemDefinition*)obj);
                }

                else if (obj->Class == UFortAmmoItemDefinition::StaticClass() || obj->Class == UFortTrapItemDefinition::StaticClass() || obj->Class == UFortConsumableItemDefinition::StaticClass())
                {
                    AddedItems.push_back((UFortItemDefinition*)obj);
                }
                else if (obj->Class == UFortIngredientItemDefinition::StaticClass() || obj->Class == UFortGadgetItemDefinition::StaticClass())
                {
                    AddedItems.push_back((UFortItemDefinition*)obj);
                }

            }
        }

        for (int i = 0; i < AmountOfItems; i++)//maximum of 50 items cuz yeah
        {
            auto Item = AddedItems[i];
            if (!Item) continue;
            if (Item->Class == UFortWeaponRangedItemDefinition::StaticClass() || Item->Class == UFortWeaponMeleeItemDefinition::StaticClass())
            {
                AddItem(Item, SDK::EFortQuickBars::Primary, AmountOfItems, 1);
            }

            else if (Item->Class == UFortIngredientItemDefinition::StaticClass())
            {
                AddItem(Item, SDK::EFortQuickBars::Primary, AmountOfItems, 1);
            }
            else if (Item->Class == UFortTrapItemDefinition::StaticClass())
            {
                AddItem(Item, SDK::EFortQuickBars::Primary, AmountOfItems, 1);
            }
            else if (Item->Class == UFortGadgetItemDefinition::StaticClass())
            {
                AddItem(Item, SDK::EFortQuickBars::Secondary, AmountOfItems, 1);
            }
            else if (Item->Class == UFortAmmoItemDefinition::StaticClass())
            {
                AddItem(Item, SDK::EFortQuickBars::Primary, AmountOfItems, 1);
            }
            else
            {
                AddItem(Item, SDK::EFortQuickBars::Primary, AmountOfItems, 1);
            }
        }
    }
}