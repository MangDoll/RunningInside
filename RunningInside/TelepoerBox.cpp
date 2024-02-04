#include "TelepoerBox.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
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