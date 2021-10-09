#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTile_Interface.h"
#include "ForestTile.generated.h"

class UArrowComponent;

UCLASS()
class APIECEOFPLANET_API AForestTile : public AActor, public IBaseTile_Interface
{
	GENERATED_BODY()
	
public:	
	AForestTile();

	virtual void SpawnEnemy(float damageMultiplier, float healthMultiplier) override;

	virtual void PlayerEnterTile() override;
	virtual void PlayerLeaveTile() override;

	void DestroyTile_Implementation() override;
	bool IsBroken_Implementation() const override;

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	//GROUND
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ground")
		USkeletalMeshComponent* m_GroundSkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ground")
		UStaticMeshComponent* m_GroundStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ground")
		UStaticMesh* m_TileStage1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ground")
		UStaticMesh* m_TileStage2;

	//ENVIRONMENT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "environment")
		UStaticMeshComponent* m_EnvironmentMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "environment")
		UStaticMesh* m_ForestMesh;

	//ENEMIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class AForestEnemyMelee> m_EnemyMelee;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class AForestEnemy_Ranged> m_EnemyRanged;

	//COCONUT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "coconut")
		TSubclassOf<class AAlienCoconut> m_AlienCoconut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "forestEvents")
		UArrowComponent* m_CoconutSpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "forestEvents")
		float m_MinTime;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "forestEvents")
		float m_MaxTime;
	
	float m_AccumulatedTime;
	
	void SpawnCoconut(float dt);

public:	
	virtual void Tick(float DeltaTime) override;
};
