#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunningInsideCharacter.h"
#include "Components/AudioComponent.h"
#include "SpeedBlock.generated.h"


UCLASS()
class RUNNINGINSIDE_API ASpeedBlock : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpeedBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//캐릭터 포인터 변수 선언
	ARunningInsideCharacter* RunningInsideCharacter;

	//메쉬 컴포넌트 설정
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	//BOX컴포넌트의 콜리전 보이게 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;



	//콜리전에 진입 했을 시 함수
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//콜리전에서 나왔을 시 함수
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* SpeedSound;

};