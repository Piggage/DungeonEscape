// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
//#include "Engine/World.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
    
    UPROPERTY(EditAnywhere)
    float Reach = 200.f;
    
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    FHitResult Hit;
    FVector LineTraceEnd;
    UPROPERTY()
    UPhysicsHandleComponent* PhysicsHandle = nullptr;
    UPROPERTY()
    UInputComponent* InputComponent = nullptr;
    
    void Grab();
    void Release();
    void FindPhysicsHandle();
    void BindInputActions();
    void UpdateViewPointVectors();
    bool DoesRayHitRightObject();
    FVector GetOrigin();
};
