#include "ForestEnemy_Ranged.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MainGameInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PlanetGravityComponent.h"

#include "PlayerCharacter.h"
#include "Delegates.h"
#include "BaseTile_Interface.h"
#include "ResearchCamp.h"
#include "PersistentActorReferences.h"
#include "Bomb.h"
#include "Bullet.h"
#include "HealthPickup.h"
#include "ResearchPickup.h"

AForestEnemy_Ranged::AForestEnemy_Ranged()
	: m_Mesh{}
	, m_HealthPickup{}
	, m_ResearchPickup{}
	, m_BombDrop{}
	, m_HealthChance{}
	, m_ResearchChance{}
	, m_BombChance{}
	, m_MaxHealth{ 2.f }
	, m_Damage{ 2.f }
	, m_BaseMoveSpeed{ 300.f }
	, m_IsAttacking{ false }
	, m_MaxAttackDelay{ 2.f }
	, m_BulletSpeed{ 500.f }
{
 	PrimaryActorTick.bCanEverTick = true;

	//COMPONENTS
	m_Collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	m_Collider->SetCollisionProfileName("BlockAll");
	m_Collider->SetSimulatePhysics(false);
	m_Collider->SetEnableGravity(false);
	m_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	m_Mesh->SetCollisionProfileName("NoCollision");

	SetRootComponent(m_Collider);
	m_Mesh->SetupAttachment(m_Collider);

	m_Movement = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	m_Movement->MaxSpeed = m_BaseMoveSpeed;
	m_Gravity = CreateDefaultSubobject<UPlanetGravityComponent>("GravityComponent");

	//PROTECTED VARIABLES
	m_CurrentHealth = 2.f;
	m_DetectRangeSquared = 700000.f;
	m_AttackRangeSquared = 400000.f;
	m_CurrentAttackDelay = 0.f;
	m_DamageMultiplier = 1.f;
	m_HealthMultiplier = 1.f;

	m_WasAttacked = false;
	m_MaxWasAttackedCooldown = 3.f;
	m_CurrentWasAttackedCooldown = 0.f;
}

void AForestEnemy_Ranged::BeginPlay()
{
	Super::BeginPlay();

	m_Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0));
	m_Camp = APersistentActorReferences::instance->GetCamp();
}

void AForestEnemy_Ranged::SetDamageMultiplier(float multiplier)
{
	m_DamageMultiplier = multiplier;
	m_Damage *= m_DamageMultiplier;
}

void AForestEnemy_Ranged::SetHealthMultiplier(float multiplier)
{
	m_HealthMultiplier = multiplier;
	m_MaxHealth *= m_HealthMultiplier;
	m_CurrentHealth = m_MaxHealth;
}

float AForestEnemy_Ranged::GetMaxHealth_Implementation() const
{
	return m_MaxHealth;
}

float AForestEnemy_Ranged::GetCurrentHealth_Implementation() const
{
	return m_CurrentHealth;
}

bool AForestEnemy_Ranged::IsMelee_Implementation() const
{
	return false;
}

bool AForestEnemy_Ranged::IsAttacking_Implementation() const
{
	return m_IsAttacking;
}

void AForestEnemy_Ranged::SetIsAttacking_Implementation(bool isAttacking)
{
	m_IsAttacking = isAttacking;
}

void AForestEnemy_Ranged::DestroyEnemy_Implementation()
{
	UDelegates::ENEMY_DIED_NOTIFY.ExecuteIfBound();
	Destroy();
}

float AForestEnemy_Ranged::GetDamage_Implementation() const
{
	return m_Damage;
}

void AForestEnemy_Ranged::DropItem_Implementation() const
{
	int chance = UKismetMathLibrary::RandomIntegerInRange(0, 100);

	if (chance < m_HealthChance)
	{
		GetWorld()->SpawnActor<AHealthPickup>(m_HealthPickup, GetActorTransform());
	}
	else if (chance < m_HealthChance + m_ResearchChance)
	{
		GetWorld()->SpawnActor<AResearchPickup>(m_ResearchPickup, GetActorTransform());
	}
	else if (chance < m_HealthChance + m_ResearchChance + m_BombChance)
	{
		FActorSpawnParameters params{};
		FVector position = GetRootComponent()->GetComponentLocation();
		FRotator rotation = GetRootComponent()->GetRelativeRotation();

		ABomb* spawnedBomb = GetWorld()->SpawnActor<ABomb>(m_BombDrop, position, rotation, params);
	}
}

