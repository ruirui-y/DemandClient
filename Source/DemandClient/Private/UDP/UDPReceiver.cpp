// Fill out your copyright notice in the Description page of Project Settings.


#include "UDP/UDPReceiver.h"
#include "Common/UdpSocketBuilder.h"
#include "DemandDebug.h"
using namespace Debug;

void UUDPReceiver::Init(int32 Port)
{
	Sock = FUdpSocketBuilder(TEXT("BroadcastRecv"))
		.AsNonBlocking()
		.AsReusable()
		.WithBroadcast()
		.BoundToEndpoint(FIPv4Endpoint(FIPv4Address::Any, Port))
		.WithReceiveBufferSize(256 * 1024);

	if (!Sock) { UE_LOG(LogTemp, Warning, TEXT("Create UDP socket failed")); return; }

	Addr = ISocketSubsystem::Get()->CreateInternetAddr();
	Addr->SetPort(Port);
}

void UUDPReceiver::Shutdown()
{
	if (Sock)
	{
		Sock->Close();
		ISocketSubsystem::Get()->DestroySocket(Sock);
		Sock = nullptr;
	}
}

void UUDPReceiver::StartRecv()
{
	bStopRecv = false;

	Async(EAsyncExecution::Thread, [this]()
		{
			while (!bStopRecv)
			{
				RecvFrom();																				// 直接调用接收函数
				FPlatformProcess::Sleep(0.016f);														// 模拟原来定时器的 60FPS
			}
		});
}

void UUDPReceiver::StopRecv()
{
	bStopRecv = true;
}

void UUDPReceiver::RecvFrom()
{
	if (!Sock)
	{
		UE_LOG(LogTemp, Warning, TEXT("UUDPReceiver::StartRecv skipped: socket is null"));
		return;
	}

	uint8 Buffer[2048];
	int32 BytesRead = 0;
	TSharedRef<FInternetAddr> From = ISocketSubsystem::Get()->CreateInternetAddr();

	if (Sock->RecvFrom(Buffer, sizeof(Buffer), BytesRead, *From))
	{
		if (Once)
		{
			Once = false;
			uint32 OutAddr;
			From->GetIp(OutAddr);
			IP = FIPv4Address(OutAddr);
			Addr->SetIp(IP.Value);
			Print(FString::Printf(TEXT("RecvFrom: %s "), *IP.ToString()));
		}

		// 解析消息ID
		uint16 MsgId = (uint16)Buffer[0] | ((uint16)Buffer[1] << 8);

		// 解析消息长度
		uint16 MsgLen = (uint16)Buffer[2] | ((uint16)Buffer[3] << 8);

		// 解析消息体
		FString MsgBody;
		if (MsgLen > 0)
		{
			// 拷贝 Body 部分
			FString Temp = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Buffer + 2 + 2)));
			MsgBody = Temp.Left(MsgLen);																	// 避免 ANSI_TO_TCHAR 读过头
		}

		// 组装成消息体
		RecvMsg Recv;
		Recv.ID = MsgId;
		Recv.MsgLen = MsgLen;
		Recv.Msg = MsgBody;
		Recv.Socket = Sock;
		Recv.Addr = Addr;

		// 异步执行
		OnRecvMsg.ExecuteIfBound(Recv);
	}
}