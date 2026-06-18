// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class VAULTVIEW_API IDamageableInterface
{
	GENERATED_BODY()

public:
	// Function to implement in target classes
	virtual void TakeDamage(float DamageAmount) = 0;
};
