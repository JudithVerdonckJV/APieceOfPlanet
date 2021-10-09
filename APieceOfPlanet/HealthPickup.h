#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class APIECEOFPLANET_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "collider")
		USphereComponent* m_Collider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mesh")
		USkeletalMeshComponent* m_Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "pickup")
		float m_HealingPoints;

	UPROPERTY(EditAnywhere, Category = "VisualFX")
		UNiagaraSystem* m_HealEffect;

	UFUNCTION()
		void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;
};
