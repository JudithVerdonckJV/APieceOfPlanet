#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

class ATileManager;

UCLASS()
class APIECEOFPLANET_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyManager();

	void SpawnWave();

	UFUNCTION()
	void EnemyDied();

protected:
	virtual void BeginPlay() override;	
	
	UPROPERTY(EditAnywhere, Category = "Waves")
		int m_EnemySpawnNr;
	UPROPERTY(EditAnywhere, Category = "Waves")
		int m_EnemyNrCap;
	UPROPERTY(EditAnywhere, Category = "Waves")
		float m_MaxWaveTimer;
	UPROPERTY(EditAnywhere, Category = "Waves")
		float m_FirstWaveTimer;
	UPROPERTY(EditAnywhere, Category = "Waves")
		int m_EnemyIncrease;

	UPROPERTY(EditAnywhere, Category = "Stat")
		float m_HealthIncrease;
	UPROPERTY(EditAnywhere, Category = "Stat")
		float m_DamageIncrease;
	
	int m_CurrentNrOfEnemies;
	float m_DamageMultiplier;
	float m_HealthMultiplier;
	float m_CurrentWaveTimer;
	int m_WaveNr;

	ATileManager* m_TileManager;

public:	
	virtual void Tick(float DeltaTime) override;
};
