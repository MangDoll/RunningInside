#include "TelepoerBox.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine//Engine.h" //로그 출력같은 기능 담당
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //게임관련 요소 접근 및 상호작용
#include "Components/StaticMeshComponent.h" //StaticMesh컴포넌트 가져오기
#include "Components/AudioComponent.h"


ATelepoerBox::ATelepoerBox()
{
	otherTele = this;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	Mesh->SetupAttachment(RootComponent);
	Mesh->SetVisibility(true);

	OnActorBeginOverlap.AddDynamic(this, &ATelepoerBox::EnterTeleporter);
	OnActorEndOverlap.AddDynamic(this, &ATelepoerBox::ExitTeleporter);
	teleporting = false;

}


void ATelepoerBox::BeginPlay()
{
	Super::BeginPlay();

}


void ATelepoerBox::EnterTeleporter(class AActor* overlappedActor, class AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (otherTele)
		{
			ARunningInsideCharacter* character = Cast<ARunningInsideCharacter>(otherActor);
			if (character && !otherTele->teleporting)
			{
				teleporting = true;
				character->SetActorRotation(otherTele->GetActorRotation());
				character->GetController()->SetControlRotation(character->GetActorRotation());
				character->SetActorLocation(otherTele->GetActorLocation());
				UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, GetActorLocation());
			}
		}
	}

}

void ATelepoerBox::ExitTeleporter(class AActor* overlappedActor, class AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (otherTele && otherActor != this)
		{
			otherTele->teleporting = false;
			UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, GetActorLocation());
		}
	}
}