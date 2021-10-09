#include "AlienCoconut.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "Kismet/GameplayStatics.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PlanetGravityComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

#include "PlayerCharacter.h"
#include "BaseEnemy_Interface.h"

AAlienCoconut::AAlienCoconut()
	: m_Mesh{}
	, m_ColliderBlock{}
	, m_Gravity{}
	, m_Movement{}
	, m_Damage{ 1.f }
	, m_VelocityScale{ 1.f }
	, m_Slowing{ 0.01f }
	, m_Direction{}
{
 	PrimaryActorTick.bCanEverTick = true;

	m_Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	m_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_ColliderBlock = CreateDefaultSubobject<USphereComponent>("ColliderBlock");
	m_ColliderBlock->SetCollisionProfileName(FName("BlockAll"));
	m_ColliderBlock->OnComponentHit.AddDynamic(this, &AAlienCoconut::OnHit);
	
	m_Gravity = CreateDefaultSubobject<UPlanetGravityComponent>("PlanetGravity");
	m_Gravity->SetLineTraceDistance(500.f);
	m_Movement = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	m_Movement->MaxSpeed = 500.f;

	SetRootComponent(m_ColliderBlock);
	m_Mesh->SetupAttachment(m_ColliderBlock);
}

void AAlienCoconut::BeginPlay()
{
	Super::BeginPlay();
	SetActorScale3D({ 2.f, 2.f, 2.f });
	SetLifeSpan(2.f);
	FRandomStream randomVectorGenerator{};
	randomVectorGenerator.GenerateNewSeed();
	m_Direction = randomVectorGenerator.VRandCone(GetActorUpVector(), 3.14);
}

void AAlienCoconut::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_Gravity->ApplyGravity();
	m_Gravity->ApplyRotation();

	m_VelocityScale -= m_Slowing;
	if (m_VelocityScale <= 0.f) m_VelocityScale = 0.f;
	AddMovementInput(m_Direction, m_VelocityScale);
}

void AAlienCoconut::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAlienCoconut::OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (Cast<APlayerCharacter>(OtherActor) || Cast<IBaseEnemy_Interface>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, m_Damage, GetInstigatorController(), this, {});
		if (m_HitEffect) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_HitEffect, GetActorLocation(), GetActorRotation());
		Destroy();
	}
	else
	{
		m_Direction = {0.f, 0.f, 0.f};
	}
}