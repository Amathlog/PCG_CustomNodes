#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SnapToSplineMesh.generated.h"

class UMaterialInterface;
class UStaticMesh;
class USplineMeshComponent;

USTRUCT(BlueprintType)
struct FSplineMeshInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector End;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EndTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator EndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawStartTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RawStartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RawEndTangent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RawEndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USplineMeshComponent> MeshComponent = nullptr;

	FSplineMeshInfo()
	{
		Start = FVector::ZeroVector;
		StartTangent = FVector::ZeroVector;
		StartRotation = FRotator::ZeroRotator;

		End = FVector::ZeroVector;
		EndTangent = FVector::ZeroVector;
		EndRotation = FRotator::ZeroRotator;

		RawStart = FVector::ZeroVector;
		RawStartTangent = FVector::ZeroVector;
		RawStartRotation = FRotator::ZeroRotator;

		RawEnd = FVector::ZeroVector;
		RawEndTangent = FVector::ZeroVector;
		RawEndRotation = FRotator::ZeroRotator;

		MeshComponent = nullptr;
	}

	FSplineMeshInfo(FVector InStart, FVector InStartTangent, FRotator InStartRotation,
					  FVector InEnd, FVector InEndTangent, FRotator InEndRotation,
					  FVector InRawStart, FVector InRawStartTangent, FRotator InRawStartRotation,
					  FVector InRawEnd, FVector InRawEndTangent, FRotator InRawEndRotation,
					  USplineMeshComponent * InMeshComponent)
	{
		Start = InStart;
		StartTangent = InStartTangent;
		StartRotation = InStartRotation;

		End = InEnd;
		EndTangent = InEndTangent;
		EndRotation = InEndRotation;

		RawStart = InRawStart;
		RawStartTangent = InRawStartTangent;
		RawStartRotation = InRawStartRotation;

		RawEnd = InRawEnd;
		RawEndTangent = InRawEndTangent;
		RawEndRotation = InRawEndRotation;

		MeshComponent = InMeshComponent;
	}
};


UCLASS(BlueprintType)
class ASnapToSplineMesh : public AActor
{
	GENERATED_BODY()

public:
	ASnapToSplineMesh();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "GothGirl")
	void MakeSplineMesh();

#if WITH_EDITOR
	void RefreshPCG();
	double LastPCGRefreshTime{ 0.0f };
	void RefreshEditor(bool ForceRefresh);
	double LastEditorTime{ 0.0f };
	void TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
	FDelegateHandle UpdateHandle;
#endif

	const float GetPCGSteepness() { return PCG_Steepness; }

	const float GetPCGMinBoundsMultiplier() { return PCG_MinBoundsMultiplier; }

	const float GetPCGMaxBoundsMultiplier() { return PCG_MaxBoundsMultiplier; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> Spline = nullptr;

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool SnappingOn{ true };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ZOffset{ 0.25f };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TraceDistance{ 200.0f };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool PCGAutoRefresh{ true };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PCG_Steepness{ 1.0f };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = 0.0))
	float PCG_MinBoundsMultiplier{ 1.0f };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = 0.0))
	float PCG_MaxBoundsMultiplier{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ETraceTypeQuery> TraceQueryType{ ETraceTypeQuery::TraceTypeQuery1 };

	UPROPERTY(Category = "Config", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType{ EDrawDebugTrace::Type::None };

	bool DoLineTraceSingle(FHitResult& OutHit, const FVector& Start, const FVector& End, const TArray<AActor*> ActorsToIgnore, FLinearColor TraceColour = FLinearColor::Red);

	UPROPERTY()
	TArray<FSplineMeshInfo> SplinePoints;

	void CreateFromCache();

	int LastPointCount{ 0 };
	FTransform LastParentTransform{ FTransform::Identity };
};