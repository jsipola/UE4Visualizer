// Fill out your copyright notice in the Description page of Project Settings.

#include "WorkerThread.h"
#include <string>
#include "UE4Visualizer.h"

WorkerThread::WorkerThread(APlayerController* APC, TQueue<FString>& RecQueue) : PCa(APC)//, DaQueue(RecQueue)
//
// Thread for TCP communication and pushing incoming messages to the Queue
//
//WorkerThread::WorkerThread(APlayerController* APC, Queue<uint8>& AQueue) : PCa(APC)//, AQueue(Aqueue)
{
	kill = false;
	pause = false;

	PCa->ClientMessage("Successs"); // missä näkyy??
	
	DaQueue = &RecQueue;

	//NewQueue->push(123);
	
	semaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);

	Thread = FRunnableThread::Create(this, TEXT("ReceiverThread"), 0, TPri_BelowNormal);

}

WorkerThread::~WorkerThread()
{
	if (semaphore){
		FGenericPlatformProcess::ReturnSynchEventToPool(semaphore);
		semaphore = nullptr;
	}
	if (SocketConnected != nullptr) SocketConnected->Close();
	
	SocketConnected = nullptr;

	if (SocketTCP != nullptr) SocketTCP->Close();
	SocketTCP = nullptr;
/*
	if (listener != nullptr) {
		//delete TCP listener
		listener->OnConnectionAccepted().Unbind();
		delete listener;
		listener = nullptr;
	}
*/
	if (SocketUDP != nullptr) SocketUDP->Close();
	SocketUDP = nullptr;

	if (Thread) {
		delete Thread;
		Thread = nullptr;
	}
}


bool WorkerThread::Init() {
	// Make the sockets 
	//

	//make tcp socket
	SocketTCP = FTcpSocketBuilder(TEXT("TCP SOCKET"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToAddress(FIPv4Address::Any)
		.Listening(5)
		.BoundToPort(10000);

	//make udp socket
	SocketUDP = FUdpSocketBuilder(TEXT("UDP SOCKET"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToAddress(FIPv4Address::Any)
		.BoundToPort(10001)
		.WithMulticastLoopback();

	int32 newSize = 0;
	SocketTCP->SetReceiveBufferSize(65536, newSize);

	UE_LOG(LogTemp, Log, TEXT("Sockets made"));

	return true;
}

uint32 WorkerThread::Run() {
	// Reads for incoming connections
	// and parse data from messages

	FPlatformProcess::Sleep(0.04);
	SocketConnected = nullptr;

	bool isConnected = false;
	FIPv4Address TargetIp(192,168,10,84);
	TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	TSharedRef<FInternetAddr> TargetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	TargetAddr->SetIp(TargetIp.Value);
	TargetAddr->SetPort(10000);

	uint32 Size;
	bool PendingConn;
	bool successful = false;
	uint8 data[65536];
	int32 Read;

	while (!kill){
		if (pause) {
			semaphore->Wait();
		}

		// retrieving the messages
		// and parsing
			
		if (Socket->Connect(*TargetAddr) && (isConnected == false)) {
			UE_LOG(LogTemp, Log, TEXT("Connected To Target IP"));
			isConnected = true;

		}
		
		if (SocketTCP->HasPendingConnection(PendingConn) && PendingConn) {
			if (SocketConnected != nullptr) {
				SocketConnected->Close();
			}
			SocketConnected = SocketTCP->Accept(*Sender, TEXT("The TCP receiver socket"));
			UE_LOG(LogTemp, Log, TEXT("Connected"));
		}
		
		if (SocketConnected != nullptr) {
			while (SocketConnected->HasPendingData(Size)) {
				ReceivedData.Init(FMath::Min(Size, 65507u), Size);
				SocketConnected->Recv(ReceivedData.GetData(), sizeof(data), Read);
			}
		}

		if (isConnected) {
			if (!successful){
				FString serialized = TEXT("HELLO");
				TCHAR *serializedChar = serialized.GetCharArray().GetData();
				int32 size = FCString::Strlen(serializedChar);
				int32 sent = 0;
				successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
			}
			
			while (SocketTCP->HasPendingData(Size)) {
				ReceivedData.Init(FMath::Min(Size, 65507u), Size);
				SocketTCP->Recv(ReceivedData.GetData(), sizeof(data), Read);
			}
			
		}

/*
		if (SocketUDP != nullptr) {
			while (SocketUDP->HasPendingData(Size))
			{
				// read UDP socket
				Read = 0;
				ReceivedData.Init(FMath::Min(Size, 65507u), Size);
				SocketUDP->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), Read, *Sender);
			}
		}
*/
		if (ReceivedData.Num() != 0){
			uint8* array = ReceivedData.GetData();

			FFloat32 floatval = FFloat32(array[0]);

			UE_LOG(LogTemp, Log, TEXT("DATA Received, data length in bytes: %lu"), ReceivedData.Num()); // sizeof(array)
			const std::string cstr(reinterpret_cast<const char*>(ReceivedData.GetData()), ReceivedData.Num());
			FString msg = FString(cstr.c_str());

			msg = msg.LeftChop(1); // strip the last char from string

			TArray<FString> ret;
			msg.ParseIntoArray(ret, TEXT(";"), true); // seperate message using ";" to different cmds
			FString addr = Sender->ToString(true);
			UE_LOG(LogTemp, Log, TEXT("DATA: %s %s"), *msg, *addr);

			for (int a = 0; a < ret.Num();++a) {
				DaQueue->Enqueue(ret[a]);
//				UE_LOG(LogTemp, Log, TEXT("DATA entries: %s"), *ret[a]);
			}

			//DaQueue->Enqueue(msg);
		}
		ReceivedData.Empty();
		FPlatformProcess::Sleep(1.04);//sleep in seconds
	}
	return 0;
}

void WorkerThread::Pause(){
	pause = true;
}

void WorkerThread::Continue() {
	pause = false;

	if (semaphore) {
		semaphore->Trigger();
	}
}

void WorkerThread::Stop() {
	kill = true;
	pause = true;

	if (semaphore) {
		semaphore->Trigger();

	}
}

void WorkerThread::Complete(){

	Stop();

	if (Thread) {
		Thread->WaitForCompletion();
	}
}

bool WorkerThread::isPaused(){
	return (bool)pause;
}

bool WorkerThread::HandleConnection(FSocket* socket, const FIPv4Endpoint& End){
	SocketTCP = socket;
	return true;
}