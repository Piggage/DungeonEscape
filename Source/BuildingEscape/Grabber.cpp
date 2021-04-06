// Fill out your copyright notice in the Description page of Project Settings.
#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"


#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
    // Debug function, protecting against null pointer
    FindPhysicsHandle();
    
    // Bind grab and release actions to the input component
    BindInputActions();
}

void UGrabber::FindPhysicsHandle()
{
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if(PhysicsHandle == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("No Physics Handle for %s"), *GetOwner()->GetName());
    }
}

void UGrabber::BindInputActions()
{
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    // Protect against input component nullptr
    if(InputComponent)
    {
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
    }else
    {
        UE_LOG(LogTemp, Error, TEXT("No InputComponent for %s"), *GetOwner()->GetName());
    }
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // If the physics handle has grabbed something, move the object. Also protects against nullptr PhysicsHandle
    if(!PhysicsHandle){return;}
    if(PhysicsHandle->GrabbedComponent)
    {
        UpdateViewPointVectors();
        PhysicsHandle->SetTargetLocation(LineTraceEnd);
    }
}

// Update location, rotation and ray tracing end
void UGrabber::UpdateViewPointVectors()
{
    // Get Player Location and Rotation for raytracing
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
       OUT PlayerViewPointLocation,
       OUT PlayerViewPointRotation);
    // Calculate vector end point for raytracing
    LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

// Called when Inut Component called by mouse/keyboard/controller
void UGrabber::Grab()
{
    // Check if the ray hits a qualified object and updates the FHitResult Hit
    if(DoesRayHitRightObject())
    {
        FVector Origin = GetOrigin();
        UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();
        if(!PhysicsHandle){return;}
        PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, Origin);
    }
}

//returns true if the right object is hit and updates the HitResult
bool UGrabber::DoesRayHitRightObject()
{
    FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
    UpdateViewPointVectors();
    // Checks if the ray hits a physics body with out parameter Hit giving the object information
    if  (
       GetWorld()->LineTraceSingleByObjectType(
       OUT Hit,
       PlayerViewPointLocation,
       LineTraceEnd,
       FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
       TraceParams
                                               )
         )
    {
        return true;
    } else
    {
        return false;
    }
}

FVector UGrabber::GetOrigin()
{
    FVector BoxExtent;
    FVector Origin;
    Hit.GetActor()->GetActorBounds(false, Origin, BoxExtent, false);
    return Origin;
}

// Called when Inut Component released by mouse/keyboard/controller
void UGrabber::Release()
{
    PhysicsHandle->ReleaseComponent();
}


