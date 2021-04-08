// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "RotateWall.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API URotateWall : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URotateWall();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    void InitialiseDoorValues();
    void FindPressurePlate();
    void FindPressurePlateObject();
    bool ActorsAreOnPressurePlate();
    void CalculateMassOfOverlappingActors();
    void RotateWall(float DeltaTime);
    void RotateWallBack(float DeltaTime);
    void SlideWall(float DeltaTime);
    void SlideWallBack(float DeltaTime);

    FRotator CurrentRotation;
    float StartYaw;
    float CurrentYaw;
    float TargetYaw;
    float TotalYaw = 0.f;
    FVector CurrentLocation;
    float StartX;
    float XIncrease;
    float TargetX;
    float TotalX = 0.f;
    float TargetMass;
    float Mass;
    float TimeLastOpened = 0.f;
    TArray<UPrimitiveComponent *> OverlappingComponents;
    
    UPROPERTY(EditAnywhere)
    float MoveSpeed = 1.f;
    
    UPROPERTY(EditAnywhere)
    float TargetYawIncrease = 180.f;
    
    UPROPERTY(EditAnywhere)
    float TargetXIncrease = 300.f;
        
    // Delay in seconds between door opening and closing when player steps off trigger
    UPROPERTY(EditAnywhere)
    float DoorCloseDelay = 5.f;
    
    UPROPERTY(EditAnywhere)
    ATriggerVolume* PressurePlate = nullptr;
    
    UPROPERTY(EditAnywhere)
    AActor* PressurePlateObject = nullptr;

};
