#pragma once

#include "CoreMinimal.h" // 이미 UStaticMeshComponent를 가지고 있음
#include "RunningInsideCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "BoundBlock.generated.h"


UCLASS()
class RUNNINGINSIDE_API ABoundBlock : public AActor
{
	GENERATED_BODY()

public:

	ABoundBlock();

	ARunningInsideCharacter* RunningInsideCharacter;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* BoundSound;

	float fill_angry;
};