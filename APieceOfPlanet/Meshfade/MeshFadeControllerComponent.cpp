#include "MeshFadeControllerComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "MeshFadeComponent.h"
#include "PersistentActorReferences.h"
#include "Engine/StaticMeshActor.h"

UMeshFadeControllerComponent::UMeshFadeControllerComponent()
	: m_Camera{ nullptr }
	, m_Player{ nullptr }
	, m_MeshToFade{ nullptr }
	, m_PreviousMesh{ nullptr }
	, m_EndOffset{ 100.f }
	, m_SphereRadius{ 100.f }
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMeshFadeControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	m_Player = Cast<APawn>(GetOwner());
	m_Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	m_WalkablePlanet = APersistentActorReferences::instance->GetWalkablePlanet();
}

void UMeshFadeControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_Player == nullptr || m_Camera == nullptr) return;

	//LINETRACING
	FCollisionQueryParams linetraceParams{ };
	linetraceParams.AddIgnoredActor(m_Player);
	linetraceParams.AddIgnoredActor(m_WalkablePlanet);

	FVector start{ m_Camera->GetCameraLocation() };
	FVector end{ m_Player->GetActorLocation() };
	FVector direction{ end - start };
	direction.Normalize();
	end -= m_EndOffset * direction; // do not trace all the way to the ground

	FHitResult outHit{};

	bool isOccluded = GetWorld()->SweepSingleByChannel(outHit, start, end, {}, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(m_SphereRadius), linetraceParams);

	//SWAP MATERIALS IF NECESSARY
	if (isOccluded)
	{
		UActorComponent* meshFade = outHit.Actor->GetComponentByClass(UMeshFadeComponent::StaticClass());
		if (meshFade != nullptr) 
		{
			//Hit something, is fadeable
			m_MeshToFade = Cast<UMeshFadeComponent>(meshFade);
			if (m_MeshToFade == m_PreviousMesh) return;

			m_MeshToFade->SetToFade();
			if (m_PreviousMesh != nullptr) m_PreviousMesh->SetToDefault();
			m_PreviousMesh = m_MeshToFade;

		}
		else
		{
			//Hit something, but is not fadeable
			m_MeshToFade = nullptr;
			if (m_PreviousMesh != nullptr) m_PreviousMesh->SetToDefault();
			m_PreviousMesh = nullptr;
		}
	}
	else
	{
		//Hit nothing
		if (m_MeshToFade != nullptr)
		{
			m_MeshToFade->SetToDefault();
			m_MeshToFade = nullptr;
		}
		if (m_PreviousMesh != nullptr)
		{
			m_PreviousMesh->SetToDefault();
			m_PreviousMesh = nullptr;
		}
	}
}