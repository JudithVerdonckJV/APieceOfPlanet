#include "DesertTile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

#include "PlayerCharacter.h"
#include "HealthComponent.h"
#include "DesertEnemy_Melee.h"
#include "DesertEnemy_Ranged.h"

#include "Delegates.h"


ADesertTile::ADesertTile()
	: m_EnvironmentMesh{}
	, m_DesertMesh{}
	, m_OasisMesh{}
	, m_IsOasis{}
	, m_HeatDamageCooldownMax{ 3.f }
	, m_PlayerOnTile{ false }
	, m_Player{}
	, m_HealingPoolMax{6.f}
	, m_HealingPoolCurrent{6.f}
	, m_HealingPower{2.f}
	, m_HealingCooldownMax{2.f}
	, m_HealPlayer{false}
	, m_HealingCooldownCurrent{0.f}
{
 	PrimaryActorTick.bCanEverTick = true;

	m_Health = 2;
	m_Biome = EBiome::Desert;
	m_IsBroken = false;
	
	m_GroundStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("GroundTile");
	m_GroundStaticMesh->SetStaticMesh(m_TileStage1);

	m_GroundSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("GroundTileStage");
	m_GroundSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_GroundSkeletalMesh->SetVisibility(false);

	m_EnvironmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("Environment");

	SetRootComponent(m_GroundStaticMesh);
	m_GroundSkeletalMesh->SetupAttachment(m_GroundStaticMesh);
	m_EnvironmentMesh->SetupAttachment(m_GroundStaticMesh);
}

void ADesertTile::BeginPlay()
{
	Super::BeginPlay();

	m_Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0));

	int type = UKismetMathLibrary::RandomIntegerInRange(0, 4);

	if (type == 0)
	{
		m_IsOasis = true;
		m_EnvironmentMesh->SetStaticMesh(m_OasisMesh);
		m_EnvironmentMesh->SetRenderCustomDepth(true);
		m_EnvironmentMesh->SetCustomDepthStencilValue(1);
		m_EnvironmentMesh->MarkRenderStateDirty();
	}
	else
	{
		m_IsOasis = false;
		m_EnvironmentMesh->SetStaticMesh(m_DesertMesh);
	}

	m_HealingPoolCurrent = m_HealingPoolMax;
}

void ADesertTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (m_Player == nullptr) return;

	//HEAT DAMAGE
	if (!m_IsOasis && m_PlayerOnTile)
	{
		m_HeatDamageCooldownCurrent += DeltaTime;

		if (m_HeatDamageCooldownCurrent >= m_HeatDamageCooldownMax)
		{
			m_HeatDamageCooldownCurrent -= m_HeatDamageCooldownMax;
			UGameplayStatics::ApplyDamage(m_Player, 1.f, {}, {}, {});
			m_HeatShown = false;
		}
		if (!m_HeatShown && m_HeatDamageCooldownCurrent >= m_HealingCooldownMax / 2.f)
		{
			m_Player->ShowHeatEffect();
			m_HeatShown = true;
		}
	}

	//OASIS HEAL
	else if (m_HealPlayer && m_IsOasis)
	{
		m_HealingCooldownCurrent += DeltaTime;

		if (m_HealingCooldownCurrent >= m_HealingCooldownMax)
		{
			m_HealingCooldownCurrent -= m_HealingCooldownMax;
			

			TArray<UActorComponent*> components = m_Player->GetComponentsByClass(UHealthComponent::StaticClass());
			if (components.Num() > 0)
			{
				UHealthComponent* health = Cast<UHealthComponent>(components[0]);
				health->AddHealth(m_HealingPower);
				m_Player->ShowHealEffect();
			}

			m_HealingPoolCurrent -= m_HealingPower;
			if (m_HealingPoolCurrent <= 0)
			{
				m_IsOasis = false;
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_DisappearEffect, GetActorLocation() + 100.f * GetActorUpVector(), GetActorRotation());
				m_EnvironmentMesh->SetStaticMesh(m_DesertMesh);
				m_EnvironmentMesh->bRenderCustomDepth = false;
			}
		}
	}
}

void ADesertTile::SpawnEnemy(float damageMultiplier, float healthMultiplier)
{
	FVector location = { GetActorLocation() + 200.f * GetActorUpVector() };
	
	int type = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	IBaseEnemy_Interface* enemy{};
	if (type == 0) enemy = GetWorld()->SpawnActor<ADesertEnemy_Melee>(m_EnemyMelee, location, {});
	else enemy = GetWorld()->SpawnActor<ADesertEnemy_Ranged>(m_EnemyRanged, location, {});
	if (enemy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy in forestTile: enemy failed to spawn."));
		UDelegates::ENEMY_DIED_NOTIFY.ExecuteIfBound(); // tell enemymanager this enemy "died"
		return;
	}
	
	enemy->SetDamageMultiplier(damageMultiplier);
	enemy->SetHealthMultiplier(healthMultiplier);
}

void ADesertTile::PlayerEnterTile()
{
	UE_LOG(LogTemp, Warning, TEXT("Player Entered DesertTile"));

	m_PlayerOnTile = true;
	m_HeatDamageCooldownCurrent = 0.f;
	m_HealingCooldownCurrent = 0.f;
}

void ADesertTile::PlayerLeaveTile()
{
	m_PlayerOnTile = false;
}

void ADesertTile::DestroyTile_Implementation()
{
	Destroy();
}

bool ADesertTile::IsBroken_Implementation() const
{
	return m_IsBroken;
}

float ADesertTile::TakeDamage
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

bool ADesertTile::IsOasis() const
{
	return m_IsOasis;
}