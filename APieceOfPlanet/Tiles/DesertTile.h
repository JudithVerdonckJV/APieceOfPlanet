#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTile_Interface.h"
#include "DesertTile.generated.h"

class APlayerCharacter;
class USkeletalMeshComponent;
class UNiagaraSystem;

UCLASS()
class APIECEOFPLANET_API ADesertTile : public AActor, public IBaseTile_Interface
{
	GENERATED_BODY()
	
public:	
	ADesertTile();

	virtual void SpawnEnemy(float damageMultiplier, float healthMultiplier) override;

	virtual void PlayerEnterTile() override;
	virtual void PlayerLeaveTile() override;

	void DestroyTile_Implementation() override;
	bool IsBroken_Implementation() const override;

	bool IsOasis() const;
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
		UStaticMesh* m_DesertMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "environment")
		UStaticMesh* m_OasisMesh;
	
	//ENEMIES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class ADesertEnemy_Melee> m_EnemyMelee;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemies")
		TSubclassOf<class ADesertEnemy_Ranged> m_EnemyRanged;

	//OASIS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "oasis")
		bool m_HealPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "oasis")
		float m_HealingPoolMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "oasis")
		float m_HealingPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "oasis")
		float m_HealingCooldownMax;
	UPROPERTY(EditAnywhere, Category = "oasis")
		UNiagaraSystem* m_DisappearEffect;

	//EFFECTS
	UPROPERTY(VisibleAnywhere, Category = "visual fx")
		bool m_HeatShown = false;


	float m_HealingPoolCurrent;
	float m_HealingCooldownCurrent;
	bool m_IsOasis;

	//HEAT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "oasis")
		float m_HeatDamageCooldownMax;

	float m_HeatDamageCooldownCurrent;

	//PLAYER
	bool m_PlayerOnTile;	
	APlayerCharacter* m_Player;

public:	
	virtual void Tick(float DeltaTime) override;
};
