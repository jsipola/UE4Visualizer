// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "CoreMinimal.h"
#include "RunnableThread.h"
#include "Runnable.h"
#include "ThreadSafeBool.h"
#include "GenericPlatformProcess.h"
#include "PlatformProcess.h"
#include "Networking.h"
#include <queue>
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

/**
 * 
 */
class UE4VISUALIZER_API WorkerThread : public FRunnable//, public UObject
{
public:
	WorkerThread(APlayerController* APC, TQueue<FString>& RecQueue);
	//WorkerThread(APlayerController* APC, Queue<uint8>& AQueue);
	~WorkerThread();

	APlayerController* PCa;



	//Queue<uint8> AQueue;

	void Complete();

	void Pause();

	void Continue();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	bool isPaused();

	bool HandleConnection(FSocket * socket, const FIPv4Endpoint & End);

private:

	TQueue<FString>* DaQueue;
	std::queue<int32>* NewQueue;

	FSocket* SocketTCP;
	FSocket* SocketConnected;
	FSocket* SocketUDP;

	TArray<uint8> ReceivedData;

	TArray<FVector> ArrayForData;

	FTcpListener* listener;

	FRunnableThread* Thread;
	FCriticalSection mutex;
	FEvent* semaphore;

	FThreadSafeBool kill;
	FThreadSafeBool pause;

};
