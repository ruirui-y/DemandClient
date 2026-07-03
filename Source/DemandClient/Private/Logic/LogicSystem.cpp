// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/LogicSystem.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

#if PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
#include "Android/AndroidSystemIncludes.h"
#endif

#include "Tools/VRCommandTool.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include "DemandDebug.h"
using namespace Debug;

#define MAX_SEND_MSG_SIZE								1024 * 10
#define NOT_SEND_DEVICE_ID								0
#define RETURN_SUCCESS									"SUCCESS"
#define RETURN_FAILURE									"FAILURE"

void ULogicSystem::DisMsg(RecvMsg recvMsg)
{
	if (recvMsg.ID == ReqID_VR::ID_VR_BROCAST_MSG_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到广播请求 ID=608"));
		DisBroadcast(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_HEART_BEAT_REQUEST)
	{
		// UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到心跳请求 ID=606"));
		DisHeart(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_START_GAME_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到启动游戏请求 ID=602"));
		StartGame(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_STOP_GAME_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到停止游戏请求 ID=604"));
		StopGame(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_MODIFY_USER_NICK_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到修改昵称请求 ID=610"));
		DisModifyNick(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_MODIFY_USER_PLAY_TIME_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到修改游戏时长请求 ID=614"));
		DisModifyPlayTime(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_MODIFY_USER_TEAM_REQUEST)
	{
		UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到修改队伍请求 ID=612"));
		DisModifyTeam(recvMsg);
	}
	else if (recvMsg.ID == ReqID_VR::ID_VR_BRING_TO_FOREGROUND_REQUEST)
	{
        UE_LOG(LogTemp, Log, TEXT("[LogicSystem] 收到前台请求 ID=700"));
		BringToForeground(recvMsg);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[LogicSystem] 收到未知消息 ID=%d"), recvMsg.ID);
	}
}

void ULogicSystem::DisBroadcast(RecvMsg recvMsg)
{
	// 修改配置文件的IP地址
	ModifyAddr(recvMsg);

	// 组转成JSON
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), GetDeviceID());
	Obj->SetStringField(TEXT("nick"), GetDeviceNick());

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_BROCAST_MSG_RESPONSE, Json, recvMsg);
}

void ULogicSystem::DisHeart(RecvMsg recvMsg)
{
	// 组转成JSON
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), GetDeviceID());
	Obj->SetStringField(TEXT("nick"), GetDeviceNick());
	Obj->SetNumberField(TEXT("battery"), VRCommandTool::Instance()->GetDeviceBatteryPercent());
	Obj->SetBoolField(TEXT("is_foreground"), VRCommandTool::Instance()->IsAppInForeground());

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_HEART_BEAT_RESPONSE, Json, recvMsg);
}

void ULogicSystem::StartGame(RecvMsg recvMsg)
{
	// 先启动前台通知保活 UDP
	VRCommandTool::Instance()->StartForegroundForUdp();

	// 记录当前启动的游戏包名，供心跳实时检测
	m_gamePackageName = recvMsg.Msg;

	// 启动游戏
	bool bSuccess = VRCommandTool::Instance()->LaunchAppByPackage(recvMsg.Msg);

	// 立即返回结果
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), GetDeviceID());
	Obj->SetBoolField(TEXT("start_result"), bSuccess);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_START_GAME_RESPONSE, Json, recvMsg);
}

void ULogicSystem::StopGame(RecvMsg recvMsg)
{
	// 停止游戏
	bool bSuccess = VRCommandTool::Instance()->TryKillGameByPackage(recvMsg.Msg);

	// 移除前台通知
	VRCommandTool::Instance()->StopForegroundForUdp();

	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), GetDeviceID());
	Obj->SetBoolField(TEXT("stop_result"), bSuccess);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_STOP_GAME_RESPONSE, Json, recvMsg);
}

void ULogicSystem::DisModifyNick(RecvMsg recvMsg)
{
	FString Nick = recvMsg.Msg;
	FString DeviceId = GetDeviceID();

	// 修改昵称
	VRCommandTool::Instance()->WriteConfig("Nick", Nick);

	// 组转成JSON
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), DeviceId);
	Obj->SetStringField(TEXT("nick"), Nick);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_MODIFY_USER_NICK_RESPONSE, Json, recvMsg);
}

