// Fill out your copyright notice in the Description page of Project Settings.


#include "SwingDoors.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
USwingDoors::USwingDoors()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USwingDoors::BeginPlay()
{
    Super::BeginPlay();
    
    FindDoorComponent();

    InitialiseDoorValues();
    
    FindAudioComponent();

    FindPressurePlate();

    FindPressurePlateObject();
    
    TargetMass = PressurePlateObject->FindComponentByClass<UPrimitiveComponent>()->GetMass();
    UE_LOG(LogTemp, Warning, TEXT("TargetMass: %f"), TargetMass);
}

void USwingDoors::InitialiseDoorValues()
{
    // Get start yaw value for calculating target yaw
    StartYawRight = GetOwner()->GetActorRotation().Yaw;
    StartYawLeft = GetOwner()->GetActorRotation().Yaw - 180.f;
    // Calculate target yaw for OpenDoor method
    TargetYawRight = StartYawRight + TargetYawIncrease;
    TargetYawLeft = StartYawLeft - TargetYawIncrease;
}

void USwingDoors::FindAudioComponent()
{
    DoorSound = GetOwner()->FindComponentByClass<UAudioComponent>();

    if(DoorSound == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no Audio Component set on OpenDoor!"), *GetOwner()->GetName());
        return;
    }
}

void USwingDoors::FindPressurePlate()
{
    if(PressurePlate == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no pressure plate set on OpenDoor!"), *GetOwner()->GetName());
        return;
    }
}

void USwingDoors::FindPressurePlateObject()
{
    if(PressurePlateObject ==  nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component but with no pressure plate object set!"), *GetOwner()->GetName());
        return;
    }
}

void USwingDoors::FindDoorComponent()
{
    TArray<UStaticMeshComponent*> children;
    GetOwner()->GetComponents(children);
    for (int32 i = 0; i < children.Num(); i++)
    {
        if (children[i])
        {
            UStaticMeshComponent* child = children[i];
            FString name = child->GetName();
            if (child->GetName() == "SM_DoorWay_Large_Door_Left")
            {
                LeftDoor = child;
            } else if (child->GetName() == "SM_DoorWay_Large_Door_Right")
            {
                RightDoor = child;
            }
        }
    }
}


// Called every frame
void USwingDoors::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Check if Actors are overlapping the pressure plate
    if(ActorsAreOnPressurePlate()){
        UE_LOG(LogTemp, Warning, TEXT("Actor on pressureplate..."));
        CalculateMassOfOverlappingActors();
        if(Mass == TargetMass){
            OpenDoor(DeltaTime);
            // Update time last opened for the timer below
            TimeLastOpened = GetWorld()->GetTimeSeconds();
        }
    }else{
        UE_LOG(LogTemp, Warning, TEXT("Actor not on pressureplate..."));
    }
    // If door is open, wait delay until closing
    if(CurrentYawRight != StartYawRight)
    {
        // Wait for DoorCloseDelay seconds after the pressure plate lifts
        if((GetWorld()->GetTimeSeconds() - TimeLastOpened) > DoorCloseDelay)
        {
            // Close door to the start yaw
            CloseDoor(DeltaTime);
            UE_LOG(LogTemp, Warning, TEXT("StartYawRight: %f. CurrentYawRight: %f."), StartYawRight, CurrentYawRight);
        }
    }
}

bool USwingDoors::ActorsAreOnPressurePlate()
{
    if(PressurePlate == nullptr){return false;}
    PressurePlate->GetOverlappingComponents(OUT OverlappingComponents);
    return OverlappingComponents.IsValidIndex(0);
}

void USwingDoors::CalculateMassOfOverlappingActors()
{
    Mass = 0.f;
    for(UPrimitiveComponent* Component : OverlappingComponents)
    {
        Mass += Component->GetMass();
    }
}

void USwingDoors::OpenDoor(float DeltaTime)
{
    // CurrentRotationRight = GetOwner()->GetActorRotation();
    // CurrentRotationLeft = LeftDoor->GetActorRotation();

    // Binary switch for playing door sound and door must be closed
    if(bPlayDoorOpen && CurrentRotationRight.Yaw == StartYawRight)
    {
        if(DoorSound == nullptr){return;}
        bPlayDoorOpen = false;
        bPlayDoorClose = true;
        DoorSound->Play();
    }
    // Calculate the interpolated value of the yaw
    CurrentYawRight = FMath::Lerp(StartYawRight, TargetYawRight, DeltaTime * OpenSpeed);
    CurrentYawLeft = FMath::Lerp(StartYawLeft, TargetYawLeft, DeltaTime * OpenSpeed);
    //Apply the new Yaw to the current rotation
    CurrentRotationRight.Yaw = CurrentYawRight;
    CurrentRotationLeft.Yaw = CurrentYawLeft;
    UE_LOG(LogTemp, Warning, TEXT("Current Yaw Right: %f, Target Yaw Right: "), CurrentRotationRight.Yaw, TargetYawRight);
    //RightDoor->SetActorRotation(CurrentRotationRight);
    RightDoor->AddRelativeRotation(CurrentRotationRight);
    //LeftDoor->SetActorRotation(CurrentRotationLeft);
    LeftDoor->AddRelativeRotation(CurrentRotationLeft);
    UE_LOG(LogTemp, Warning, TEXT("Opening Door..."));
}

void USwingDoors::CloseDoor(float DeltaTime)
{
    // CurrentRotationRight = RightDoor->GetActorRotation();
    // CurrentRotationLeft = LeftDoor->GetActorRotation();
    // Calculate the interpolated value of the yaw
    CurrentYawRight = FMath::Lerp(TargetYawRight, StartYawRight, DeltaTime * CloseSpeed);
    CurrentYawLeft = FMath::Lerp(TargetYawLeft, StartYawLeft, DeltaTime * CloseSpeed);
    //Apply the new Yaw to the current rotation
    CurrentRotationRight.Yaw = CurrentYawRight;
    CurrentRotationLeft.Yaw = CurrentYawLeft;

    // RightDoor->SetActorRotation(CurrentRotationRight);
    RightDoor->AddRelativeRotation(CurrentRotationRight);
    // LeftDoor->SetActorRotation(CurrentRotationLeft);
    LeftDoor->AddRelativeRotation(CurrentRotationLeft);
    /*
     Binary switch for playing door sound and door must be closed.
     Exponential movement towards close, so slams door if it gets within 10 degrees.
     */
    if(bPlayDoorClose /*&& CurrentYawRight < StartYawRight + 10.f*/){
        // CurrentRotationRight.Yaw = StartYawRight;
        // CurrentRotationLeft.Yaw = StartYawLeft;
        // RightDoor->AddRelativeRotation(CurrentRotationRight);
        // LeftDoor->AddRelativeRotation(CurrentRotationLeft);
        if(DoorSound == nullptr){return;}
        bPlayDoorClose = false;
        bPlayDoorOpen = true;
        DoorSound->Play();
    }
}


