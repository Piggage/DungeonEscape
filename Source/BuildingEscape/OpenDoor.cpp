// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
    
    InitialiseDoorValues();
    
    FindAudioComponent();
    
    FindPressurePlate();
        
    // Protect against nullptr
    FindPressurePlateObject();
    
    TargetMass = PressurePlateObject->FindComponentByClass<UPrimitiveComponent>()->GetMass();
    UE_LOG(LogTemp, Warning, TEXT("TargetMass: %f"), TargetMass);
    

}

void UOpenDoor::InitialiseDoorValues()
{
    // Get start yaw value for calculating target yaw
    StartYaw = GetOwner()->GetActorRotation().Yaw;
    // Calculate target yaw for OpenDoor method
    TargetYaw = StartYaw + TargetYawIncrease;
}

void UOpenDoor::FindAudioComponent()
{
    AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

    if(AudioComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no Audio Component set on OpenDoor!"), *GetOwner()->GetName());
        return;
    }
}

void UOpenDoor::FindPressurePlate()
{
    if(PressurePlate == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no pressure plate set on OpenDoor!"), *GetOwner()->GetName());
        return;
    }
}

void UOpenDoor::FindPressurePlateObject()
{
    if(PressurePlateObject ==  nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component but with no pressure plate object set!"), *GetOwner()->GetName());
        return;
    }
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Check if Actors are overlapping the pressure plate
    if(ActorsAreOnPressurePlate()){
        CalculateMassOfOverlappingActors();
        if(Mass == TargetMass){
            // Open Door to the target yaw
            OpenDoor(DeltaTime);
            // Update time last opened for the timer below
            TimeLastOpened = GetWorld()->GetTimeSeconds();
            Mass = 0;
        }
    }
    if(CurrentYaw != StartYaw)
    {
        // Wait for DoorCloseDelay seconds after the pressure plate lifts
        if((GetWorld()->GetTimeSeconds() - TimeLastOpened) > DoorCloseDelay)
        {
            // Close door to the start yaw
            CloseDoor(DeltaTime);
            UE_LOG(LogTemp, Warning, TEXT("StartYaw: %f. CurrentYaw: %f."), StartYaw, CurrentYaw);

        }
    }
}

bool UOpenDoor::ActorsAreOnPressurePlate()
{
    if(PressurePlate == nullptr){return false;}
    PressurePlate->GetOverlappingComponents(OUT OverlappingComponents);
    // Protects against nullptr array if no overlapping components
    if(OverlappingComponents.IsValidIndex(0))
    {
        return true;
    }else
    {
        return false;
    }
}

void UOpenDoor::CalculateMassOfOverlappingActors()
{
    Mass = 0.f;
    for(UPrimitiveComponent* Component : OverlappingComponents)
    {
        Mass += Component->GetMass();
    }
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
    // Get Current Rotation
    CurrentRotation = GetOwner()->GetActorRotation();
    
    if(bPlayDoorOpen && CurrentRotation.Yaw == StartYaw)
    {
        if(AudioComponent == nullptr){return;}
        bPlayDoorOpen = false;
        bPlayDoorClose = true;
        AudioComponent->Play();
    }
    
    // Calculate the interpolated value of the yaw
    CurrentYaw = FMath::Lerp(CurrentRotation.Yaw, TargetYaw, DeltaTime * OpenSpeed);
    //Apply the new Yaw to the current rotation
    CurrentRotation.Yaw = CurrentYaw;
    GetOwner()->SetActorRotation(CurrentRotation);
   

}

void UOpenDoor::CloseDoor(float DeltaTime)
{
    // Get Current Rotation
    CurrentRotation = GetOwner()->GetActorRotation();
    // Calculate the interpolated value of the yaw
    CurrentYaw = FMath::Lerp(CurrentRotation.Yaw, StartYaw, DeltaTime * CloseSpeed);
    //Apply the new Yaw to the current rotation
    CurrentRotation.Yaw = CurrentYaw;
    GetOwner()->SetActorRotation(CurrentRotation);
    if(bPlayDoorClose && CurrentYaw < StartYaw + 10.f){
        CurrentRotation.Yaw = StartYaw;
        GetOwner()->SetActorRotation(CurrentRotation);
        if(AudioComponent == nullptr){return;}
        bPlayDoorClose = false;
        bPlayDoorOpen = true;
        AudioComponent->Play();

    }

}

