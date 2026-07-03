// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceSubsystem/UDPSubsystem.h"
#include "UDP/UDPReceiver.h"
#include "Logic/LogicSystem.h"
#include "Tools/VRCommandTool.h"

#include "DemandDebug.h"
using namespace Debug;


void UUDPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// ---- 初始化日志文件记录（在启动任何逻辑之前） ----
	VRCommandTool::Instance()->InitFileLogging();

	Receiver = NewObject<UUDPReceiver>(this);
	Receiver->Init(8889);
	Receiver->StartRecv();

	LogicSystem = NewObject<ULogicSystem>(this);
	Receiver->OnRecvMsg.BindUObject(LogicSystem, &ULogicSystem::DisMsg);
}

void UUDPSubsystem::Deinitialize()
{
    // ---- 先确认 Deinitialize 到底跑没跑 ----
    UE_LOG(LogTemp, Log, TEXT("[UDPSubsystem] Deinitialize called"));

    // ---- 退出前 dump 一次完整 logcat ----
    FString JavaLogPath = VRCommandTool::Instance()->DumpLogcatToFile();
    UE_LOG(LogTemp, Log, TEXT("[UDPSubsystem] Logcat 已保存: %s"), *JavaLogPath);

    if (Receiver)
    {
        Receiver->StopRecv();
        Receiver->Shutdown();
    }
}