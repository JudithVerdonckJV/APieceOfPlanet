#include "PlanetGravityComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "DrawDebugHelpers.h"

#include "PersistentActorReferences.h"
#include "Engine/StaticMeshActor.h"

UPlanetGravityComponent::UPlanetGravityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPlanetGravityComponent::BeginPlay()
{
	Super::BeginPlay();

	m_Owner = Cast<APawn>(GetOwner());
	m_WalkablePlanet = APersistentActorReferences::instance->GetWalkablePlanet();
}

void UPlanetGravityComponent::ApplyGravity()
{
	//if (!m_Owner) return;
	m_Owner->AddMovementInput(m_Owner->GetActorUpVector(), -1.f);
}

void UPlanetGravityComponent::ApplyRotation()
{
	//if (!m_Owner) return;
	FRotator rotation = UKismetMathLibrary::MakeRotFromZX(GetOwner()->GetActorLocation() - m_GravityCenter, GetOwner()->GetActorForwardVector());
	GetOwner()->SetActorRotation(rotation);
}

bool UPlanetGravityComponent::IsGrounded(FHitResult& hit)
{
	if (!m_Owner) return false;
	
	FCollisionQueryParams linetraceParams{ };
	linetraceParams.AddIgnoredActor(m_Owner);
	linetraceParams.AddIgnoredActor(Cast<AActor>(m_WalkablePlanet));
	
	FVector start{ m_Owner->GetActorLocation() };
	FVector end{ start - (m_Owner->GetActorUpVector() * m_LineTraceDistance) };

	//DrawDebugLine(GetWorld(), start, end, { 255, 0, 0, 0 });
	return GetWorld()->LineTraceSingleByChannel(hit, start, end, ECollisionChannel::ECC_WorldStatic, linetraceParams);
}



void UPlanetGravityComponent::SetLineTraceDistance(float lineDistance)
{
	m_LineTraceDistance = lineDistance;
}
