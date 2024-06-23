#pragma once

#include <consoleapi.h>
#include <cstdio>
#include "SDK.hpp"
#include <memoryapi.h>
#include <inttypes.h>
#include <Windows.h>
#include <psapi.h>
#include <stdio.h>
#include <winscard.h>



using namespace SDK;

//globals before globals existed lol
namespace Cores {
    SDK::UFortEngine* FortEngine;
    SDK::UEngine* Engine;
    SDK::APlayerPawn_Generic_C* PlayerPawn;
    SDK::UGameplayStatics* GameplayStatics;
    SDK::UGameplayStatics* GPS;
    SDK::UWorld** World;
    SDK::ULevel* Level;
    SDK::TArray<SDK::AActor*>* Actors;
    SDK::UGameInstance* GameInstance;
    SDK::TArray<SDK::ULocalPlayer*> LocalPlayers;
    SDK::ULocalPlayer* LocalPlayer;
    SDK::APlayerController* PlayerController;
    SDK::UObject* (*StaticConstructObject_Internal)(SDK::UClass* InClass, SDK::UObject* InOuter, SDK::FName InName, int64_t InFlags, SDK::FUObjectItem::ObjectFlags InternalSetFlags, SDK::UObject* InTemplate, bool bCopyTransientsFromClassDefaults, void* InInstanceGraph, bool bAssumeTemplateIsArchetype);
}

static SDK::UObject* (*StaticLoadObject)(SDK::UClass* ObjectClass, SDK::UObject* InOuter, const TCHAR* InName, const TCHAR* Filename, uint32_t LoadFlags, SDK::UPackageMap* Sandbox, bool bAllowObjectReconciliation);
template<class T>
static T* LoadObject(const TCHAR* InPath)
{
    return (T*)StaticLoadObject(T::StaticClass(), nullptr, InPath, nullptr, 0, nullptr, false);
}

template <typename T>
static T* StaticFindObject(std::string ObjectName, SDK::UClass* ObjectClass = SDK::UObject::StaticClass())
{
    auto OrigInName = std::wstring(ObjectName.begin(), ObjectName.end()).c_str();

    auto StaticFindObject = (T * (*)(SDK::UClass*, SDK::UObject * Package, const wchar_t* OrigInName, bool ExactClass))((uintptr_t)GetModuleHandleA(0) + 0x142D2E0);
    return StaticFindObject(ObjectClass, nullptr, OrigInName, false);
}

class Util
{
public:
    static void InitGameplaystatics()
    {
        Cores::GameplayStatics = reinterpret_cast<SDK::UGameplayStatics*>(SDK::UGameplayStatics::StaticClass());
        //Cores::GPS = reinterpret_cast<SDK::UGameplayStatics*>(SDK::UGameplayStatics::StaticClass());
    }

    template<typename ReturnType = SDK::AActor>
    static ReturnType* SpawnActor(SDK::UClass* ActorClass, SDK::FVector Location, SDK::FRotator Rotation)
    {
        SDK::FQuat Quat;
        SDK::FTransform Transform;
        Quat.W = 0;
        Quat.X = Rotation.Pitch;
        Quat.Y = Rotation.Roll;
        Quat.Z = Rotation.Yaw;

        Transform.Rotation = Quat;
        Transform.Scale3D = SDK::FVector{ 1,1,1 };
        Transform.Translation = Location;

        auto Actor = Cores::GameplayStatics->STATIC_BeginSpawningActorFromClass((*Cores::World), ActorClass, Transform, false, nullptr);
        Cores::GameplayStatics->STATIC_FinishSpawningActor(Actor, Transform);
        return (ReturnType*)(Actor);
    }

    static SDK::UFortEngine* GetFortEngine()
    {
        printf("Searching FortEngine..\n");
        //FortEngine Transient.FortEngine_0
        //FortEngine FortniteGame.Default__FortEngine
        auto FortEngine = SDK::UObject::FindObject<SDK::UFortEngine>("FortEngine Transient.FortEngine_0");//backported from V2.
        if (FortEngine) 
        {
            printf("FortEngine was found!\n");
            return FortEngine;
        }
        else 
        {
            printf("No FortEngine found!\n");
            return nullptr;
        }
            
        
            
        //std::cout << "No FortEngine\n";
    }

