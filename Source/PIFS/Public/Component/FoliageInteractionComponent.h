// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FoliageInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIFS_API UFoliageInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFoliageInteractionComponent();
	
	

	/*
		whether this component will Automatically activates surrounding foliages  or not
		if set to false, using ActivateFoliagesInRange() is a must to make foliages Interactive
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Settings")
	bool bAutoActivateFoliages = true;

	/*
		if @bAutoActivateFoliages is true, how offen ActivateFoliagesInRange() should be called to activate surrounding foliages
		the lower the acurate it is but it is also more performance heavy .
		( bigger @AutoActivateCheckRadius  required less @AutoActivateRate and vice versa)
		
		clamped to (min: 0.2 , max: 2.0)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Settings")
	float AutoActivateRate = 0.5;
	
	/*
		if @bAutoActivateFoliages is true, what should be the trace range (how far the foliage must be in order to activate it ? )
		( bigger @AutoActivateRate  required less @AutoActivateCheckRadius and vice versa)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Settings")
	float AutoActivateCheckRadius = 150.0;

	/*
		list of all interactive foliages
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Interactive Foliage Actors")
	TArray<struct FIntractFoliageSet> FoliageActorsSet;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
		check for any foliage static mesh around the actor
		and replaces them with proper Interactive foliage actor if provided
	*/
	UFUNCTION(BlueprintCallable, Category = "Interactive Foliage")
	void ActivateFoliagesInRange(float ActivationRadius = 30, bool bDebug = false);

	/*
		check if any blueprint provided for @record in @UFoliageInteractionComponent::FoliageActorsSet
	*/
	void ReplaceFoliageStaticMeshWithInteractiveFoliageActor(struct FIntractFoliageRecord record);
};
