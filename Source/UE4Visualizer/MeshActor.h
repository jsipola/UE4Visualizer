// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
//#include "RuntimeMesh.h"
#include "RuntimeMeshLibrary.h"
#include "GameFramework/Actor.h"
#include "MeshActor.generated.h"

UCLASS()
class UE4VISUALIZER_API AMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FRuntimeMeshTangent> Tangents;
	TArray<FColor> VertexColors;
	TArray<FVector2D> TextureCoordinates;
	TArray<FVector2D> TextureCoordinates2;
	TArray<int32> Triangles;

	FString id;
	
	URuntimeMesh* mesh;
	UStaticMesh* staticmesh;
	UStaticMeshComponent* Control;
	UMaterialInterface* MatRef;
	UMaterialInterface* MatRef_2;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVertices(TArray<FVector> verts);
	void SetNormals(TArray<FVector> normals);
	void SetTexcoords(TArray<FVector2D> texcoords);
	void SetTexcoords2(TArray<FVector2D> texcoords2);
	void SetTriangles(TArray<int32> triangles);
	void SetTangents();
	void SetColors();
	void SetId(FString name);
	void MakeMesh();

	FString GetId();

	void CreateMesh();

	void CreateMesh(int32 section);

	void UpdateMesh();

	URuntimeMeshComponent* RuntimeMesh;


};