    static FGameplayAbilitySpec GenerateAbilitySpec(UClass* Ability)
    {
        FGameplayAbilitySpecHandle SpecHandle{ rand() };

        FGameplayAbilitySpec AbilitySpec{ -1, -1, -1, SpecHandle, (UGameplayAbility*)Ability->DefaultObject, 1, -1, nullptr, 0, false, false, false };

        return AbilitySpec;
    }

    //credit?
    static void GrantAbility(SDK::UClass* GameplayAbilityClass)
    {
        printf("Start of GrantAbility!");
        auto AbilitySystemComponent = Cores::PlayerPawn->AbilitySystemComponent;
        static auto DefaultGameplayEffect = SDK::UObject::FindObject<SDK::UGameplayEffect>("GE_Constructor_ContainmentUnit_Applied_C GE_Constructor_ContainmentUnit_Applied.Default__GE_Constructor_ContainmentUnit_Applied_C");

        if (!DefaultGameplayEffect)
            return;

        SDK::TArray<SDK::FGameplayAbilitySpecDef> GrantedAbilities = DefaultGameplayEffect->GrantedAbilities;


        GrantedAbilities[0].Ability = GameplayAbilityClass;

        DefaultGameplayEffect->DurationPolicy = SDK::EGameplayEffectDurationType::Infinite;

        static auto GameplayEffectClass = SDK::UObject::FindClass("BlueprintGeneratedClass GE_Constructor_ContainmentUnit_Applied.GE_Constructor_ContainmentUnit_Applied_C");
        if (!GameplayEffectClass)
            return;

        auto handle = SDK::FGameplayEffectContextHandle();

        AbilitySystemComponent->BP_ApplyGameplayEffectToTarget(GameplayEffectClass, AbilitySystemComponent, 1, handle);

    }

    //crash on GameplayAbilities.Num()
    //cba to fix. sorry lmao
   static void GrantAbilities()
    {
        printf("Granting Abilities.\n");
        static auto AbilitySet = StaticFindObject<SDK::UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");
        ///static auto EmoteAbility = Actors::StaticFindObject<SDK::UClass>("/Game/Abilities/Player/Ninja/Perks/MantisLeap/GA_Ninja_MantisLeap.Ga_Ninja_MantisLeap");
        //("/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic");

        for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
        {
            auto Ability = AbilitySet->GameplayAbilities[i];

            GrantAbility(Ability);

        }


    }

    static bool AreGuidsTheSame(SDK::FGuid guidA, SDK::FGuid guidB)
    {
        if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
            return true;
        else
            return false;
    }

    static VOID InitConsole()
    {
        AllocConsole();

        FILE* pFile;
        freopen_s(&pFile, "CONOUT$", "w", stdout);
    }

    //credit to Polaris
    static VOID InitSdk()
    {
        auto pUWorldAddress = Util::FindPattern("\x48\x8B\x1D\x00\x00\x00\x00\x00\x00\x00\x10\x4C\x8D\x4D\x00\x4C", "xxx???????xxxx?x");
        auto pUWorldOffset = *reinterpret_cast<uint32_t*>(pUWorldAddress + 3);
        Cores::World = reinterpret_cast<SDK::UWorld**>(pUWorldAddress + 7 + pUWorldOffset);

        auto pGObjectAddress = Util::FindPattern("\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xD6", "xxx????x????x????x????xxx");
        auto pGObjectOffset = *reinterpret_cast<uint32_t*>(pGObjectAddress + 3);
        SDK::UObject::GObjects = reinterpret_cast<SDK::FUObjectArray*>(pGObjectAddress + 7 + pGObjectOffset);

        auto pGNameAddress = Util::FindPattern("\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x75\x50\xB9\x00\x00\x00\x00\x48\x89\x5C\x24", "xxx????xxxxxx????xxxx");
        auto pGNameOffset = *reinterpret_cast<uint32_t*>(pGNameAddress + 3);

        SDK::FName::GNames = *reinterpret_cast<SDK::TNameEntryArray**>(pGNameAddress + 7 + pGNameOffset);

        auto pStaticConstructObject_InternalOffset = FindPattern("\xE8\x00\x00\x00\x00\x89\x78\x38", "x????xxx");
        auto pStaticConstructObject_InternalAddress = pStaticConstructObject_InternalOffset + 5 + *reinterpret_cast<int32_t*>(pStaticConstructObject_InternalOffset + 1);

        Cores::StaticConstructObject_Internal = reinterpret_cast<decltype(Cores::StaticConstructObject_Internal)>(pStaticConstructObject_InternalAddress);
        StaticLoadObject = reinterpret_cast<decltype(StaticLoadObject)>(BaseAddress() + 0x142E560);
    }

