#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseEnemy_Interface.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UBaseEnemy_Interface : public UInterface
{
	GENERATED_BODY()
};

class APlayerCharacter;
class UCapsuleComponent;
class UPlanetGravityComponent;
class UFloatingPawnMovement;
class IBaseTile_Interface;

class AResearchCamp;

class APIECEOFPLANET_API IBaseEnemy_Interface
{
	GENERATED_BODY()
public:

	virtual void SetDamageMultiplier(float multiplier) = 0;

	virtual void SetHealthMultiplier(float multiplier) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool IsMelee() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool IsAttacking() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void SetIsAttacking(bool isAttacking);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DestroyEnemy();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		float GetDamage() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DropItem() const
		;
protected:
	APlayerCharacter* m_Player;
	AResearchCamp* m_Camp;

	UCapsuleComponent* m_Collider;
	UPlanetGravityComponent* m_Gravity;
	UFloatingPawnMovement* m_Movement;

	float m_CurrentHealth;

	float m_DetectRangeSquared;
	float m_AttackRangeSquared;
	float m_CurrentAttackDelay;
	float m_DamageMultiplier;
	float m_HealthMultiplier;
	bool m_WasAttacked;
	float m_MaxWasAttackedCooldown;
	float m_CurrentWasAttackedCooldown;

	virtual void Attack(float dt) = 0;
	virtual void MoveToPlayer() = 0;
	virtual void MoveToCamp(bool fullSpeed = true) = 0;
	virtual void Die() = 0;
};
