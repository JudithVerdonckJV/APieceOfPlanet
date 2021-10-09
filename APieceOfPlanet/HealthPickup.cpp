#include "HealthPickup.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"

#include "PlayerCharacter.h"
#include "HealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

AHealthPickup::AHealthPickup()
	: m_HealingPoints{ 2.f }
{
 	PrimaryActorTick.bCanEverTick = false;

	m_Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	m_Collider->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnSphereBeginOverlap);
	m_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");

	SetRootComponent(m_Collider);
	m_Mesh->SetupAttachment(m_Collider);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthPickup::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor == player)
	{
		TArray<UActorComponent*> components = player->GetComponentsByClass(UHealthComponent::StaticClass());
		if (components.Num() > 0)
		{
			UHealthComponent* health = Cast<UHealthComponent>(components[0]);
			health->AddHealth(m_HealingPoints);
			if (m_HealEffect)
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					m_HealEffect,
					player->GetRootComponent(),
					"ParticleSocket",
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					EAttachLocation::SnapToTarget,
					true
				);
		}

		//score
		UGameInstance* GI{ GetGameInstance() };

		if (GI)
		{
			UMainGameInstance* MGI{ Cast<UMainGameInstance>(GI) };
			if (MGI) MGI->PickupTaken();
		}
		
		Destroy();
	}
}