// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunningInsideCharacter.h"
#include "Components/AudioComponent.h"
#include "SlowBlock.generated.h"

UCLASS()
class RUNNINGINSIDE_API ASlowBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlowBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ARunningInsideCharacter* RunningInsideCharacter;
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxCollision;

	//콜리전에 진입 했을 시 함수
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//콜리전에서 나왔을 시 함수
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* SlowSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
