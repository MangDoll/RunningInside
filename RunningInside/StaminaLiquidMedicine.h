#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunningInsideCharacter.h"
#include "Components/AudioComponent.h"
#include "StaminaLiquidMedicine.generated.h"

UCLASS()
class RUNNINGINSIDE_API AStaminaLiquidMedicine : public AActor
{
	GENERATED_BODY()

public:
	AStaminaLiquidMedicine();
	ARunningInsideCharacter* RunningInsideCharacter;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
		class UBoxComponent* BoxCollision;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* ItemSound;


};