// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/PIFS_Types.h"

#include "components/InstancedStaticMeshComponent.h"

FTransform FIntractFoliageRecord::GetFoliageTransform() const
{
    if (!FoliageComponent) return FTransform();

    FTransform transform;
    FoliageComponent->GetInstanceTransform(FoliageIndex, transform, true);
    return transform;

}