void AForestEnemy_Ranged::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_Gravity->ApplyRotation();

	//GRAVITY
	FHitResult hit{};
	bool onGround = m_Gravity->IsGrounded(hit);
	if (!onGround) m_Collider->SetCollisionProfileName(FName("OverlapAll"));

	if (!onGround && m_CurrentHealth <= 0.9) // if dead and not on tile, still apply gravity, then return
	{
		m_Gravity->ApplyGravity();
		m_Gravity->ApplyRotation();
		return;
	}
	else if (m_CurrentHealth <= 0.9f) return; // if dead and on tile, simply return (no gravity applied, no AI)

	m_Gravity->ApplyGravity();

	//ICE TILE MOVEMENT
	IBaseTile_Interface* tile = Cast<IBaseTile_Interface>(hit.Actor);
	if (tile && tile->GetBiome() == EBiome::Tundra)
	{
		m_Movement->MaxSpeed = m_BaseMoveSpeed / 2.f;
	}
	else
	{
		m_Movement->MaxSpeed = m_BaseMoveSpeed;
	}

	if (m_Camp == nullptr) return;
	if (m_Player == nullptr) return;

	//SIMPLE AI
	if (m_WasAttacked)
	{
		m_CurrentWasAttackedCooldown += DeltaTime;
		if (m_CurrentWasAttackedCooldown > m_MaxWasAttackedCooldown)
		{
			m_CurrentWasAttackedCooldown = 0.f;
			m_WasAttacked = false;
		}
	}

	float campDistanceSquared{ UKismetMathLibrary::Vector_DistanceSquared(GetActorLocation(), m_Camp->GetActorLocation()) };
	float playerDistanceSquared{ UKismetMathLibrary::Vector_DistanceSquared(GetActorLocation(), m_Player->GetActorLocation()) };

	//AI:
	// - first prioritize camp, for a short distance.
	// - then check if player is near and handle follow/attack
	// - then look if camp is nearby
	// if nothing applies, idle

	if (campDistanceSquared < m_DetectRangeSquared / 10.f && !m_WasAttacked)
	{
		MoveToCamp();
	}
	else if (playerDistanceSquared < m_DetectRangeSquared || m_WasAttacked)
	{
		if (playerDistanceSquared < m_AttackRangeSquared)
		{
			FVector direction{ m_Player->GetActorLocation() - GetActorLocation() };
			FRotator rotation = UKismetMathLibrary::MakeRotFromZX(GetActorUpVector(), direction);
			SetActorRotation(rotation);
			Attack(DeltaTime);
			return;
		}
		else MoveToPlayer();
	}
	else if (campDistanceSquared < m_DetectRangeSquared)
	{
		MoveToCamp();
	}
}

void AForestEnemy_Ranged::Attack(float dt)
{
	m_CurrentAttackDelay += dt;
	if (m_CurrentAttackDelay >= m_MaxAttackDelay)
	{
		m_IsAttacking = true;
		m_CurrentAttackDelay -= m_MaxAttackDelay;
	}
}

float AForestEnemy_Ranged::TakeDamage
(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	m_CurrentHealth -= DamageAmount;
	m_WasAttacked = true;
	if (m_CurrentHealth <= 0.9f) Die();
	return DamageAmount;
}

void AForestEnemy_Ranged::Die()
{
	SetCanBeDamaged(false);
	SetActorEnableCollision(false);

	//score
	//score
	UGameInstance* GI{ GetGameInstance() };

	if (GI)
	{
		UMainGameInstance* MGI{ Cast<UMainGameInstance>(GI) };
		if (MGI) MGI->EnemyDied();
	}
}

void AForestEnemy_Ranged::MoveToPlayer()
{
	FVector direction{ m_Player->GetActorLocation() - GetActorLocation() };
	FRotator rotation = UKismetMathLibrary::MakeRotFromZX(GetActorUpVector(), direction);
	SetActorRotation(rotation);
	AddMovementInput(GetActorForwardVector(), 1.f);
}

void AForestEnemy_Ranged::MoveToCamp(bool fullSpeed)
{
	FVector direction{ m_Camp->GetActorLocation() - GetActorLocation() };
	FRotator rotation = UKismetMathLibrary::MakeRotFromZX(GetActorUpVector(), direction);
	SetActorRotation(rotation);

	float speed = fullSpeed ? 1.f : 0.1f;
	AddMovementInput(GetActorForwardVector(), speed);
}

void AForestEnemy_Ranged::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}