#include "ForestTile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

#include "Components/ArrowComponent.h"

#include "PlayerCharacter.h"
#include "ForestEnemyMelee.h"
#include "ForestEnemy_Ranged.h"
#include "AlienCoconut.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "Delegates.h"

AForestTile::AForestTile()
	: m_EnvironmentMesh{}
	, m_CoconutSpawnPoint{}
	, m_AccumulatedTime{ 1.5f }
	, m_MinTime{ 3.f }
	, m_MaxTime{ 5.f }
{
 	PrimaryActorTick.bCanEverTick = true;

	m_Health = 2;
	m_Biome = EBiome::Forest;
	m_IsBroken = false;

	m_GroundStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("GroundTile");
	m_GroundStaticMesh->SetStaticMesh(m_TileStage1);

	m_GroundSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("GroundTileStage");
	m_GroundSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_GroundSkeletalMesh->SetVisibility(false);

	m_EnvironmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("Environment");
	m_EnvironmentMesh->SetEnableGravity(false);

	m_CoconutSpawnPoint = CreateDefaultSubobject<UArrowComponent>("CoconutSpawnpoint");

	SetRootComponent(m_GroundStaticMesh);
	m_GroundSkeletalMesh->SetupAttachment(m_GroundStaticMesh);
	m_EnvironmentMesh->SetupAttachment(m_GroundStaticMesh);
	m_CoconutSpawnPoint->SetupAttachment(m_GroundStaticMesh);
}

void AForestTile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AForestTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnCoconut(DeltaTime);
}

void AForestTile::SpawnEnemy(float damageMultiplier, float healthMultiplier)
{
	FVector location = { GetActorLocation() + 200.f * GetActorUpVector() };
	
	int type = UKismetMathLibrary::RandomIntegerInRange(0, 1);
	
	IBaseEnemy_Interface* enemy{};
	if (type == 0) enemy = GetWorld()->SpawnActor<AForestEnemyMelee>(m_EnemyMelee, location, {});
	else enemy = GetWorld()->SpawnActor<AForestEnemy_Ranged>(m_EnemyRanged, location, {});	
	if (enemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy in forestTile: enemy failed to spawn."));
		UDelegates::ENEMY_DIED_NOTIFY.ExecuteIfBound(); // tell enemymanager this enemy "died"
		return;
	}
	enemy->SetDamageMultiplier(damageMultiplier);
	enemy->SetHealthMultiplier(healthMultiplier);
}

void AForestTile::PlayerEnterTile()
{
	Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->AddMultiplier(EUpgrade::PlayerSpeed, 0.3);
}

void AForestTile::PlayerLeaveTile()
{
	Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->AddMultiplier(EUpgrade::PlayerSpeed, -0.3);
}

void AForestTile::DestroyTile_Implementation()
{
	Destroy();
}

bool AForestTile::IsBroken_Implementation() const
{
	return m_IsBroken;
}

float AForestTile::TakeDamage
(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	m_Health -= DamageAmount;
	switch (m_Health)
	{
	case 1:
		m_GroundStaticMesh->SetStaticMesh(m_TileStage2);
		break;
	default:
		m_IsBroken = true;
		m_GroundSkeletalMesh->SetVisibility(true);
		m_GroundStaticMesh->SetVisibility(false);
		m_GroundStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_EnvironmentMesh->SetVisibility(false);
		m_EnvironmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}

	return DamageAmount;
}

void AForestTile::SpawnCoconut(float dt)
{
	m_AccumulatedTime += dt;

	if (m_AccumulatedTime >= m_MinTime)
	{
		int randInt = UKismetMathLibrary::RandomIntegerInRange(0, 1000);
		if (randInt <= 2 || m_AccumulatedTime >= m_MaxTime)
		{
			GetWorld()->SpawnActor<AAlienCoconut>(m_AlienCoconut, m_CoconutSpawnPoint->GetComponentTransform());
			m_AccumulatedTime = 0.f;
		}
	}
}