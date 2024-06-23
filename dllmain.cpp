#include <Windows.h>
#include "Util.h"
#include "minhook/MinHook.h"
#include "Inventory.h"
#include "Building.h"
#include "HuskAI.h"
#include <ostream>
#include <iostream>

#pragma comment(lib, "minhook/minhook.lib")



bool bIsReady = false;
bool bHasSpawned = false;
bool bIsInGame = false;
bool hasInventorySetup = false;
SDK::FString MapName = L"Zone_Temperate_Suburban?game=zone"; //change the variable here to any map name you want to load onto.
// note: leave the "?game=zone" part unless you want to replace "=zone" with "=outpost"



PVOID(*CollectGarbageInternal)(uint32_t, bool) = nullptr;
PVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
{
    return NULL;
}

DWORD WINAPI InventoryThread(LPVOID)
{
    Inventory::SetupQuickbars();
    Inventory::SetupInventory();
    Inventory::UpdateInventory();

    return NULL;
}


template<typename T>
T* FindOffSet(const std::string& sClassName, const std::string& sQuery)
{
    for (int i = 0; i < SDK::UObject::GetGlobalObjects().Num(); ++i)
    {
        auto pObject = SDK::UObject::GetGlobalObjects().GetByIndex(i);
        if (pObject != nullptr && pObject->GetFullName().find("F_Med_Head1") == std::string::npos)
        {
            if (pObject->GetFullName().rfind(sClassName, 0) == 0 && pObject->GetFullName().find(sQuery) != std::string::npos)
                return static_cast<T*>(pObject);
        }
    }

    return nullptr;
}

