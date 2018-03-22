// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshActor.h"


// Sets default values
AMeshActor::AMeshActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComp"));
	mesh = CreateDefaultSubobject<URuntimeMesh>(TEXT("RuntimeMesh"));
//	RuntimeMesh = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("Material Billboard"));
	
	//test materials
	static ConstructorHelpers::FObjectFinder<UMaterial> StaticMat(TEXT("Material'/Game/Material_Textures/Rock_6_d_Mat.Rock_6_d_Mat'")); //  
//	static ConstructorHelpers::FObjectFinder<UMaterial> StaticMat(TEXT("Material'/Game/NewMaterial.NewMaterial'"));
//	static ConstructorHelpers::FObjectFinder<UMaterial> StaticMat_2(TEXT("Material'/Game/Material_Textures/Green.Green'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> StaticMat_2(TEXT("Material'/Game/Material_Textures/Rattan_Deckchair_df_Mat.Rattan_Deckchair_df_Mat'"));
//	static ConstructorHelpers::FObjectFinder<UMaterial> StaticMat(TEXT("Material'/Game/chalet_Mat.chalet_Mat'"));

	//if materials are found
	if (StaticMat.Succeeded()) MatRef = StaticMat.Object;
	if (StaticMat_2.Succeeded()) MatRef_2 = StaticMat_2.Object;

	RuntimeMesh->SetRuntimeMesh(mesh);
	RootComponent = RuntimeMesh;

	//	static ConstructorHelpers::FObjectFinder<UTexture> StaticTex(TEXT("Texture2D'/Game/Rock_6_d.Rock_6_d'")); 

//	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(StaticMat.Object, this);
//	DynMat->SetTextureParameterValue(TEXT("tex"), StaticTex.Object);

//	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("Material'/Game/Rock_6_d_Mat.Rock_6_d_Mat'"));

	//RootComponent->CreatePhysicsState();
	//UE_LOG(LogTemp, Log, TEXT(" AMeshActor spawned"));

	//UMaterial* Mat = UMaterial::GetDefaultMaterial(MD_Surface);

}

// Called when the game starts or when spawned
void AMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeshActor::MakeMesh() {
	//example mesh

	Vertices.Add(FVector(0, 100, 0));
	Normals.Add(FVector(0, 0, 1));
	Tangents.Add(FRuntimeMeshTangent(0, -1, 0));
	VertexColors.Add(FColor::White);
	TextureCoordinates.Add(FVector2D(0, 0));

	// Second vertex
	Vertices.Add(FVector(100, 100, 0));
	Normals.Add(FVector(0, 0, 1));
	Tangents.Add(FRuntimeMeshTangent(0, -1, 0));
	VertexColors.Add(FColor::White);
	TextureCoordinates.Add(FVector2D(0, 1));

	// Third vertex
	Vertices.Add(FVector(100, 0, 0));
	Normals.Add(FVector(0, 0, 1));
	Tangents.Add(FRuntimeMeshTangent(0, -1, 0));
	VertexColors.Add(FColor::White);
	TextureCoordinates.Add(FVector2D(1, 1));

	// Fourth vertex
	Vertices.Add(FVector(0, 0, 0));
	Normals.Add(FVector(0, 0, 1));
	Tangents.Add(FRuntimeMeshTangent(0, -1, 0));
	VertexColors.Add(FColor::White);
	TextureCoordinates.Add(FVector2D(1, 0));

	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(3);

	CreateMesh();
	
}

void AMeshActor::CreateMesh() {
	//create a mesh
	RuntimeMesh->CreateMeshSection(0, Vertices, Triangles, Normals, TextureCoordinates, VertexColors, Tangents, false);
	RuntimeMesh->SetMeshSectionCastsShadow(0, false);
	//RuntimeMesh->bRenderInMainPass = 0;
	//RuntimeMesh->SetCastShadow(false);
	if (MatRef != nullptr) RuntimeMesh->SetMaterial(0, MatRef);
	
//	RuntimeMesh->SetEnableGravity(true);
//	RuntimeMesh->bUseComplexAsSimpleCollision = false;
//	RuntimeMesh->SetSimulatePhysics(true);
	RuntimeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMeshActor::CreateMesh(int32 section) {
	//create a specified section of the mesh
//	RuntimeMesh->CreateMeshSection(section, Vertices, Triangles, Normals, TextureCoordinates, VertexColors, Tangents, true);
	mesh->CreateMeshSection(section, Vertices, Triangles, Normals, TextureCoordinates, VertexColors, Tangents, false);
//	mesh->CreateMeshSection<FVector,int32>(section);
//	FRuntimeMeshDataRef asd = mesh->GetRuntimeMeshData();
//	asd->CreateMeshSection<FVector, int32>(section, Vertices, Triangles, FBox(FVector(0, 0, 0), FVector(1000, 1000, 1000)));
//	asd->UpdateMeshSection(section, Vertices, Triangles, FBox(FVector(0, 0, 0), FVector(1000, 1000, 1000)));
//	mesh->UpdateMeshSection(section, Vertices, Triangles, FBox(FVector(0, 0, 0), FVector(1000, 1000, 1000)));
	mesh->SetMeshSectionCastsShadow(section, false);
//	RuntimeMesh->SetMeshSectionVisible(section, true);
	if (section == 0) {
		if (MatRef != nullptr) mesh->SetSectionMaterial(section, MatRef);//RuntimeMesh->SetMaterial(section, MatRef);
	} else {
		if (MatRef_2 != nullptr) mesh->SetSectionMaterial(section, MatRef_2);// RuntimeMesh->SetMaterial(section, MatRef_2);
	}
}


void AMeshActor::UpdateMesh() {
	// update the first section of the mesh
	RuntimeMesh->UpdateMeshSection(0, Vertices, Triangles, Normals, TextureCoordinates, VertexColors, Tangents);
}

void AMeshActor::SetVertices(TArray<FVector> verts) {
	Vertices = verts;
}

void AMeshActor::SetNormals(TArray<FVector> normals) {
	Normals = normals;
}

void AMeshActor::SetTexcoords(TArray<FVector2D> texcoords) {
	TextureCoordinates = texcoords;
}

void AMeshActor::SetTexcoords2(TArray<FVector2D> texcoords2) {
	TextureCoordinates2 = texcoords2;
}

void AMeshActor::SetTangents() {
	for (int i = 0; i < Vertices.Num(); ++i) {
		Tangents.Add(FRuntimeMeshTangent(-1, -1, 0));
	}
//	URuntimeMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, TextureCoordinates, Normals, Tangents); // doesn't work?
}

void AMeshActor::SetColors() {
	if (VertexColors.Num() != Vertices.Num()) {
		for (int i = 0; i < Vertices.Num(); ++i) {
			VertexColors.Add(FColor::Red);
		}
	}
}

void AMeshActor::SetTriangles(TArray<int32> triangles) {
	Triangles = triangles;
}

void AMeshActor::SetId(FString name) {
	id = name;
}

FString AMeshActor::GetId() {
	return id;
}
