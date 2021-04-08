// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateWall.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"
#include "Math/Box.h"
#include "Engine/TriggerVolume.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
//#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
URotateWall::URotateWall()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URotateWall::BeginPlay()
{
	Super::BeginPlay();

    FindPressurePlate();
    FindPressurePlateObject();
    InitialiseDoorValues();
    if(PressurePlateObject)
    {
        TargetMass = PressurePlateObject->FindComponentByClass<UPrimitiveComponent>()->GetMass();
    }
}

void URotateWall::InitialiseDoorValues()
{
    // Get start yaw value for calculating target yaw
    StartYaw = GetOwner()->GetActorRotation().Yaw;
    // Calculate target yaw for OpenDoor method
    TargetYaw = StartYaw + TargetYawIncrease;
    UE_LOG(LogTemp, Error, TEXT("TargetYaw: %f, StartYaw: %f, TargetYawIncrease: %f."), TargetYaw, StartYaw, TargetYawIncrease);
    CurrentRotation = GetOwner()->GetActorRotation();
    
    StartX = GetOwner()->GetActorLocation().X;
    TargetX = StartX - TargetXIncrease;
    CurrentLocation = GetOwner()->GetActorLocation();
}

void URotateWall::FindPressurePlate()
{
    if(PressurePlate == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no pressure plate set on OpenDoor!"), *GetOwner()->GetName());
        return;
    }
}

void URotateWall::FindPressurePlateObject()
{
    if(PressurePlateObject ==  nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component but with no pressure plate object set!"), *GetOwner()->GetName());
        return;
    }
}


// Called every frame
void URotateWall::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(ActorsAreOnPressurePlate()){
        CalculateMassOfOverlappingActors();
        if(Mass == TargetMass){
            if(TotalX != TargetXIncrease){
                //RotateWall(DeltaTime);
                SlideWall(DeltaTime);
                TimeLastOpened = GetWorld()->GetTimeSeconds();
            }
        }
    } else
    {
        if((GetWorld()->GetTimeSeconds() - TimeLastOpened) > DoorCloseDelay)
        {
            if(TotalX > 0.f){
                SlideWallBack(DeltaTime);
                //RotateWallBack(DeltaTime);
            }
        }
    }
}

bool URotateWall::ActorsAreOnPressurePlate()
{
    if(PressurePlate == nullptr){return false;}
    PressurePlate->GetOverlappingComponents(OUT OverlappingComponents);
    return OverlappingComponents.IsValidIndex(0);
}

void URotateWall::CalculateMassOfOverlappingActors()
{
    Mass = 0.f;
    for(UPrimitiveComponent* Component : OverlappingComponents)
    {
        Mass += Component->GetMass();
    }
}

void URotateWall::RotateWall(float DeltaTime)
{
    CurrentRotation = GetOwner()->GetActorRotation();
    CurrentYaw = FMath::Lerp(0.f, TargetYawIncrease, DeltaTime * MoveSpeed);
    CurrentRotation.Yaw += CurrentYaw;
    GetOwner()->SetActorRotation(CurrentRotation);
    TotalYaw += CurrentYaw;
    if(TotalYaw > TargetYawIncrease - 2.f)
    {
        TotalYaw = TargetYawIncrease;
        CurrentRotation.Yaw = TargetYaw;
        GetOwner()->SetActorRotation(CurrentRotation);
    }
    UE_LOG(LogTemp, Warning, TEXT("TotalYaw: %f"), TotalYaw);
}

void URotateWall::RotateWallBack(float DeltaTime)
{
    CurrentRotation = GetOwner()->GetActorRotation();
    CurrentYaw = FMath::Lerp(0.f, TargetYawIncrease, DeltaTime * MoveSpeed);
    CurrentRotation.Yaw -= CurrentYaw;
    GetOwner()->SetActorRotation(CurrentRotation);
    TotalYaw -= CurrentYaw;
    if(TotalYaw < 2.f)
    {
        TotalYaw = 0.f;
        CurrentRotation.Yaw = StartYaw;
        GetOwner()->SetActorRotation(CurrentRotation);
    }
}

void URotateWall::SlideWall(float DeltaTime)
{
    CurrentLocation = GetOwner()->GetActorLocation();
    XIncrease = FMath::Lerp(0.f, TargetXIncrease, DeltaTime * MoveSpeed);
    UE_LOG(LogTemp, Warning, TEXT("XIncrease: %f"), XIncrease);
    CurrentLocation.X -= XIncrease;
    GetOwner()->SetActorLocation(CurrentLocation);
    TotalX += XIncrease;
    UE_LOG(LogTemp, Warning, TEXT("TotalX: %f"), TotalX);
    if(TotalX > TargetXIncrease- 2.f)
    {
        TotalX = TargetXIncrease;
        CurrentLocation.X = TargetX;
        GetOwner()->SetActorLocation(CurrentLocation);
    }
}

void URotateWall::SlideWallBack(float DeltaTime)
{
    CurrentLocation = GetOwner()->GetActorLocation();
    XIncrease = FMath::Lerp(0.f, TargetXIncrease, DeltaTime * MoveSpeed);
    UE_LOG(LogTemp, Warning, TEXT("CurrentX: %f"), CurrentLocation.X);
    CurrentLocation.X += XIncrease;
    GetOwner()->SetActorLocation(CurrentLocation);
    TotalX -= XIncrease;
    UE_LOG(LogTemp, Warning, TEXT("TotalX: %f"), TotalX);
    if(TotalX < 2.f)
    {
        TotalX = 0.f;
        CurrentLocation.X = StartX;
        GetOwner()->SetActorLocation(CurrentLocation);
    }
}
