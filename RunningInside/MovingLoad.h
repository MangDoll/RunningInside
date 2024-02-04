// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunningInsideCharacter.h"
#include "Components/AudioComponent.h"
#include "MovingLoad.generated.h"

UCLASS()
class RUNNINGINSIDE_API AMovingLoad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingLoad();
	ARunningInsideCharacter* RunningInsideCharacter;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void MovingPlatform();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		float moveDistance;

	UPROPERTY(EditAnywhere)
		float MoveSpeed;

	UPROPERTY(EditAnywhere)
		//��� �ִ� �Ÿ�
		float PlusLimitedDistance;

	UPROPERTY(EditAnywhere)
		//���� �ִ� �Ÿ�
		float MinusLimitedDistance;

	FVector InitialLocation;

	bool bMovingRight = true;

	//���� �� - �ʱ� �� �Ÿ�
	float Distance;
};
