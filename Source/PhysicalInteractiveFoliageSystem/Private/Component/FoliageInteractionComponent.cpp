// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FoliageInteractionComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Actors/PhysicalInteractiveFoliageActor.h"
#include "Types/PIFS_Types.h"
#include "Kismet/kismetSystemLibrary.h"


// Sets default values for this component's properties
UFoliageInteractionComponent::UFoliageInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFoliageInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UFoliageInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFoliageInteractionComponent::ActivateFoliagesInRange(float ActivationRadius, bool bDebug)
{
	if (bDebug) {
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), ActivationRadius, 20, FColor::Red, 0, 2);
	}


	int32 counter = 0;
	TArray<FHitResult> hits;

	TArray<int32> indexes;

	TArray<FIntractFoliageRecord> IntractRecords;

	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), ActivationRadius, ETraceTypeQuery::TraceTypeQuery2, 0, {}, EDrawDebugTrace::None, hits, 1)) {

		for (const FHitResult h : hits) {
			if (UInstancedStaticMeshComponent* instComp = Cast<UInstancedStaticMeshComponent>(h.GetComponent())) {

				// track current peek of the array
				int32 PeekIndex = indexes.Num() - 1;
				// add unique index and track the index
				int32 uniqueIndex = indexes.AddUnique(h.Item);
				// check if it was a unique index !
				if (uniqueIndex > PeekIndex) {
					// store the index of the foliage with InstancedStaticMeshComponent in a record
					FIntractFoliageRecord record = FIntractFoliageRecord(instComp, h.Item);
					IntractRecords.Add(record);
				}
			}

		}

	}

	for (FIntractFoliageRecord& r : IntractRecords) {
		if (!r.FoliageComponent) continue;

		// debug
		if (bDebug) {
			DrawDebugPoint(GetWorld(), r.GetFoliageTransform().GetLocation() + FVector(0, 0, 120), 12, FColor::Red, 0, 5);
		}

		// replace 
		ReplaceFoliageStaticMeshWithInteractiveFoliageActor(r);


	}

	// debug
	if (bDebug) {
		UE_LOG(LogTemp, Warning, TEXT(" number of Unique foliages Intraction records %d"), IntractRecords.Num());
	}


}

void UFoliageInteractionComponent::ReplaceFoliageStaticMeshWithInteractiveFoliageActor(FIntractFoliageRecord record)
{
	if (FoliageActorsSet.IsEmpty()) return;
	for (FIntractFoliageSet foliage_actor : FoliageActorsSet) {
		if (foliage_actor.foliageStaticMesh == record.FoliageComponent->GetStaticMesh()) {

			if (APhysicalInteractiveFoliageActor* spawned = GetWorld()->SpawnActor<APhysicalInteractiveFoliageActor>(foliage_actor.foliageClass, record.GetFoliageTransform())) {
				spawned->InitSpawnedFoliage(record);

				// remove instance that we spawned this actor for
				if (record.FoliageComponent && record.FoliageComponent->IsValidInstance(record.FoliageIndex)) {
					record.FoliageComponent->RemoveInstance(record.FoliageIndex);
				}

			}
			else UE_LOG(LogTemp, Warning, TEXT("Failed to spawn foliage actor !"));


			return;
		}
	}

}