//ProcessEvent. where the game executes functions.
PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;
PVOID ProcessEventHook(SDK::UObject* object, SDK::UFunction* function, PVOID params) 
{
    if (object && function) {
        if (function->GetName().find("StartButton") != std::string::npos)
        {
            // this is the map that it loads to
            //Cores::PlayerController->ClientTravel(L"Zone_Onboarding_FarmsteadFort?game=zone");
            Cores::PlayerController->ClientTravel(MapName, SDK::ETravelType::TRAVEL_Absolute, false, {});
            bIsReady = true;
        }

        if(function->GetName().find("ServerReturnToMainMenu") != std::string::npos)
        {
            // this is the map that it loads to
            //Cores::PlayerController->ClientTravel(L"Zone_Onboarding_FarmsteadFort?game=zone");
            Cores::PlayerController->ClientTravel(L"Frontend", SDK::ETravelType::TRAVEL_Absolute, false, {});
            bIsReady = false;
            bHasSpawned = false;
            bIsInGame = false;
            hasInventorySetup = false;
        }

        if (function->GetName().find("HandleStartingNewPlayer") != std::string::npos && bIsReady)
        {
            Util::InitSdk();
            Util::InitCores();
            SDK::InitGObjects();

            printf("HandleStartingNewPlayer!\n");

            if (!bHasSpawned) {
                // sets the game to summon the playerpawn
                SDK::UClass* PlayerPawn = nullptr;
                Cores::PlayerController->CheatManager->Slomo(0);
               // Cores::PlayerController->CheatManager->Summon(L"PlayerPawn_Generic_C");
                PlayerPawn = SDK::APlayerPawn_Generic_C::StaticClass();
                Cores::PlayerPawn = Util::SpawnActor<SDK::APlayerPawn_Generic_C>(PlayerPawn, { 0, 10000, 0 }, {});
                //Cores::PlayerPawn = reinterpret_cast<SDK::APlayerPawn_Generic_C*>(Util::FindActor(SDK::APlayerPawn_Generic_C::StaticClass()));
                if (!Cores::PlayerPawn)
                {
                    // prints if the playerpawn cannot be summoned for some unknown reason
                    printf("PlayerPawn not found!\n");
                    return FALSE;
                }
                // allows for the game to possess the pawn and sets the player pawn's location and state
                Cores::PlayerController->Pawn = Cores::PlayerPawn;
                Cores::PlayerController->AcknowledgedPawn = Cores::PlayerPawn;
                Cores::PlayerController->OnRep_Pawn();
                Cores::PlayerController->Possess(Cores::PlayerPawn);
                //Cores::PlayerController->CheatManager->BugItGo(-3600, -300, -1522, 0, 275, 0);
                //Cores::PlayerController->MyFortPawn
                Cores::PlayerController->CheatManager->God();
                Cores::PlayerController->CheatManager->Slomo(1);

                LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Abilities/Player/Constructor/Perks/ContainmentUnit/GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");
                LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Abilities/Player/Constructor/Perks/Default/GE_Constructor_IsConstructor.GE_Constructor_IsConstructor_C");
                LoadObject<SDK::UFortAbilityKit>(L"/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");
                printf("Pawn!\n");
                // sets the game state for the game, which is zone
                //Cores::PlayerController->CheatManager->Summon(TEXT("FortGameStateZone"));
                printf("State!\n");

                // variables for the stw gamestate
                auto FortPlayerController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
                auto FortGameMode = reinterpret_cast<SDK::AFortGameModeZone*>((*Cores::World)->AuthorityGameMode);
                auto FortPlayerState = reinterpret_cast<SDK::AFortPlayerStateZone*>(Cores::PlayerController->PlayerState);
                auto FortGameState = reinterpret_cast<SDK::AFortGameStateZone*>((*Cores::World)->GameState);
                FortPlayerState->bIsGameSessionOwner = true;
                FortPlayerState->OnRep_SessionOwner();
                FortPlayerState->OnRep_CharacterParts();
                Cores::PlayerPawn->OnCharacterPartsReinitialized();

                FortGameMode->GameState = FortGameState;
               // FortGameState->GameState

                FortPlayerState->bHasStartedPlaying = true;
                FortPlayerState->bIsReadyToContinue = true;

                auto pickaxeDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SR_T06.WID_Harvest_Pickaxe_SR_T06");
                PickaxeDef = pickaxeDef;

                printf("Setting up building..\n");
                Building::Initialize();
                printf("Building successfully loaded into memory!\n");
               
                Inventory::SetupQuickbars();
                printf("Quickbars successfully created!\n");
                Inventory::SetupInventory();
                printf("Inventory successfully created!\n");
                FortGameMode->StartMatch();
                printf("StartMatch was called!\n");
                FortGameMode->StartPlay();
                FortPlayerController->ServerSetClientHasFinishedLoading(true);
                printf("Client loaded!\n");
                FortPlayerController->bClientPawnIsLoaded = true;
                printf("Client Finished!\n");
                FortPlayerController->bHasClientFinishedLoading = true;
                printf("Server Finished!\n");
                FortPlayerController->bHasServerFinishedLoading = true;
                printf("OnRepping changes!!\n");
                FortPlayerController->OnRep_bHasServerFinishedLoading();
                // sets the pickaxe for the player pawn
             
               

                bHasSpawned = true;
                bIsReady = false;
                bIsInGame = true;

                //FortPlayerController->
            }

        }

        // game executes these functions if ReadyToStartMatch is called!
        if (function->GetName().find("ReadyToStartMatch") != std::string::npos && bIsReady)
        {
            Util::InitSdk();
            Util::InitCores();
            //Util::InitPatches();

            printf("ReadyToStartMatch!\n");


            auto FortGameMode = reinterpret_cast<SDK::AFortGameModeZone*>((*Cores::World)->AuthorityGameMode);

            FortGameMode->bWorldIsReady = true;
            printf("Successfully set world to ready!\n");
            //DidMatchStart = true;
        }

        if (function->GetName().find("ServerExecuteInventoryItem") != std::string::npos && bIsInGame) 
        {
            SDK::FGuid* guid = reinterpret_cast<SDK::FGuid*>(params);
            Inventory::ExecuteInventoryItem(guid);
        }

        if (function->GetName().find("ServerRemoveInventoryItem") != std::string::npos && bIsInGame)
            if (function->GetName().find("ServerAttemptInventoryDrop") != std::string::npos && bIsInGame)
            {
                struct Params_
                {
                    SDK::FGuid ItemGuid;
                    int Count;
                };
                auto Params = (Params_*)(params);
                auto ItemInstances = FortInventory->Inventory.ItemInstances;
                auto QuickbarSlots = QuickBars->PrimaryQuickBar.Slots;
                for (int i = 0; i < ItemInstances.Num(); i++)
                {
                    auto ItemInstance = ItemInstances.operator[](i);
                    if (Util::AreGuidsTheSame(Params->ItemGuid, ItemInstance->GetItemGuid()))
                    {
                        Inventory::DropPickupAtLocation(ItemInstance->GetItemDefinitionBP(), Params->Count);
                    }
                }

                for (int i = 0; i < QuickbarSlots.Num(); i++)
                    for (int i = 0; i < QuickbarSlots.Num(); i++)
                    {
                        if (Util::AreGuidsTheSame(QuickbarSlots[i].Items[0], Params->ItemGuid))
                            if (Util::AreGuidsTheSame(QuickbarSlots[i].Items[0], Params->ItemGuid))
                            {
                                QuickBars->EmptySlot(SDK::EFortQuickBars::Primary, i);
                                Inventory::UpdateInventory();
                            }
                    }
            }

        if (function->GetName().find("ServerHandlePickup") != std::string::npos && bIsInGame)
        {
            struct ServerHandlePickupParams
            {
                SDK::UFortItemDefinition* Pickup;
                float InFlyTime;
                SDK::FVector InStartDirection;
                bool bPlayPickupSound;
            };

            auto Params = (ServerHandlePickupParams*)(params);

            Inventory::AddItem(Params->Pickup, EFortQuickBars::Primary, 1, 1);
            Inventory::UpdateInventory();
        }

        if (function->GetName().find("Tick") != std::string::npos && bIsInGame) 
        {
            //Jumping (scuffed)
            if (GetAsyncKeyState(VK_SPACE) && 0x01) {
                if (Cores::PlayerPawn->CanJump() && !Cores::PlayerPawn->IsJumpProvidingForce()) {
                    Cores::PlayerPawn->Jump();
                }
            }

            if (GetAsyncKeyState(VK_SHIFT) && 0x01) {
                Cores::PlayerPawn->CurrentMovementStyle = SDK::EFortMovementStyle::Sprinting;
            }

            // WIP HuskAI code
            if (GetAsyncKeyState(VK_F1) && 0x01) {
                HuskAI::SpawnHusk();
            }

            if (GetAsyncKeyState(VK_F10) && 0x01 && !hasInventorySetup) {
                printf("Adding all items.. (This may take a while!)\n");
                Inventory::AddItems((AFortPlayerController*)Cores::PlayerController, 35);
                printf("Items have been sucessfully added!\n");
                hasInventorySetup = true;
            }

            if (GetAsyncKeyState(VK_F10) && 0x01 && hasInventorySetup) {
                printf("Items have already been added to Inventory!\n");
            }

            Cores::PlayerPawn->CurrentMovementStyle = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->bWantsToSprint ? SDK::EFortMovementStyle::Walking : SDK::EFortMovementStyle::Sprinting;
        }

        if (function->GetName().find("ServerLoadingScreenDropped") != std::string::npos && bIsInGame)
        {
            Util::InitSdk();
            Util::InitCores();
           // Util::InitPatches();
            printf("ServerLoadingScreenDropped!\n");
           // printf("Beginning to grant abilities..\n");
           // not gonna bother fixing this. would rather work on NocturnoV3 lol.
           // the crash is likely due to the SDK used. replace it with a new one and it might work? 
           // - Windermed 6/23/2024
           // Util::GrantAbilities();

            printf("Loading Every Husk to memory..\n");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Shielder/Blueprints/ShielderPawn.ShielderPawn_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Smasher/Blueprints/SmasherPawn.SmasherPawn_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Flinger/Blueprints/FlingerPawn.FlingerPawn_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Taker/Blueprints/TakerPawn.TakerPawn_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/BlasterPawn.BlasterPawn_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Beehive.HuskPawn_Beehive_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell_Poison.HuskPawn_Bombshell_Poison_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Bombshell.HuskPawn_Bombshell_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Dwarf.HuskPawn_Dwarf_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Husky.HuskPawn_Husky_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Pitcher.HuskPawn_Pitcher_C");
            LoadObject<SDK::UBlueprintGeneratedClass>(L"/Game/Characters/Enemies/Husk/Blueprints/HuskPawn_Sploder.HuskPawn_Sploder_C");
            printf("Successfully loaded Every Husk to memory!\n");
           // printf("All Abilities successfully granted!\n");
            //CreateThread(0, 0, InventoryThread, 0, 0, 0);
            printf("Inventory created! please press F10 in-game to spawn inventory items!\n");
            printf("NOTE: This may take a while. It may take a minute or so for all items to be granted.\n");
            auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
            auto FortCheatManager = reinterpret_cast<SDK::UFortCheatManager*>(Cores::PlayerController->CheatManager);
            auto FortGameMode = reinterpret_cast<SDK::AFortGameModeZone*>((*Cores::World)->AuthorityGameMode);
            FortCheatManager->CraftFree(); //Lets you craft anything but it doesn't work as of right now
            FortCheatManager->BackpackSetSize(69420); //funny number go brr
            FortCheatManager->ToggleInfiniteAmmo();
            FortCheatManager->ToggleInfiniteStamina();
            //FortCheatManager->GiveCheatInventory(); // gives the player all of the items in-game but cmd is stripped
            FortCheatManager->EvolveHero(); //Evolves the hero i hope - stripped
             printf("Started mission!\n");

            /*auto GCADDR = Util::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");
            MH_CreateHook((LPVOID)(GCADDR), CollectGarbageInternalHook, (LPVOID*)(&CollectGarbageInternal));
            MH_EnableHook((LPVOID)(GCADDR));*/
        }
    }

    return ProcessEvent(object, function, params);
}

