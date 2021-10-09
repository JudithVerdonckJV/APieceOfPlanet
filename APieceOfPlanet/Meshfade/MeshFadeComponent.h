#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshFadeComponent.generated.h"

 class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class APIECEOFPLANET_API UMeshFadeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMeshFadeComponent();

	void SetToFade();

	void SetToDefault();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "materials")
		UMaterialInstance* m_DefaultMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "materials")
		UMaterialInstance* m_FadedMaterial;

	UStaticMeshComponent* m_MeshToFade;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
