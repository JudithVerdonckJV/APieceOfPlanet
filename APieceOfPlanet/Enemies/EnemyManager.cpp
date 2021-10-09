#include "EnemyManager.h"
#include "TileManager.h"
#include "Kismet/GameplayStatics.h"

#include "Delegates.h"

AEnemyManager::AEnemyManager()
	: m_EnemySpawnNr{ 10 }
	, m_EnemyNrCap{ 50 }
	, m_MaxWaveTimer{ 30.f }
	, m_FirstWaveTimer{ 10.f }
	, m_EnemyIncrease{ 2 }

	, m_HealthIncrease{ 0.2f }
	, m_DamageIncrease{ 0.1f }

	, m_CurrentNrOfEnemies{ 0 }
	, m_DamageMultiplier{ 1.f }
	, m_HealthMultiplier{ 1.f }
	, m_CurrentWaveTimer{ 0.f }
	, m_WaveNr{ 0 }
{
 	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), ATileManager::StaticClass(), foundActors);
	if (foundActors[0] != nullptr)
	{
		m_TileManager = Cast<ATileManager>(foundActors[0]);
	}

	UDelegates::ENEMY_DIED_NOTIFY.BindUFunction(this, FName("EnemyDied"));

	m_CurrentWaveTimer = m_MaxWaveTimer - m_FirstWaveTimer;
}

void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_CurrentWaveTimer += DeltaTime;
	if (m_CurrentWaveTimer > m_MaxWaveTimer)
	{
		SpawnWave();
	}
}

void AEnemyManager::SpawnWave()
{
	//decide how many enemies should spawn, stay below cap
	int spawnNr{ m_EnemySpawnNr };
	if (m_CurrentNrOfEnemies + m_EnemySpawnNr > m_EnemyNrCap)
	{
		spawnNr = m_EnemyNrCap - m_CurrentNrOfEnemies;
		if (spawnNr <= 0) return;
	}
	
	//spawn
	m_TileManager->SpawnEnemies(spawnNr, m_DamageMultiplier, m_HealthMultiplier);
	
	//set current nr of enemies to correct nr
	m_CurrentNrOfEnemies += spawnNr;

	//setup next wave
	m_CurrentWaveTimer = 0.f;
	++m_WaveNr;
	m_EnemySpawnNr += m_EnemyIncrease;
	m_DamageMultiplier += m_DamageIncrease;
	m_HealthMultiplier += m_HealthIncrease;
}

void AEnemyManager::EnemyDied()
{
	UE_LOG(LogTemp, Warning, TEXT("Message received: enemy died."));
	--m_CurrentNrOfEnemies;
	if (m_CurrentNrOfEnemies <= 0) SpawnWave();
}