DWORD WINAPI MainThread(LPVOID) 
{
    Util::InitConsole();

    auto idk = R"(    _   __           __                       
   / | / /___  _____/ /___  ___________  ____ 
  /  |/ / __ \/ ___/ __/ / / / ___/ __ \/ __ \
 / /|  / /_/ / /__/ /_/ /_/ / /  / / / / /_/ /
/_/ |_/\____/\___/\__/\__,_/_/  /_/ /_/\____/ 
                                             )";
    printf(idk);
    printf(" \n");
    printf("\nNOTE: This Version of Nocturno is Discontinued and Unsupported! Please go to https://discord.gg/nocturno\n");
    printf("\nIf that link does not work, join through here: https://discord.gg/hDCe7KJPgt\n");
    
    printf("\nCreated by Jacobb626 and Windermed!\n");
    printf("\nGo to the Map and select any mission to load in, have fun!\n");
    SetConsoleTitle(L"NocturnoV1 Remastered [2024 REVISED BUILD]");
    MH_Initialize();

    Util::InitSdk();
    Util::InitCores();
    //Util::InitPatches();
    Util::InitGameplaystatics();



    auto ProcessEventAddress = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
    if (!ProcessEventAddress) {
        MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for ProcessEvent has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    printf("Initiating Console!\n");
    auto Console = SDK::UConsole::StaticClass()->CreateDefaultObject<SDK::UConsole>();
    Console->Outer = Cores::LocalPlayer->ViewportClient;

    Cores::LocalPlayer->ViewportClient->ViewportConsole = Console;
    printf("Console successfully initialized!\n");

    MH_CreateHook((LPVOID)(ProcessEventAddress), ProcessEventHook, (LPVOID*)(&ProcessEvent));
    MH_EnableHook((LPVOID)(ProcessEventAddress));

    return TRUE;
}

// basic dll injection shit lol
BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH) 
    {
        CreateThread(0, 0, MainThread, mod, 0, 0);
    }

    return TRUE;
}
