// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "IPAddress.h"

struct Message
{
public:
	uint16 ID;
	uint16 MsgLen;
	FString Msg;
};

struct SendMsg : public Message
{

};

struct RecvMsg : public Message
{
public:
	FSocket* Socket;
	TSharedPtr<FInternetAddr> Addr;
};