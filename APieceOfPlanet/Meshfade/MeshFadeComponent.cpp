#include "MeshFadeComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UMeshFadeComponent::UMeshFadeComponent()
	: m_DefaultMaterial{ nullptr }
	, m_FadedMaterial{ nullptr }
	, m_MeshToFade{ nullptr }
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UMeshFadeComponent::BeginPlay()
{
	Super::BeginPlay();

	//Get the mesh to fade in/out. should only be one (complete environment) (can be changed btw, but it wont work for multiple ones atm)
	// Please, for the sake of everything that has to do with logic and tech, do not forget to add the TAGS.
	TArray<UActorComponent*> meshes = GetOwner()->GetComponentsByTag(UStaticMeshComponent::StaticClass(), "fadeable");
	if (meshes[0] != nullptr)
	{
		m_MeshToFade = Cast<UStaticMeshComponent>(meshes[0]);
	}

	//m_MeshToFade->bCastVolumetricTranslucentShadow = true;
}

void UMeshFadeComponent::SetToFade()
{
	if (m_MeshToFade == nullptr || m_DefaultMaterial == nullptr || m_FadedMaterial == nullptr) return;

	m_MeshToFade->SetMaterial(0, m_FadedMaterial);
}

void UMeshFadeComponent::SetToDefault()
{
	if (m_MeshToFade == nullptr || m_DefaultMaterial == nullptr || m_FadedMaterial == nullptr) return;

	m_MeshToFade->SetMaterial(0, m_DefaultMaterial);
}

void UMeshFadeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}