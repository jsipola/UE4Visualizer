// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeshActor.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
//#include "RuntimeMeshComponent.h"
#include "WorkerThread.h"
#include <unordered_map>
#include "GameFramework/PlayerController.h"
#include "PlayerControl.generated.h"

/**
 * 
 */

UCLASS()
class UE4VISUALIZER_API APlayerControl : public APlayerController
{
	GENERATED_BODY()
	
public:
		
	void BeginPlay();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ParseFile();

	TQueue<FString> DataQueue;
//	std::queue<int32>* que;

private:

	WorkerThread* CommThread;
	FPlatformMemory::FSharedMemoryRegion* Region;

	AMeshActor* mesha;
	AMeshActor* meshactor;

	TMap<FString, AMeshActor*> MeshMap; // map of the meshes identified by Fstring id
	TMap<int32, AMeshActor*> MapOfMeshes; // map of the empty meshes identified by an counter
	int32 MeshCounter; // counter for meshes

	FHttpModule* Http;
	FString received; // the received http msg
	FString id; // id for the mesh
	bool bFileObtained = true; // flag for when file has been downloaded and mesh spawned

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void BeginDestroy();

	void Tick(float DeltaSeconds);

	FString GetValueFromQueue();
	void MakeHttp();
	void GetHttp();
	void GetHttp(FString file);
	void SetupEnvironment();
};
