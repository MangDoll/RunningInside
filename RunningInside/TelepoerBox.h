#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "RunningInsideCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "TelepoerBox.generated.h"

UCLASS()
class RUNNINGINSIDE_API ATelepoerBox : public ATriggerBox
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

public:
	ATelepoerBox();

	UPROPERTY(VisibleAnyWhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "Teleporter")
		ATelepoerBox* otherTele;

	UFUNCTION()
		void EnterTeleporter(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitTeleporter(class AActor* overlappedActor, class AActor* otherActor);

	UPROPERTY()
		bool teleporting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* TeleportSound;

};