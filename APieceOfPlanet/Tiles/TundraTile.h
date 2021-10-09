#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTile_Interface.h"
#include "TundraTile.generated.h"

class APlayerCharacter;

UCLASS()
class APIECEOFPLANET_API ATundraTile : public AActor, public IBaseTile_Interface
{
	GENERATED_BODY()
	
public:	
	ATundraTile();

	virtual void SpawnEnemy(float damageMultiplier, float healthMultiplier) override;

	virtual void PlayerEnterTile() override;
	virtual void PlayerLeaveTile() override;

	virtual void DestroyTile_Implementation() override;
	virtual bool IsBroken_Implementation() const override;

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

	//ENVIRONMENT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "environment")
		UStaticMeshComponent* m_EnvironmentMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "environment")
		UStaticMesh* m_DesertMesh;

	//ENEMIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class ATundraEnemy_Melee> m_EnemyMelee;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class ATundraEnemy_Ranged> m_EnemyRanged;

	//PLAYER
	APlayerCharacter* m_Player;

public:	
	virtual void Tick(float DeltaTime) override;
};
