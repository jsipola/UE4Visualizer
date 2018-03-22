// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControl.h"


#define SCALE 100

void APlayerControl::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("START"));

	//make TCP communication thread
	CommThread = nullptr;
	CommThread = new WorkerThread(this, DataQueue);
	
	//test mesh
/*	mesha = (AMeshActor*)GetWorld()->SpawnActor(AMeshActor::StaticClass());

	//mesha = AMeshActor();
	mesha->SetActorLocation(FVector(300, 300,300));
	mesha->MakeMesh(); //test mesh
	mesha->SetId("Test1");
//	mesha->SetActorLabel("Test Actor");

	//test mesh
	AMeshActor* mesha2 = (AMeshActor*)GetWorld()->SpawnActor(AMeshActor::StaticClass());
	mesha2->SetActorLocation(FVector(600, 600, 600));
	mesha2->MakeMesh();
	mesha2->SetActorRotation(FRotator(90,0,0));
	mesha2->SetId("TEST2");

	//add test mesh to map for meshes
	MeshMap.Add(mesha->GetId(), mesha);
*/
	//make 50 empty meshactors ready for use
	for (MeshCounter = 0; MeshCounter < 50; ++MeshCounter) {
		AMeshActor* mesha3 = (AMeshActor*)GetWorld()->SpawnActor(AMeshActor::StaticClass());
		mesha3->AttachToActor(mesha, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		MapOfMeshes.Add(MeshCounter,mesha3);
	}
	MeshCounter = 0;
//	APlayerControl::Pause();
	//SetupEnvironment();

}


void APlayerControl::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	if (CommThread) {
		CommThread->Complete();
		delete CommThread;
		CommThread = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void APlayerControl::BeginDestroy() {
	UE_LOG(LogTemp, Log, TEXT("END"));

	while (!(DataQueue.IsEmpty())) {
		FString val;
		DataQueue.Dequeue(val);
		UE_LOG(LogTemp, Log, TEXT("(END) Data from queue: %s"), *val);
	}
	if (CommThread) {
		CommThread->Complete();
		delete CommThread;
		CommThread = nullptr;
	}
	Super::BeginDestroy();
}


void APlayerControl::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!(DataQueue.IsEmpty()) && bFileObtained) {
		//FFloat32 val = GetValueFromQueue();
		FString msg;
		DataQueue.Dequeue(msg);
		UE_LOG(LogTemp, Log, TEXT("(TICK) Data from Queue: %s"), *msg);
		//msg = msg.LeftChop(1); // strip the last char from string
		TArray<FString> arr;
		msg.ParseIntoArray(arr, TEXT(" "), true); // seperate the command using " " delimiter
		if (msg.Left(4).Compare("make") == 0) { // the make mesh command
			bFileObtained = false;
//			if (id.Equals("")) {
				GetHttp();
				//id = msg.RightChop(5);
				id = arr[1];
				meshactor = MapOfMeshes[MeshCounter]; // retrieve next mesh in the pool
				MeshCounter++;
				MeshMap.Add(id, meshactor);
				UE_LOG(LogTemp, Log, TEXT("(TICK) From queue: %s, %d"), *id, arr.Num());
//			} else { DataQueue.Enqueue(msg); } // if download in progress put value back in queue
			
		}
		else if (msg.Left(4).Compare("move") == 0) {
			//UE_LOG(LogTemp, Log, TEXT("(TICK) From queue: %s, %d"), *arr[1], arr.Num());
			AMeshActor* act = MeshMap.FindRef(arr[1]); // find the correct actor
			if (act != nullptr) act->SetActorLocation(FVector(arr.Num() > 2 ? FCString::Atoi(*arr[2]) : 0, arr.Num() > 3 ? FCString::Atoi(*arr[3]) : 0, arr.Num() == 5 ? FCString::Atoi(*arr[4]) : 0)); // move the actor and checking the input for missing values
			else { DataQueue.Enqueue(msg); } // if no actor with id is found put value back in queue, make a better solution in future!!!!
		}
	}
}

void APlayerControl::MakeHttp() {
	//init http 
	if (Http == nullptr) {
		Http = &FHttpModule::Get();
	}
}

void APlayerControl::GetHttp() {
	//HTTP get request
	MakeHttp();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &APlayerControl::OnResponseReceived);
	Request->SetURL("http://192.168.10.84:8000/rock.txt"); // file here
	Request->SetVerb("GET");
	//Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();

}

void APlayerControl::GetHttp(FString file) {
	//HTTP get request with argument
	MakeHttp();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &APlayerControl::OnResponseReceived);
	FString address = "http://192.168.10.84:8000/";
	address.Append(file);
	Request->SetURL(address); // file here
	Request->SetVerb("GET");
	//Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/json"));
	Request->ProcessRequest();
	
}

