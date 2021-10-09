#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshFadeControllerComponent.generated.h"

class APlayerCameraManager;
class APawn;
class UMeshFadeComponent;
class AStaticMeshActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APIECEOFPLANET_API UMeshFadeControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMeshFadeControllerComponent();

protected:
	virtual void BeginPlay() override;

	APlayerCameraManager* m_Camera;
	APawn* m_Player;
	UMeshFadeComponent* m_MeshToFade;
	UMeshFadeComponent* m_PreviousMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_EndOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_SphereRadius;
		
	AStaticMeshActor* m_WalkablePlanet;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
