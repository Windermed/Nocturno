#include <Windows.h>
#include "Util.h"
#include "minhook/MinHook.h"
#include "Inventory.h"
#include "HuskAI.h"

#pragma comment(lib, "minhook/minhook.lib")

PVOID(*CollectGarbageInternal)(uint32_t, bool) = nullptr;
PVOID CollectGarbageInternalHook(uint32_t KeepFlags, bool bPerformFullPurge)
{
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

bool bIsReady = false;
bool bHasSpawned = false;
bool bIsInGame = false;

PVOID(*ProcessEvent)(SDK::UObject*, SDK::UFunction*, PVOID) = nullptr;
PVOID ProcessEventHook(SDK::UObject* object, SDK::UFunction* function, PVOID params) 
{
    if (object && function) {
        if (function->GetName().find("StartButton") != std::string::npos) 
        {
            Cores::PlayerController->SwitchLevel(L"Zone_Onboarding_Suburban_a");
            bIsReady = true;
        }

        if (function->GetName().find("ReadyToStartMatch") != std::string::npos && bIsReady) 
        {
            Util::InitSdk();
            Util::InitCores();
            Util::InitPatches();

            printf("ReadyToStartMatch!\n");

            if (!bHasSpawned) {
                Cores::PlayerController->CheatManager->Slomo(0);
                Cores::PlayerController->CheatManager->Summon(L"PlayerPawn_Generic_C");
                Cores::PlayerPawn = reinterpret_cast<SDK::APlayerPawn_Generic_C*>(Util::FindActor(SDK::APlayerPawn_Generic_C::StaticClass()));
                if (!Cores::PlayerPawn)
                {
                    printf("PlayerPawn not found!\n");
                    return FALSE;
                }
                Cores::PlayerController->Possess(Cores::PlayerPawn);
                Cores::PlayerController->CheatManager->BugItGo(1, 1, 10000, 0, 0, 0);
                Cores::PlayerController->CheatManager->God();
                Cores::PlayerController->CheatManager->Slomo(1);
                printf("Pawn!\n");
                Cores::PlayerController->CheatManager->Summon(TEXT("FortGameStateZone"));
                printf("State!\n");

                auto FortPlayerController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
                auto FortGameMode = reinterpret_cast<SDK::AFortGameMode*>((*Cores::World)->AuthorityGameMode);
                auto FortPlayerState = reinterpret_cast<SDK::AFortPlayerState*>(Cores::PlayerController->PlayerState);
                FortPlayerState->OnRep_CharacterParts();
                Cores::PlayerPawn->OnCharacterPartsReinitialized();

                auto pickaxeDef = SDK::UObject::FindObject<SDK::UFortWeaponMeleeItemDefinition>("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_SR_T05.WID_Harvest_Pickaxe_SR_T05");
                m_pPickaxeDef = pickaxeDef;

                FortGameMode->StartMatch();
                printf("StartMatch!\n");
                FortGameMode->StartPlay();
                printf("StartPlay!\n");

                printf("Started mission!\n");

                bHasSpawned = true;
                bIsReady = false;
                bIsInGame = true;
            }
        }

        if (function->GetName().find("ServerExecuteInventoryItem") != std::string::npos && bIsInGame) 
        {
            SDK::FGuid* guid = reinterpret_cast<SDK::FGuid*>(params);
            Inventory::ExecuteInventoryItem(guid);
        }

        if (function->GetName().find("ServerAddItemInternal") != std::string::npos)
        {
            auto params1 = reinterpret_cast<SDK::AFortQuickBars_ServerAddItemInternal_Params*>(params);
            int slot = params1->Slot;
            SDK::EFortQuickBars quickbar = params1->InQuickBar;
            SDK::FGuid guid = params1->Item;
            if (slot != -1 && quickbar == SDK::EFortQuickBars::Secondary)
            {
                for (auto it = m_mItems.begin(); it != m_mItems.end(); it++)
                {
                    if (Util::AreGuidsTheSame((*it->first), guid))
                    {
                        SDK::FLinearColor color{ 100,100,100,100 };
                        auto m_pHud = SDK::UObject::FindObject<SDK::UAthenaHUD_C>("AthenaHUD_C Transient.FortEngine_1.FortGameInstance_1.AthenaHUD_C_1");
                        if (m_pHud != nullptr)
                        {
                            SDK::FSlateBrush brush = it->second->GetSmallPreviewImageBrush();
                            if (&brush)
                            {
                                m_pHud->QuickbarSecondary->QuickbarSlots[params1->Slot]->Empty->SetBrush(brush);
                                m_pHud->QuickbarSecondary->QuickbarSlots[params1->Slot]->Empty->SetColorAndOpacity(color);
                            }
                        }
                    }
                }
            }
        }

        if (function->GetName().find("Tick") != std::string::npos && bIsInGame) 
        {
            if (GetAsyncKeyState(VK_SPACE) && 0x01) {
                if (Cores::PlayerPawn->CanJump() && !Cores::PlayerPawn->IsJumpProvidingForce()) {
                    Cores::PlayerPawn->Jump();
                }
            }

            if (GetAsyncKeyState(VK_F1) && 0x01) {
                HuskAI::SpawnHusk();
            }

            Cores::PlayerPawn->CurrentMovementStyle = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->bWantsToSprint ? SDK::EFortMovementStyle::Walking : SDK::EFortMovementStyle::Sprinting;
        }

        if (function->GetName().find("ServerLoadingScreenDropped") != std::string::npos && bIsInGame)
        {
            Inventory::CreateBuildPreviews();
            Inventory::SetupInventory();
            Inventory::SetupQuickbars();
            Inventory::UpdateInventory();

            auto FortController = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController);
            auto FortCheatManager = reinterpret_cast<SDK::UFortCheatManager*>(Cores::PlayerController->CheatManager);
            FortCheatManager->CraftFree(); //Lets you craft anything
            FortCheatManager->BackpackSetSize(69420); //Funny
            FortCheatManager->GiveCheatInventory();
            FortCheatManager->EvolveHero();
            FortCheatManager->GiveAllWeapons();
            FortCheatManager->GiveResources(999);
            FortCheatManager->GiveUsefulThings(999);

            auto GCADDR = Util::FindPattern("\x48\x8B\xC4\x48\x89\x58\x08\x88\x50\x10", "xxxxxxxxxx");
            MH_CreateHook((LPVOID)(GCADDR), CollectGarbageInternalHook, (LPVOID*)(&CollectGarbageInternal));
            MH_EnableHook((LPVOID)(GCADDR));
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
    
    printf("\nCreated by Jacobb626 and Windermed!\n");

    MH_Initialize();

    Util::InitSdk();
    Util::InitCores();
    Util::InitPatches();
    Util::InitGameplaystatics();

    auto ProcessEventAddress = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x48\x63\x41\x0C", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxx");
    if (!ProcessEventAddress) {
        MessageBox(NULL, static_cast<LPCWSTR>(L"Finding pattern for ProcessEvent has failed, please re-open Fortnite and try again!"), static_cast<LPCWSTR>(L"Error"), MB_ICONERROR);
        ExitProcess(EXIT_FAILURE);
    }

    MH_CreateHook((LPVOID)(ProcessEventAddress), ProcessEventHook, (LPVOID*)(&ProcessEvent));
    MH_EnableHook((LPVOID)(ProcessEventAddress));

    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == DLL_PROCESS_ATTACH) 
    {
        CreateThread(0, 0, MainThread, mod, 0, 0);
    }

    return TRUE;
}