void APlayerControl::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{

	if (!bWasSuccessful) {
		UE_LOG(LogTemp, Log, TEXT("HTTP GET FAILURE"));
		return;
	}
	else {
		//UE_LOG(LogExec, Warning, TEXT("Response: %s"), *(Response->GetContentAsString()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" %s"), *(Response->GetContentAsString())));
		//std::string str;
		received = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("File Obtained"));
		ParseFile();
	}
}

void APlayerControl::ParseFile() {
	// Parsing the downloaded mesh file

	FString str = received;
	FString line;
	TArray<FString> lines;
	TArray<FString> arr;
	FVector vect;

	FVector2D vect2d;
	TArray<FVector> verts;
	TArray<FVector> normals;
	TArray<FVector2D> texcoords;
	TArray<int32> triangles;
	FVector location = FVector(0, 0, 0);
	FRotator rotation = FRotator(0, 0, 0);

	id = nullptr; // id of the mesh
	str.ParseIntoArrayLines(lines,true); // hox not efficient
	//UE_LOG(LogTemp, Log, TEXT("LINE COUNT: %d"), lines.Num());
	int s = 0; //counter for the mesh section
	for (int i= 0; i < lines.Num(); i++) {
		line = lines[i];
		//UE_LOG(LogTemp, Log, TEXT("A LINE: %s"), *line);
		line.ParseIntoArray(arr, TEXT(";"), true);
		//UE_LOG(LogTemp, Log, TEXT("A VALUE: %s"), *arr[0]);
		if (arr.Num() != 0) { // 
			if (arr[0].Compare("v ") == 0) {
				//define vertices
				arr.RemoveAt(0);
				//UE_LOG(LogTemp, Log, TEXT("Added vert"));
				for (int i = 0; i < arr.Num(); ++i) {
					vect.X = (FCString::Atof(*arr[i + 0])) * SCALE;
					vect.Y = (FCString::Atof(*arr[i + 1])) * SCALE;
					vect.Z = (FCString::Atof(*arr[i + 2])) * SCALE;
					i += 2;
					verts.Add(vect);
				}
			}
			else if (arr[0].Compare("n ") == 0) {
				arr.RemoveAt(0);
				//define normals
				for (int i = 0; i < arr.Num(); ++i) {
					vect.X = FCString::Atof(*arr[i + 0]) * SCALE;
					vect.Y = FCString::Atof(*arr[i + 1]) * SCALE;
					vect.Z = FCString::Atof(*arr[i + 2]) * SCALE;
					//UE_LOG(LogTemp, Log, TEXT("normal z %f %f %f"), vect.X, vect.Y, vect.Z);
					i += 2;
					normals.Add(vect);
				}
			}
			else if (arr[0].Compare("tc ") == 0) {
				arr.RemoveAt(0);
				//define texcoords
				for (int i = 0; i < arr.Num(); i++) {
					vect2d.Y = FCString::Atof(*arr[i + 0]) * 1; // SCALE
					vect2d.X = FCString::Atof(*arr[i + 1]) * 1; // SCALE
					i += 1;
					texcoords.Add(vect2d);
				}
			}
			else if (arr[0].Compare("tri ") == 0) {
				arr.RemoveAt(0);
				//define triangles
				for (int i = 0; i < arr.Num(); i++) {
					triangles.Add(FCString::Atoi(*arr[i]));
				}
			}
			else if (arr[0].Compare("pos ") == 0) {
				//if mesh has a position defined
				arr.RemoveAt(0);
				location.X = FCString::Atof(*arr[0]);
				location.Y = FCString::Atof(*arr[1]);
				location.Z = FCString::Atof(*arr[2]);
				if (arr.Num() == 6) {
					rotation = FRotator(FCString::Atof(*arr[3]), FCString::Atof(*arr[4]), FCString::Atof(*arr[5]));
				}
			}
			else if (arr[0].Compare("s ") == 0) {
				//make the shape/section defined in the file
//				meshactor->SetActorLocation(location);
				meshactor->SetActorRotation(rotation);
				meshactor->SetVertices(verts);
				meshactor->SetNormals(normals);
				meshactor->SetTriangles(triangles);
				meshactor->SetTexcoords(texcoords);
				meshactor->SetColors();
				//meshactor->SetTangents();
				meshactor->CreateMesh(s);
				s++; // shape index
				//verts.Empty();
				triangles.Empty();
			}
			
		}
	}
	bFileObtained = true;
}

void APlayerControl::SetupEnvironment() {
	GetHttp("environment.txt");
}