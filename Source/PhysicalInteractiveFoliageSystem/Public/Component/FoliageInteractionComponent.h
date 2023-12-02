// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FoliageInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PHYSICALINTERACTIVEFOLIAGESYSTEM_API UFoliageInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFoliageInteractionComponent();

	/*
		list of all interactive foliages
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Intractive Foliage | Interactive Foliage Actors")
	TArray<struct FIntractFoliageSet> FoliageActorsSet;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
		check for any foliage static mesh around the actor
		and replaces them with proper Intractive foliage actor if provided
	*/
	UFUNCTION(BlueprintCallable, Category = "Interactive Foliage")
	void ActivateFoliagesInRange(float ActivationRadius = 30, bool bDebug = false);

	/*
		check if any blueprint provided for @record in @UFoliageInteractionComponent::FoliageActorsSet
	*/
	void ReplaceFoliageStaticMeshWithInteractiveFoliageActor(struct FIntractFoliageRecord record);
};
