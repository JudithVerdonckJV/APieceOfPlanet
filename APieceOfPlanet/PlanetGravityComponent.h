// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlanetGravityComponent.generated.h"

class AStaticMeshActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APIECEOFPLANET_API UPlanetGravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlanetGravityComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
		void ApplyGravity();
	UFUNCTION()
		void ApplyRotation();
	UFUNCTION()
		bool IsGrounded(FHitResult& hit);

	UFUNCTION()
		void SetLineTraceDistance(float lineDistance);

private:
	APawn* m_Owner = nullptr;
	const FVector m_GravityCenter = { 0, 0, 0 };
	float m_LineTraceDistance = 200.f;

	AStaticMeshActor* m_WalkablePlanet;
};