void ULogicSystem::DisModifyPlayTime(RecvMsg recvMsg)
{
	FString PlayTime = recvMsg.Msg;
	FString DeviceId = GetDeviceID();

	// 修改玩家时长
	VRCommandTool::Instance()->WriteConfig("PlayTime", PlayTime);

	// 组转成JSON
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), DeviceId);
	Obj->SetStringField(TEXT("play_time"), PlayTime);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_MODIFY_USER_PLAY_TIME_RESPONSE, Json, recvMsg);
}

void ULogicSystem::DisModifyTeam(RecvMsg recvMsg)
{
	FString Team = recvMsg.Msg;
	FString DeviceId = GetDeviceID();

	// 修改队伍
	VRCommandTool::Instance()->WriteConfig("Team", Team);

	// 组转成JSON
	TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("error"), ErrorCodes::SUCCESS);
	Obj->SetStringField(TEXT("device_id"), DeviceId);
	Obj->SetStringField(TEXT("team"), Team);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer);

	SendMsg(ReqID_VR::ID_VR_MODIFY_USER_TEAM_RESPONSE, Json, recvMsg);
}

// 将自己拉到前台
void ULogicSystem::BringToForeground(RecvMsg recvMsg)
{
	VRCommandTool::Instance()->BringToForeground();
}

void ULogicSystem::SendMsg(uint16 MsgId, FString MsgStr, RecvMsg& recvMsg)
{
	FTCHARToUTF8 Data(*MsgStr);
	int32 payloadLen = Data.Length();																// 用 UTF-8 字节数
	int32 MsgLen = 2 + 2 + payloadLen;																// 头4字节 + 负载

	TArray<uint8> Packet;
	Packet.SetNumUninitialized(MsgLen);
	Packet.SetNumZeroed(MsgLen);

	// 写入消息ID
	Packet[0] = (uint8)(MsgId & 0xFF);
	Packet[1] = (uint8)((MsgId >> 8) & 0xFF);

	// 写入消息长度 
	Packet[2] = (uint8)(MsgStr.Len() & 0xFF);
	Packet[3] = (uint8)((MsgStr.Len() >> 8) & 0xFF);

	// 写入消息体
	FMemory::Memcpy(Packet.GetData() + 2 + 2, Data.Get(), Data.Length());

	int sendBytes;
	recvMsg.Socket->SendTo(Packet.GetData(), Packet.Num(), sendBytes, *recvMsg.Addr.Get());

	FString AddrStr = recvMsg.Addr->ToString(true);
	UE_LOG(LogTemp, Warning, TEXT("[LogicSystem] SendTo ID=%d, Addr=%s, Body=%s"),
		MsgId, *AddrStr, *MsgStr);
}

void ULogicSystem::ModifyAddr(RecvMsg recvMsg)
{
	// 读取配置文件
	FString Addr;
	VRCommandTool::Instance()->ReadConfig("Address", Addr);

	if (Addr == recvMsg.Addr->ToString(false))
	{
		return;
	}

	// 不相等修改配置文件
	VRCommandTool::Instance()->WriteConfig("Address", recvMsg.Addr->ToString(false));
}

FString ULogicSystem::GetLocalIP()
{
	bool bCanBind = false;
	TSharedRef<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	return LocalAddr->ToString(false); // false = 不带端口
}

FString ULogicSystem::GetDeviceNick()
{
	FString Nick;
	VRCommandTool::Instance()->ReadConfig("Nick", Nick);
	return Nick;
}

FString ULogicSystem::GetDeviceTeam()
{
	FString Team;
	VRCommandTool::Instance()->ReadConfig("Team", Team);
	return Team;
}

FString ULogicSystem::GetDevicePlayTime()
{
	FString PlayTime;
	VRCommandTool::Instance()->ReadConfig("PlayTime", PlayTime);
	return PlayTime;
}

FString ULogicSystem::GetDeviceID()
{
	FString ID;
	VRCommandTool::Instance()->ReadConfig("ID", ID);
	return ID;
}

void ULogicSystem::PrintData(TArray<uint8> Pack)
{
	// 前两字节是 MsgId
	uint16 MsgId = Pack[0] | (Pack[1] << 8);

	// 剩下的转成字符串
	FString Body = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Pack.GetData() + 2)));

	Print(FString::Printf(TEXT("SendTo ID=%d, Body=%s"),
		MsgId,
		*Body));
}