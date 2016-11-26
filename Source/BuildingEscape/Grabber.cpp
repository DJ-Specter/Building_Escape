// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingEscape.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();

}


void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed"))
		// LINE TRACE and reach any actors with phyysics body collision channel set
		auto HitResult = GetFirstPhysicsBodyInReach();
		auto ComponentToGrab = HitResult.GetComponent();
		auto ActorHit = HitResult.GetActor();


		//If we hit something then attach a physics handle 
		if (ActorHit)
		{
			PhysicsHandle->GrabComponent(
				ComponentToGrab,
				NAME_None,
				ComponentToGrab->GetOwner()->GetActorLocation(),
				true // allow rotation 
			);
		}
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Release Pressed"))
		// TODO release physics handle 
		PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	///Look for attached Physics Handle 
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		//Physics handle is found

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());

	}
}

void UGrabber::SetupInputComponent()
{
	///Look for attached Input Component (Only appears at run time) 

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		//Input Component is found
		UE_LOG(LogTemp, Warning, TEXT("%s input component found!"), *GetOwner()->GetName());
		/// Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());

	}
}


// Called every frame
void UGrabber::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );


	// Get player viewpoint this tick
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Logging Location: %s and Rotation: %s "), *PlayerViewpointLocation.ToString(), *PlayerViewpointRotation.ToString())

	FVector LineTraceDirection = PlayerViewpointRotation.Vector() * Reach;
	FVector LineTraceEnd = PlayerViewpointLocation + LineTraceDirection;

	// If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		//move the object that we're holding 
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
		

	
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// Get player viewpoint this tick
	FVector PlayerViewpointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewpointLocation,
		OUT PlayerViewpointRotation
	);

	//UE_LOG(LogTemp, Warning, TEXT("Logging Location: %s and Rotation: %s "), *PlayerViewpointLocation.ToString(), *PlayerViewpointRotation.ToString())

	FVector LineTraceDirection = PlayerViewpointRotation.Vector() * Reach;
	FVector LineTraceEnd = PlayerViewpointLocation + LineTraceDirection;


	DrawDebugLine(
		GetWorld(),
		PlayerViewpointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f
	);

	/// Setup query parameters 
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// Ray-cast out to reach distance 

	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewpointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	// See what he hit
	AActor *HitPointer = Hit.GetActor();

	if (HitPointer)
	{
		UE_LOG(LogTemp, Error, TEXT("Found object: %s "), *HitPointer->GetName());
	}
	return Hit;
}
