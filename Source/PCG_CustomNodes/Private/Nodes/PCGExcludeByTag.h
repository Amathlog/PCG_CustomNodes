#pragma once

#include "PCGSettings.h"
#include "PCGPin.h"

#include "PCGExcludeByTag.generated.h"

UCLASS(BlueprintType)
class UPCGExcludeByTagSettings : public UPCGSettings
{
    GENERATED_BODY()
public:
    UPCGExcludeByTagSettings();

    //~Begin UPCGSettings interface
#if WITH_EDITOR
    virtual FName GetDefaultNodeName() const override { return FName(TEXT("PCGExcludeByTag")); }
    virtual FText GetDefaultNodeTitle() const override { return NSLOCTEXT("ExcludeByTagSettings", "NodeTitle", "PCG Exclude by Tag"); }
    virtual FText GetNodeTooltipText() const override { return NSLOCTEXT("ExcludeByTagSettings", "NodeTooltip", "Works with Bounds or Splines"); }
    virtual EPCGSettingsType GetType() const override { return EPCGSettingsType::Spatial; }
#endif

    virtual FName AdditionalTaskName() const override;

protected:
    virtual TArray<FPCGPinProperties> InputPinProperties() const override { return TArray<FPCGPinProperties>(); }
    virtual TArray<FPCGPinProperties> OutputPinProperties() const override;
    virtual FPCGElementPtr CreateElement() const override;
    //~End UPCGSettings interface

public:
    /** If True, will use the fixed number of points. Ratio otherwise. */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config", meta = (PCG_Overridable))
    FString ActorTag{ TEXT("PCG_EXCLUDE") };
};

class FPCGExcludeByTagElement : public IPCGElement
{
protected:
    virtual bool ExecuteInternal(FPCGContext* Context) const override;
};