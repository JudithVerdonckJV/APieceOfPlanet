#include "TundraTile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

#include "TundraEnemy_Melee.h"
#include "TundraEnemy_Ranged.h"
#include "PlayerCharacter.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "Delegates.h"

ATundraTile::ATundraTile()
	: m_EnvironmentMesh{}
{
 	PrimaryActorTick.bCanEverTick = false;

	m_Health = 3;
	m_Biome = EBiome::Tundra;
	m_IsBroken = false;

	m_GroundStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("GroundTile");
	m_GroundStaticMesh->SetStaticMesh(m_TileStage1);

	m_GroundSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("GroundTileStage");
	m_GroundSkeletalMesh->SetVisibility(false);
	m_GroundSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_EnvironmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("Environment");

	SetRootComponent(m_GroundStaticMesh);
	m_GroundSkeletalMesh->SetupAttachment(m_GroundStaticMesh);
	m_EnvironmentMesh->SetupAttachment(m_GroundStaticMesh);
}

void ATundraTile::BeginPlay()
{
	Super::BeginPlay();

	m_Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0));
}

void ATundraTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATundraTile::SpawnEnemy(float damageMultiplier, float healthMultiplier)
{
	FVector location = { GetActorLocation() + 200.f * GetActorUpVector() };
	
	int type = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	IBaseEnemy_Interface* enemy{};
	if (type == 0) enemy = GetWorld()->SpawnActor<ATundraEnemy_Melee>(m_EnemyMelee, location, {});
	else enemy = GetWorld()->SpawnActor<ATundraEnemy_Ranged>(m_EnemyRanged, location, {});
	if (enemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy in forestTile: enemy failed to spawn."));
		UDelegates::ENEMY_DIED_NOTIFY.ExecuteIfBound(); // tell enemymanager this enemy "died"
		return;
	}
	enemy->SetDamageMultiplier(damageMultiplier);
	enemy->SetHealthMultiplier(healthMultiplier);
}

void ATundraTile::PlayerEnterTile()
{
	if (m_Player == nullptr) return;
	m_Player->CanBeStunned(true);
}

void ATundraTile::PlayerLeaveTile()
{
	if (m_Player == nullptr) return;
	m_Player->CanBeStunned(false);
}

void ATundraTile::DestroyTile_Implementation()
{
	Destroy();
}

bool ATundraTile::IsBroken_Implementation() const
{
	return m_IsBroken;
}

float ATundraTile::TakeDamage
(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	m_IsBroken = true;
	m_GroundSkeletalMesh->SetVisibility(true);
	m_GroundStaticMesh->SetStaticMesh(nullptr);
	m_GroundStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_EnvironmentMesh->SetVisibility(false);
	m_EnvironmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	return DamageAmount;
}
