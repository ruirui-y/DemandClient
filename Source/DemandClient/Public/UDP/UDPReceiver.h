// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Common/UdpSocketReceiver.h"
#include "GlobalStruct.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UDPReceiver.generated.h"

DECLARE_DELEGATE_OneParam(FOnRecvMessage, RecvMsg)

/**
 * 
 */
UCLASS()
class DEMANDCLIENT_API UUDPReceiver : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(int32 Port);
	void Shutdown();
	void StartRecv();
	void StopRecv();

private:
	void RecvFrom();

public:
	FOnRecvMessage OnRecvMsg;

private:
	FSocket* Sock = nullptr;
	FIPv4Address IP;
	TSharedPtr<FInternetAddr> Addr;
	bool Once = true;
	bool bStopRecv = false;
};
