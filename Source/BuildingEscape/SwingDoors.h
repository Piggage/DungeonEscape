// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "SwingDoors.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API USwingDoors : public UActorComponent
{
	GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USwingDoors();
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    void InitialiseDoorValues();
    void FindAudioComponent();
    void FindPressurePlate();
    void FindPressurePlateObject();
    bool ActorsAreOnPressurePlate();
    void CalculateMassOfOverlappingActors();
    void OpenDoor(float DeltaTime);
    void CloseDoor(float DeltaTime);
    void FindDoorComponent();
    
    FRotator CurrentRotationRight;
    float StartYawRight;
    float CurrentYawRight;
    float TargetYawRight;
    FRotator CurrentRotationLeft;
    float StartYawLeft;
    float CurrentYawLeft;
    float TargetYawLeft;
    float TargetMass;
    float Mass;
    float TimeLastOpened = 0.f;
    TArray<UPrimitiveComponent *> OverlappingComponents;
    bool bPlayDoorOpen = true;
    bool bPlayDoorClose = true;
    
    // Angle in degrees that the actor will rotate around the yaw (vertical plane).
    UPROPERTY(EditAnywhere)
    float TargetYawIncrease = 90.f;
    
    UPROPERTY(EditAnywhere)
    ATriggerVolume* PressurePlate = nullptr;
    
    UPROPERTY(EditAnywhere)
    AActor* PressurePlateObject = nullptr;
    
    // Delay in seconds between door opening and closing when player steps off trigger
    UPROPERTY(EditAnywhere)
    float DoorCloseDelay = 5.f;
    
    UPROPERTY()
    UAudioComponent* DoorSound = nullptr;
    
    UPROPERTY(EditAnywhere)
    float OpenSpeed = 1.f;
    
    UPROPERTY(EditAnywhere)
    float CloseSpeed = 0.5f;
    
    UPROPERTY()
    UStaticMeshComponent* RightDoor = nullptr;
    
    UPROPERTY()
    UStaticMeshComponent* LeftDoor = nullptr;
};
