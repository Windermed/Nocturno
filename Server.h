#pragma once

#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include "nlohmann/json.hpp"
#include "PlayerStruct.h"

#pragma comment(lib, "ws2_32.lib") 

using json = nlohmann::json;

SOCKET Socket;

int LocalPlayerId;

void __stdcall ReadingThread(LPVOID param)
{
	SOCKET s = reinterpret_cast<SOCKET>(param);
	char Buffer[512];

	while (true) {
		const int iResult = recv(s, Buffer, 512, 0);

		if (iResult <= 0) break;

		std::cout << "[SERVER] " << Buffer << std::endl;

		json j;

		try {
			j = j.parse(Buffer);
		} catch(...) {
			printf("Failed to parse json from server!\n");
		}

		std::string function = j["Function"];

		if (function == "ClientConnection") {
			auto newPlayer = new Player();
			newPlayer->Controller = reinterpret_cast<SDK::APlayerController*>(Util::SpawnActor(SDK::APlayerController::StaticClass(), SDK::FVector{0,0,2139}, SDK::FRotator()));
			newPlayer->Pawn = reinterpret_cast<SDK::APlayerPawn_Generic_C*>(Util::SpawnActor(SDK::APlayerPawn_Generic_C::StaticClass(), SDK::FVector{ 0,0,10000 }, SDK::FRotator()));
			newPlayer->DisplayName = j["Params"]["Name"];
			newPlayer->PlayerId = j["Params"]["Id"];
			Players->Add(newPlayer);

			auto pSkeletalMesh = SDK::UObject::FindObject<SDK::USkeletalMesh>("SkeletalMesh F_SML_Starter_Epic.F_SML_Starter_Epic");
			newPlayer->Pawn->Mesh->SetSkeletalMesh(pSkeletalMesh, true);

			newPlayer->Controller->Possess(newPlayer->Pawn);
		} else if (function == "ClientExit") {
			auto player = FindPlayerById(j["Params"]["Id"]);
			player->Controller->UnPossess();
		} else if (function == "SetTransform") {
			auto player = FindPlayerById(j["Params"]["Id"]);
			auto newLocX = j["Params"]["Loc"]["X"];
			auto newLocY = j["Params"]["Loc"]["Y"];
			auto newLocZ = j["Params"]["Loc"]["Z"];
			auto newRotPitch = j["Params"]["Rot"]["Pitch"];
			auto newRotRoll = j["Params"]["Rot"]["Roll"];
			auto newRotYaw = j["Params"]["Rot"]["Yaw"];
			auto newLocVector = SDK::FVector{ newLocX, newLocY, newLocZ };
			auto newRotVector = SDK::FRotator{ newRotPitch, newRotYaw, newRotRoll };

			player->Controller->AddPitchInput(newRotPitch);
			player->Controller->AddYawInput(newRotYaw);
			player->Controller->AddRollInput(newRotRoll);
			player->Pawn->AddMovementInput(newLocVector, j["Params"]["ScaleVal"], true);
		} else if (function == "Jump") {
			auto player = FindPlayerById(j["Params"]["Id"]);
			player->Pawn->Jump();
		} else if (function == "Crouch") {
			auto player = FindPlayerById(j["Params"]["Id"]);
			player->Pawn->Crouch(true);
		} else if (function == "UnCrouch") {
			auto player = FindPlayerById(j["Params"]["Id"]);
			player->Pawn->UnCrouch(true);
		} else if (function == "ClientConnectToServer") {
			LocalPlayerId = j["Params"]["Id"];
		}
	}
}

void ConnectServer() {

	WSADATA Winsockdata;

	const auto iWsaStartup = WSAStartup(MAKEWORD(2, 2), &Winsockdata);
	if (iWsaStartup != 0)
	{
		std::cout << "iWsaStartup Failed\n";
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in TCPServerAdd;
	TCPServerAdd.sin_family = AF_INET;
	TCPServerAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
	TCPServerAdd.sin_port = htons(7777);

	connect(Socket, reinterpret_cast<SOCKADDR*>(&TCPServerAdd), sizeof(TCPServerAdd));
	printf("[CLIENT] Connected to the Server!\n");

	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&ReadingThread), reinterpret_cast<void*>(Socket), 0, nullptr);

	json j;
	j["Function"] = "ConnectToServer";
	j["Params"]["Name"] = reinterpret_cast<SDK::AFortPlayerController*>(Cores::PlayerController)->McpProfileGroup->PlayerName.ToString();
	
	send(Socket, j.dump().c_str(), sizeof(j.dump().c_str()), 0);
}