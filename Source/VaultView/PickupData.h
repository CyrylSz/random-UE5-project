#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PickupData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VAULTVIEW_API UPickupData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	int32 ScoreChange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Properties")
	float HealthPointsChange = 0.0f;
};
