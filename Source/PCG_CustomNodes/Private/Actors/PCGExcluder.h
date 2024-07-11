#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PCGComponent.h"
#include "Engine/StaticMeshActor.h"
#include "PCGExcluder.generated.h"

UCLASS(BlueprintType)
class APCGExcluder : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APCGExcluder();

	const float GetPCGSteepness() { return PCG_Steepness; }

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	void RefreshPCG();
	double LastPCGRefreshTime{ 0.0f };
	void TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
	FDelegateHandle UpdateHandle;
#endif

private:
	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool PCGAutoRefresh{ true };
	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PCG_Steepness{ 1.0f };
};
