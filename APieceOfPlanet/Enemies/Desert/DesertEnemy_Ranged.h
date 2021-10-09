#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy_Interface.h"
#include "DesertEnemy_Ranged.generated.h"

class USkeletalMeshComponent;

UCLASS()
class APIECEOFPLANET_API ADesertEnemy_Ranged : public APawn, public IBaseEnemy_Interface
{
	GENERATED_BODY()

public:
	ADesertEnemy_Ranged();

	float GetMaxHealth_Implementation() const override;
	float GetCurrentHealth_Implementation() const override;
	bool IsMelee_Implementation() const override;
	bool IsAttacking_Implementation() const override;
	void SetIsAttacking_Implementation(bool isAttacking) override;
	void DestroyEnemy_Implementation() override;
	float GetDamage_Implementation() const override;
	void DropItem_Implementation() const override;

	virtual void SetDamageMultiplier(float multiplier) override;
	virtual void SetHealthMultiplier(float multiplier) override;

protected:
	virtual void BeginPlay() override;

	virtual void Attack(float dt) override;
	virtual void MoveToPlayer() override;
	virtual void MoveToCamp(bool fullSpeed = true) override;
	virtual void Die() override;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mesh")
		USkeletalMeshComponent* m_Mesh;

	//PICKUPS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "pickups")
		TSubclassOf<class AHealthPickup> m_HealthPickup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "pickups")
		TSubclassOf<class AResearchPickup> m_ResearchPickup;

	//DROPS
	UPROPERTY(EditAnywhere, Category = "drops")
		TSubclassOf<class ABomb> m_BombDrop;
	UPROPERTY(EditAnywhere, Category = "drops")
		int m_HealthChance;
	UPROPERTY(EditAnywhere, Category = "drops")
		int m_ResearchChance;
	UPROPERTY(EditAnywhere, Category = "drops")
		int m_BombChance;

	//ENEMY STATS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "stats")
		float m_MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "stats")
		float m_Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "stats")
		float m_BaseMoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "stats")
		float m_BulletSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "stats")
		float m_MaxAttackDelay;

	//ANIMATION
	bool m_IsAttacking;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
