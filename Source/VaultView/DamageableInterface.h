// Fill out your copyright notice in the Description page of Project Settings.
// ==============================================================================
// AI DISCLOSURE AND DOCUMENTATION:
// This file was implemented entirely with the assistance of AI, to fulfill 
// the specific technical requirements of the UEC project.
// ==============================================================================

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
