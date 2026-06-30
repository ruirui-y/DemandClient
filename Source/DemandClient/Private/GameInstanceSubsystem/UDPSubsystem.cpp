// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceSubsystem/UDPSubsystem.h"
#include "UDP/UDPReceiver.h"
#include "Logic/LogicSystem.h"

#include "DemandDebug.h"
using namespace Debug;


void UUDPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Receiver = NewObject<UUDPReceiver>(this);
	Receiver->Init(8889);
	Receiver->StartRecv();

	LogicSystem = NewObject<ULogicSystem>(this);
	Receiver->OnRecvMsg.BindUObject(LogicSystem, &ULogicSystem::DisMsg);
}

void UUDPSubsystem::Deinitialize()
{
	if (Receiver) Receiver->Shutdown();
}