    //credit to Polaris
    static VOID InitCores()
    {
        uintptr_t pBaseAddress = Util::BaseAddress();
        if (!pBaseAddress)
        {
            printf("Base Address failed!\n");
            exit(0);
        }

        if (!Cores::World)
        {
            printf("UWorld failed!\n");
            exit(0);
        }
        //Cores::FortEngine = GetFortEngine();
        Cores::Level = (*Cores::World)->PersistentLevel;
        Cores::GameInstance = (*Cores::World)->OwningGameInstance;
        Cores::LocalPlayers = Cores::GameInstance->LocalPlayers;
        Cores::LocalPlayer = Cores::LocalPlayers[0];
        Cores::Actors = &Cores::Level->Actors;
        Cores::PlayerController = Cores::LocalPlayer->PlayerController;
    }

    //credit to Polaris
    static VOID InitPatches()
    {
        auto pAbilityPatchAddress = Util::FindPattern
        (
            "\xC0\x0F\x84\x3C\x02\x00\x00\x0F\x2F\xF7\x0F\x86\xF5\x00\x00\x00",
            "xxxxxxxxxxxxxxxx"
        );
        if (pAbilityPatchAddress)
        {
            DWORD dwProtection;
            VirtualProtect(pAbilityPatchAddress, 16, PAGE_EXECUTE_READWRITE, &dwProtection);

            reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[2] = 0x85;
            reinterpret_cast<uint8_t*>(pAbilityPatchAddress)[11] = 0x8D;

            DWORD dwTemp;
            VirtualProtect(pAbilityPatchAddress, 16, dwProtection, &dwTemp);
        }
    }



    static uintptr_t BaseAddress()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    }

    static PBYTE FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
    {
        MODULEINFO info = { 0 };

        GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

        return Util::FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
    }

    static SDK::AActor* FindActor(SDK::UClass* pClass, int iSkip = 0) {
        for (int i = 0, j = 0; i < Cores::Actors->Num(); i++) {
            SDK::AActor* pActor = Cores::Actors->operator[](i);

            if (pActor != nullptr) {
                if (pActor->IsA(pClass)) {
                    if (j >= iSkip)
                        return pActor;
                    else {
                        j++;
                        continue;
                    }
                }
            }
        }

        return nullptr;
    }

    // Added for inv
    static SDK::FGuid* GenerateGuidPtr() {
        SDK::FGuid* Guid = new SDK::FGuid();
        Guid->A = rand() % 1000;
        Guid->B = rand() % 1000;
        Guid->C = rand() % 1000;
        Guid->D = rand() % 1000;
        return Guid;
    }

    static SDK::FGuid GenerateGuid() {
        SDK::FGuid Guid;
        Guid.A = rand() % 1000;
        Guid.B = rand() % 1000;
        Guid.C = rand() % 1000;
        Guid.D = rand() % 1000;
        return Guid;
    }
private:

    static BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
    {
        for (auto value = static_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
        {
            if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
                return false;
        }

        return true;
    }

    static PBYTE FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
    {
        dwSize -= static_cast<DWORD>(strlen(lpMask));

        for (auto i = 0UL; i < dwSize; ++i)
        {
            auto pAddress = static_cast<PBYTE>(pBase) + i;

            if (MaskCompare(pAddress, lpPattern, lpMask))
                return pAddress;
        }

        return NULL;
    }
};