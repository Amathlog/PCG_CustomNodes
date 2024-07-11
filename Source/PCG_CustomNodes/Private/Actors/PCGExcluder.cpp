#include "Actors/PCGExcluder.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

APCGExcluder::APCGExcluder()
	: Super()
{

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(this, TEXT("/Engine/BasicShapes/Cube")));
	GetStaticMeshComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetStaticMeshComponent()->SetHiddenInGame(true);
	GetStaticMeshComponent()->SetHiddenInSceneCapture(true);
	GetStaticMeshComponent()->SetMaterial(0, LoadObject<UMaterialInterface>(this, TEXT("/Engine/BasicShapes/BasicShapeMaterial")));
	Tags.Add(FName(TEXT("PCG_EXCLUDE")));

//this is so we can auto update the PCG Component if we need to
#if WITH_EDITOR
	static bool bAddedDelegate = false;
	if (!bAddedDelegate)
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject* Object, FPropertyChangedEvent& PropertyChangedEvent)
			{
				if (!PropertyChangedEvent.Property)
				{
					return;
				}

				if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet &&
					PropertyChangedEvent.GetPropertyName().ToString().StartsWith(TEXT("PCG_")))
				{
					if (APCGExcluder* Excluder = Cast<APCGExcluder>(Object))
						Excluder->RefreshPCG();
					return;
				}
			});
	}
#endif
}

void APCGExcluder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (!UpdateHandle.IsValid())
		UpdateHandle = GetRootComponent()->TransformUpdated.AddUObject(this, &APCGExcluder::TransformUpdated);
#endif
}

#if WITH_EDITOR
void APCGExcluder::RefreshPCG()
{
	if (PCGAutoRefresh && LastPCGRefreshTime < FPlatformTime::Seconds())
	{
		LastPCGRefreshTime = FPlatformTime::Seconds() + 0.3f;

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);

		FBox MyBox = GetComponentsBoundingBox(true);

		for (AActor* Actor : OutActors)
		{
			if (!IsValid(Actor))
				continue;

			FBox box = Actor->GetComponentsBoundingBox(true);

			if (box.IsInsideOrOn(MyBox) || box.Intersect(MyBox))
			{
				TArray<UPCGComponent*> PCGs;
				Actor->GetComponents<UPCGComponent>(PCGs);

				for (UPCGComponent* PCG : PCGs)
				{
					PCG->CleanupLocalImmediate(false);
					PCG->GenerateLocal(true);
				}
			}
		}
	}
}

void APCGExcluder::TransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	RefreshPCG();
}

#endif
