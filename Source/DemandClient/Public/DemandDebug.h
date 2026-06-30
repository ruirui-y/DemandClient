#ifndef DEMAND_DEBUG_H
#define DEMAND_DEBUG_H

#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformTime.h"

#include "Engine/World.h"
#include "Engine/Engine.h"


namespace Debug
{
	static void Print(const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int InKey = -1, float Duration = 5.0f)
	{
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(InKey, Duration, Color, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}

	static void Print(const FString& FloatTitle, float FloatValueToPrint, int InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		FString FinalMsg;
		if (GEngine)
		{
			FinalMsg = FloatTitle + TEXT(": ") + FString::SanitizeFloat(FloatValueToPrint);
			//GEngine->AddOnScreenDebugMessage(InKey, 5.0f, Color, FinalMsg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMsg);
		}
	}

	static void Print_Func(const FString& CallerFunctionName, const FString& Msg, const FColor& Color = FColor::MakeRandomColor(), int InKey = -1, float Duration = 5.0f)
	{
		Print(FString::Printf(TEXT("[%s] Invalid WorldContext, unable to determine NetMode"), *CallerFunctionName), FColor::Red);
	}

	static void PrintNetMode(UWorld* WorldContext, const FString& CallerFunctionName)
	{
		if (!WorldContext)
		{
			Print(FString::Printf(TEXT("[%s] Invalid WorldContext, unable to determine NetMode"), *CallerFunctionName), FColor::Red);
			return;
		}

		const ENetMode NetMode = WorldContext->GetNetMode();

		FString NetModeMsg;
		switch (NetMode)
		{
		case NM_Client:
			NetModeMsg = TEXT("Running as Client");
			break;
		case NM_ListenServer:
			NetModeMsg = TEXT("Running as Listen Server");
			break;
		case NM_DedicatedServer:
			NetModeMsg = TEXT("Running as Dedicated Server");
			break;
		case NM_Standalone:
			NetModeMsg = TEXT("Running in Standalone mode (Client + Server)");
			break;
		default:
			NetModeMsg = TEXT("Unrecognized NetMode");
			break;
		}

		Print(FString::Printf(TEXT("[%s] %s"), *CallerFunctionName, *NetModeMsg));
	}
};
#endif // WARRIOR_DEBUG_HELPER_H