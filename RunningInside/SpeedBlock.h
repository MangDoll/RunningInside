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

	//ĳ���� ������ ���� ����
	ARunningInsideCharacter* RunningInsideCharacter;

	//�޽� ������Ʈ ����
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	//BOX������Ʈ�� �ݸ��� ���̰� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox;



	//�ݸ����� ���� ���� �� �Լ�
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//�ݸ������� ������ �� �Լ�
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* SpeedSound;

};