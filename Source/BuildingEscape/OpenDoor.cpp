// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "OpenDoor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	//Find the owning actor
	Owner = GetOwner();
	GetTotalMassOfActorsOnPlate();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Owner) {
		UE_LOG(LogTemp, Error, TEXT("%s missing Owner"), *GetOwner()->GetName());
		return;
	}
	//Poll the Trigger Volume
	if (GetTotalMassOfActorsOnPlate() > TriggerMass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Trigger Activated"));

		OnOpen.Broadcast();
	}

	//Check if it's time to close the door
	else 	{
		OnClose.Broadcast();
	}
}
float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;
	if (!PressurePlate) { 
		UE_LOG(LogTemp, Error, TEXT("%s missing PressurePlate"), *GetOwner()->GetName());
		return TotalMass;
	}	
	// Find all the overlapping Actors
	TArray<AActor*> OverlappingActors;
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Iterate through them adding their masses 

	for (const auto& Act : OverlappingActors)
	{
		
		TotalMass += Act->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("Found %s"), *Act->GetName());

	}

	return TotalMass;
}
