// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PIFS_Types.generated.h"


class APhysicalInteractiveFoliageActor;
/*
	used to store foliage index and its component during the intraction detection
*/
USTRUCT(BlueprintType)
struct FIntractFoliageRecord {
	GENERATED_BODY()

	FIntractFoliageRecord() {}

	FIntractFoliageRecord(class UInstancedStaticMeshComponent* _FoliageComponent, int32 _FoliageIndex) {

		if (_FoliageComponent == nullptr) {
			UE_LOG(LogTemp, Error, TEXT(" FoliageComponent is nullptr !  FIntractFoliageRecord::FIntractFoliageRecord()"));
		}

		FoliageComponent = _FoliageComponent;
		FoliageIndex = _FoliageIndex;
	}

	/*
			the component that its foliage overlapped (intracted)
	*/
	class UInstancedStaticMeshComponent* FoliageComponent;
	/*
		Index of the foliage in the instaced component
	*/
	int32 FoliageIndex;

	/*
		returns the transform of the foliage
	*/
	FTransform GetFoliageTransform()const;

};

USTRUCT(BlueprintType)
struct FIntractFoliageSet {
	GENERATED_BODY()

	FIntractFoliageSet() {}
	/*
		the static mesh of the foliage that spawned in to the world
	*/
	UPROPERTY(BlueprintType, EditAnywhere)
	UStaticMesh* foliageStaticMesh;

	/*
		the BP class of the interactive foliage actor
	*/
	UPROPERTY(BlueprintType, EditAnywhere)
	TSubclassOf<APhysicalInteractiveFoliageActor> foliageClass;

};


UCLASS()
class PHYSICALINTERACTIVEFOLIAGESYSTEM_API UPIFS_Types : public UObject
{
	GENERATED_BODY()
	
};
