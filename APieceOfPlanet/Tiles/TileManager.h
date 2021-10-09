#pragma once
#include <vector>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileManager.generated.h"

class IBaseTile_Interface;
class AEnemyManager;

UCLASS()
class APIECEOFPLANET_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATileManager();

	UPROPERTY(EditAnywhere, Category = "Generation")
		float Radius;
	UPROPERTY(EditAnywhere, Category = "Generation")
		float Scale;

	void SpawnEnemies(int nr, float damageMultiplier, float healthMultiplier);
	FTransform GetNewCampLocation();
	AActor* GetCampTile();
	
	
protected:
	virtual void BeginPlay() override;

	//Planet Generation
	std::vector<FVector> m_VertexBuffer;
	std::vector<FVector> m_IndexBuffer;

	UPROPERTY()
		TArray<AActor*> m_pTiles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tiles")
		TSubclassOf<class AForestTile> ForestTest;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tiles")
		TSubclassOf<class ADesertTile> DesertTest;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tiles")
		TSubclassOf<class ATundraTile> TundraTest;
public:	
	virtual void Tick(float DeltaTime) override;

private:
	FVector CalculateTriangleMiddlePoint(int nr);
	FRotator CalculateTriangleRotation(int nr, FVector middlePoint);

	UPROPERTY()
		AActor* m_pCampTile;
};
