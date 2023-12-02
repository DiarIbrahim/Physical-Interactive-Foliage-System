

#include "Actors/PhysicalInteractiveFoliageActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Component/FoliageInteractionComponent.h"



// Sets default values
APhysicalInteractiveFoliageActor::APhysicalInteractiveFoliageActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	if (!root) {
		root = CreateDefaultSubobject<USceneComponent>(FName("rootcomponent"));
		SetRootComponent(root);

	}

	if (!mesh) {
		mesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh component"));
		mesh->SetSimulatePhysics(false);
		mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		mesh->SetCollisionObjectType(ECC_PhysicsBody);
		mesh->SetCollisionResponseToAllChannels(ECR_Block);
		mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		mesh->SetupAttachment(root);
	}

	if (!collision) {
		collision = CreateDefaultSubobject<USphereComponent>(FName("Sphere Component"));
		collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		collision->SetRelativeLocation(FVector(0, 0, 50));
		collision->SetSphereRadius(100);
		collision->SetupAttachment(root);
	}

}

// Called when the game starts or when spawned
void APhysicalInteractiveFoliageActor::BeginPlay()
{
	Super::BeginPlay();


	// set the bone name  to root bone name if CustomeRootBoneName left blank
	if (CustomeRootBoneName.IsEmpty() && mesh) {
		CustomeRootBoneName = mesh->GetBoneName(0).ToString();
	}

	if (ReplacementTime < DeactiveBlendOutTime) {
		ReplacementTime = DeactiveBlendOutTime;
	}

	if (collision) {
		collision->OnComponentBeginOverlap.AddDynamic(this, &APhysicalInteractiveFoliageActor::OnOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &APhysicalInteractiveFoliageActor::OnOverlapEnd);
	}

	DeactivateFoliage();
}

void APhysicalInteractiveFoliageActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetComponentByClass<UFoliageInteractionComponent>()) {
		actorsOverlapped.AddUnique(OtherActor);
		ActivateFoliage();

	}
}

void APhysicalInteractiveFoliageActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	actorsOverlapped.Remove(OtherActor);
}

void APhysicalInteractiveFoliageActor::InitSpawnedFoliage(FIntractFoliageRecord record)
{
	FoliageRecord = record;
	SpawnedTimeCounter = ReplacementTime;

}


void APhysicalInteractiveFoliageActor::ActivateFoliage()
{
	mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	mesh->SetCollisionProfileName("Custome");
	mesh->SetCollisionObjectType(ECC_PhysicsBody);
	mesh->SetCollisionResponseToAllChannels(ECR_Block);
	mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	mesh->SetAllBodiesBelowSimulatePhysics(FName(CustomeRootBoneName), true, 0);
	ActiveTimeConter = DeactiveBlendOutTime;
	bIsActivated = true;
	
	// call bp function
	OnFoliageActivated();
}

void APhysicalInteractiveFoliageActor::DeactivateFoliage()
{
	mesh->SetAllBodiesBelowSimulatePhysics(FName(CustomeRootBoneName), false, 0);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsActivated = false;

	// call on bp
	OnFoliageDeactivated();

}


void APhysicalInteractiveFoliageActor::OnSpawnTimeEnded()
{

	if (FoliageRecord.FoliageComponent) {

		// add instance to the component !
		FoliageRecord.FoliageComponent->AddInstance(GetActorTransform(), 1);


		// destroy this
		Destroy();

	}



}



// Called every frame
void APhysicalInteractiveFoliageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTimeConter <= 0 && bIsActivated && actorsOverlapped.IsEmpty()) {
		// deactivate
		DeactivateFoliage();
	}
	else if (bIsActivated && actorsOverlapped.IsEmpty()) {
		ActiveTimeConter -= DeltaTime;
		mesh->SetAllBodiesBelowPhysicsBlendWeight(FName(CustomeRootBoneName), ActiveTimeConter / DeactiveBlendOutTime, 0, 0);
	}


	if (!bIsActivated && actorsOverlapped.IsEmpty() && SpawnedTimeCounter == 0.0) {
		if (!bSpawnTimeEnded) {
			bSpawnTimeEnded = true;
			OnSpawnTimeEnded();
		}
	}
	else if (!bIsActivated && actorsOverlapped.IsEmpty()) {
		SpawnedTimeCounter -= DeltaTime;
		SpawnedTimeCounter = FMath::Clamp(SpawnedTimeCounter, 0.0, 15.0);
	}
}

