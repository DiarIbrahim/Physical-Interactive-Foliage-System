

#include "Actors/PhysicalInteractiveFoliageActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Component/FoliageInteractionComponent.h"
#include "Components/SkeletalMeshComponent.h"



// Sets default values
APhysicalInteractiveFoliageActor::APhysicalInteractiveFoliageActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	if (!Root) {
		Root = CreateDefaultSubobject<USceneComponent>(FName("Root component"));
		SetRootComponent(Root);

	}

	if (!Mesh) {
		Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh component"));
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionResponseToAllChannels(ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetupAttachment(Root);
	}

	if (!Collision) {
		Collision = CreateDefaultSubobject<USphereComponent>(FName("Sphere Component"));
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Collision->SetRelativeLocation(FVector(0, 0, 50));
		Collision->SetSphereRadius(100);
		Collision->SetupAttachment(Root);
	}

}

// Called when the game starts or when spawned
void APhysicalInteractiveFoliageActor::BeginPlay()
{
	Super::BeginPlay();


	// set the bone name  to root bone name if CustomeRootBoneName left blank
	if (CustomeRootBoneName.IsEmpty() && Mesh) {
		CustomeRootBoneName = Mesh->GetBoneName(0).ToString();
	}

	if (ReplacementTime < DeactiveBlendOutTime) {
		ReplacementTime = DeactiveBlendOutTime;
	}

	if (Collision) {
		Collision->OnComponentBeginOverlap.AddDynamic(this, &APhysicalInteractiveFoliageActor::OnOverlap);
		Collision->OnComponentEndOverlap.AddDynamic(this, &APhysicalInteractiveFoliageActor::OnOverlapEnd);
	}
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
	Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Mesh->SetCollisionProfileName("Custom");
	Mesh->SetCollisionObjectType(ECC_PhysicsBody);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Mesh->SetAllBodiesBelowSimulatePhysics(FName(CustomeRootBoneName), true, 0);
	ActiveTimeConter = DeactiveBlendOutTime;
	bIsActivated = true;
	
	// call bp function
	OnFoliageActivated();
}

void APhysicalInteractiveFoliageActor::DeactivateFoliage()
{
	Mesh->SetAllBodiesBelowSimulatePhysics(FName(CustomeRootBoneName), false, 0);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		Mesh->SetAllBodiesBelowPhysicsBlendWeight(FName(CustomeRootBoneName), ActiveTimeConter / DeactiveBlendOutTime, 0, 0);
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

