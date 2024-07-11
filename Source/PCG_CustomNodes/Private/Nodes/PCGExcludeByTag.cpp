#include "Nodes/PCGExcludeByTag.h"

#include "PCGComponent.h"
#include "PCGContext.h"
#include "PCGModule.h"
#include "PCGParamData.h"
#include "Data/PCGPointData.h"
#include "Helpers/PCGHelpers.h"

#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Actors/PCGExcluder.h"
#include "Actors/SnapToSplineMesh.h"


#define LOCTEXT_NAMESPACE "PCGExcludeByTagSettings"

namespace PCGExcludeByTagSettings
{
    static const FName OutLabel = TEXT("Out");
}

UPCGExcludeByTagSettings::UPCGExcludeByTagSettings()
{
    bUseSeed = true;
}

FName UPCGExcludeByTagSettings::AdditionalTaskName() const
{
    return NAME_None;
}

TArray<FPCGPinProperties> UPCGExcludeByTagSettings::OutputPinProperties() const
{
    TArray<FPCGPinProperties> Properties;

    Properties.Emplace(PCGExcludeByTagSettings::OutLabel, EPCGDataType::Point);

    return Properties;
}

FPCGElementPtr UPCGExcludeByTagSettings::CreateElement() const
{
    return MakeShared<FPCGExcludeByTagElement>();
}

bool FPCGExcludeByTagElement::ExecuteInternal(FPCGContext* Context) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FPCGExcludeByTagElement::Execute);

    check(Context);

    const UPCGExcludeByTagSettings* Settings = Context->GetInputSettings<UPCGExcludeByTagSettings>();
    check(Settings);

    int32 NumberOfExcludedPoints = 0;

    check(Context->SourceComponent.IsValid());
    UWorld* World = Context->SourceComponent->GetWorld();

    FRandomStream RandomStream(Context->GetSeed());

    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClassWithTag(World, AActor::StaticClass(), FName(Settings->ActorTag), OutActors);

    //OutputData.TaggedData is empty so we'll reset and add 1 entry
    TArray<FPCGTaggedData>& Outputs = Context->OutputData.TaggedData;
    Outputs.Reset();
    Outputs.Add(FPCGTaggedData());

    //setup OutPin Data
    UPCGPointData* ExcludePointsData = NewObject<UPCGPointData>();
    Outputs[0].Data = ExcludePointsData;
    Outputs[0].Pin = PCGExcludeByTagSettings::OutLabel;

    TArray<FPCGPoint>& ExcludePoints = ExcludePointsData->GetMutablePoints();

    for (AActor* Actor : OutActors)
    {
        if (!IsValid(Actor))
            continue;

        float Steepness = 1.0f;
        float MinBoundsMultiplier = 1.0f;
        float MaxBoundsMultiplier = 1.0f;

        //check for excluders and SnapToSplineMesh
        if (ASnapToSplineMesh* SnapSpline = Cast<ASnapToSplineMesh>(Actor))
        {
            Steepness = SnapSpline->GetPCGSteepness();
            MinBoundsMultiplier = SnapSpline->GetPCGMinBoundsMultiplier();
            MaxBoundsMultiplier = SnapSpline->GetPCGMaxBoundsMultiplier();
        }
        else if (APCGExcluder* Excluder = Cast<APCGExcluder>(Actor))
        {
            Steepness = Excluder->GetPCGSteepness();
        }

        TArray< USplineMeshComponent*> SMCs;
        Actor->GetComponents<USplineMeshComponent>(SMCs);

        if (SMCs.Num() > 0)
        {
            //loop through spline mesh comments and add a point for each
            for (USplineMeshComponent* SMC : SMCs)
            {
                FPCGPoint Point;
                Point.Density = 1.0f;
                Point.Steepness = Steepness;

                FVector Origin, BoxExtent;
                float SphereRadius;

                UKismetSystemLibrary::GetComponentBounds(SMC, Origin, BoxExtent, SphereRadius);
                Point.Seed = PCGHelpers::ComputeSeedFromPosition(Origin);
                Point.BoundsMin = BoxExtent * -1.0f * MinBoundsMultiplier;
                Point.BoundsMax = BoxExtent * MaxBoundsMultiplier;

                float StartRoll = SMC->GetStartRoll();
                float EndRoll = SMC->GetEndRoll();

                // Convert StartRoll and EndRoll to quaternions
                FQuat StartQuat = FQuat(FRotator(0.0f, 0.0f, StartRoll));
                FQuat EndQuat = FQuat(FRotator(0.0f, 0.0f, EndRoll));
                FQuat InterpolatedQuat = FQuat::Slerp(StartQuat, EndQuat, 0.5f);
                FRotator InterpolatedRotation = InterpolatedQuat.Rotator();

                Point.Transform = FTransform(InterpolatedRotation, Origin, SMC->GetComponentScale());
                ExcludePoints.Add(Point);
            }
        }
        else //just use object bounds
        {
            FPCGPoint Point;
            Point.Steepness = Steepness;
            Point.Density = 1.0f;

            FBox Bounds = PCGHelpers::GetActorLocalBounds(Actor, true);
            Point.Transform = FTransform(Actor->GetActorRotation(), Actor->GetActorLocation(), Actor->GetActorScale3D());
            Point.Seed = PCGHelpers::ComputeSeedFromPosition(Actor->GetActorLocation());
            Point.BoundsMin = Bounds.Min;
            Point.BoundsMax = Bounds.Max;
            ExcludePoints.Add(Point);
        }
    }

    NumberOfExcludedPoints = ExcludePoints.Num();

    // Create Attribute set
    UPCGParamData* ParamData = NewObject<UPCGParamData>();
    FPCGMetadataAttribute<int32>* Attribute = ParamData->Metadata->CreateAttribute<int32>(TEXT("ExcludePointsNum"), NumberOfExcludedPoints, true, true);
    Attribute->SetValue(ParamData->Metadata->AddEntry(), NumberOfExcludedPoints);

    return true;
}

#undef LOCTEXT_NAMESPACE