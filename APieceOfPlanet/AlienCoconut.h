#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AlienCoconut.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UPlanetGravityComponent;
class UFloatingPawnMovement;
class UNiagaraSystem;

UCLASS()
class APIECEOFPLANET_API AAlienCoconut : public APawn
{
	GENERATED_BODY()

public:
	AAlienCoconut();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComponent,
			FVector NormalImpulse,
			const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* m_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* m_ColliderBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPlanetGravityComponent* m_Gravity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UFloatingPawnMovement* m_Movement;

	UPROPERTY(EditAnywhere)
		UNiagaraSystem* m_HitEffect;

	float m_Damage;
	float m_VelocityScale;
	float m_Slowing;
	FVector m_Direction;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};