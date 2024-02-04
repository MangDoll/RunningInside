#include "BoundBlock.h"
#include "Components/BoxComponent.h" //Box������Ʈ ��������
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
#include "GameFramework/Actor.h" //���� ��������
#include "Components/AudioComponent.h"


ABoundBlock::ABoundBlock()
{

	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	CollisionBox->SetCollisionProfileName("Bound_Trigger");
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABoundBlock::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABoundBlock::OnOverlapEnd);

	fill_angry = 5.0f;
}


void ABoundBlock::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}


void ABoundBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoundBlock::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		if (!RunningInsideCharacter->IsPowerful)
		{
			float LaunchForce = 1500.0f;

			FVector LaunchDirection = RunningInsideCharacter->GetActorLocation() - GetActorLocation();
			LaunchDirection.Normalize();

			FVector LaunchVelocity = LaunchDirection * LaunchForce;
			LaunchVelocity.Z = LaunchForce * 0.7f;

			RunningInsideCharacter->LaunchCharacter(LaunchVelocity, true, true);

			RunningInsideCharacter->AngryGage += fill_angry;

			UGameplayStatics::PlaySoundAtLocation(this, BoundSound, GetActorLocation());

			if (RunningInsideCharacter->HeigthThreshHold > RunningInsideCharacter->BoundStartDash_Height)
			{
				RunningInsideCharacter->BoundDash();
			}
		}		
	}
}

void ABoundBlock::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}