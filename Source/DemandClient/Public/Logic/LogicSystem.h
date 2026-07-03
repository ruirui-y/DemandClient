// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalStruct.h"
#include "LogicSystem.generated.h"

/**
 * 
 */
UCLASS()
class DEMANDCLIENT_API ULogicSystem : public UObject
{
	GENERATED_BODY()
	
public:
	void DisMsg(RecvMsg recvMsg);

private:
	void DisBroadcast(RecvMsg recvMsg);
	void DisHeart(RecvMsg recvMsg);
	void StartGame(RecvMsg recvMsg);
	void StopGame(RecvMsg recvMsg);
	void DisModifyNick(RecvMsg recvMsg);
	void DisModifyPlayTime(RecvMsg recvMsg);
	void DisModifyTeam(RecvMsg recvMsg);

	void BringToForeground(RecvMsg recvMsg);                          // 将自己拉到前台

	void SendMsg(uint16 MsgId, FString MsgStr, RecvMsg& recvMsg);

private:
	void ModifyAddr(RecvMsg recvMsg);

private:
	FString GetLocalIP();
	FString GetDeviceNick();
	FString GetDeviceTeam();
	FString GetDevicePlayTime();
	FString GetDeviceID();
	void PrintData(TArray<uint8> Pack);

private:
	FString m_gamePackageName;                                                  // 当前启动的游戏包名（心跳检测用）
};
