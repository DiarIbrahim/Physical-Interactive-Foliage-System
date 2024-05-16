// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Types/PIFS_Types.h"
#include "PhysicalInteractiveFoliageActor.generated.h"

UCLASS()
class PIFS_API APhysicalInteractiveFoliageActor : public AActor
{
	GENERATED_BODY()
	
protected:	
	// Sets default values for this actor's properties
	APhysicalInteractiveFoliageActor();
	

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Components")
	class USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Components")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage | Components")
	class USphereComponent* Collision;


	//// Settings

	/*
		from this bone and below all bones will be set to simulate physics during the activation and vice versa for the Deactivation
		if left blank, the root bone will be selected !
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage settings")
	FString CustomeRootBoneName;
	/*
		the time needed to blend out the physics simulation and deactivate the foliage.
		in seconds
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage settings")
	float DeactiveBlendOutTime = 4.0;
	/*
		the time to destroy the Interactive Foliage and replace it by instanced static mesh
		in seconds
		NOTE: if less than @DeactiveBlendOutTime , then DeactiveBlendOutTime  will be used.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage settings")
	float ReplacementTime = 5.0;




	// variables

	/*
		is a time in seconds used to check how long this actor is spawned and never been intracted by an actor
		after this time OnSpawnTimeEnded() will be called
	*/
	float SpawnedTimeCounter = -1;
	// whethere @SpawnedTimeCounter  is ended or not
	bool bSpawnTimeEnded = false;

	/*
		is a time that the this foliage actor been activated (not for spawn since actors may be spawned but never colide with player so we dont activate it right after spawn)
		after this time ended the foliage actor will be de activated

	*/
	float ActiveTimeConter = 0;
	/*
		whether the foliage actor is activated or deactivated
	*/
	bool bIsActivated = false;
	/*
		number of actors currently coliding with this foliage actor
	*/
	TArray<AActor*> actorsOverlapped;
	/*
		the data provided for this foliage actor
		contains : the instanced static mesh that this actor spawned to replace
	*/
	struct FIntractFoliageRecord FoliageRecord;

	/*
		manuly activates this actor and it will be physics interactiveS
	*/
	UFUNCTION(BlueprintCallable , Category = "Interactive Foliage")
	void ActivateFoliage();
	/*
		manulay deactivates this foliage
	*/
	UFUNCTION(BlueprintCallable, Category = "Interactive Foliage")
	void DeactivateFoliage();
	/*
		will be called when spawn time ends and player is not near this foliage
		so the actor will be destroy() and a new instance will be added as replace ment
	*/
	void OnSpawnTimeEnded();

	// called if @collision overlaps
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// called if @collision overlap ended

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/*
		this function is called when this foliage activated
		NOTE: for foliages with wide radius collision  will be bad choise to used this function for things like sound (not accurate)
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Foliage")
	void OnFoliageActivated();
	
	/*
		this function is called when this foliage deactivated
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Interactive Foliage")
	void OnFoliageDeactivated();


public:

	/*
		called after this actor spawned
		used to init some data like @FoliageRecord , @SpawnedTimeCounter
	*/
	void InitSpawnedFoliage(struct FIntractFoliageRecord record);



	// Called every frame

	virtual void Tick(float DeltaTime) override;

};
