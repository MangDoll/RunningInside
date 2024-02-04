#include "SuperJumpBlock.h"
#include "Components/BoxComponent.h" //Box������Ʈ ��������
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
#include "GameFramework/Actor.h" //���� ��������
#include "Components/AudioComponent.h"


ASuperJumpBlock::ASuperJumpBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	CollisionBox->SetCollisionProfileName("Bound_Trigger");
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASuperJumpBlock::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ASuperJumpBlock::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ASuperJumpBlock::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ASuperJumpBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASuperJumpBlock::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		float LaunchForceZ = 4000.0f;
		float LaunchForceX = 100.0f;

		FVector LaunchVelocity = FVector(LaunchForceX, 0 , LaunchForceZ);

		RunningInsideCharacter->LaunchCharacter(LaunchVelocity, false, false);
		
		UGameplayStatics::PlaySoundAtLocation(this, SuperJumpSound, GetActorLocation());

		if (RunningInsideCharacter->HeigthThreshHold > RunningInsideCharacter->FallingStartDash_Height)
		{
			RunningInsideCharacter->StartDash();
		}
	}	
}

void ASuperJumpBlock::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

