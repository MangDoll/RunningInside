#include "SpeedBlock.h"
#include "Components/BoxComponent.h" //Box������Ʈ ��������
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
#include "GameFramework/Actor.h" //���� ��������
#include "Components/AudioComponent.h"

// Sets default values
ASpeedBlock::ASpeedBlock()
{
	//�������ڸ��� ƽ �Լ� Ʈ��
	PrimaryActorTick.bCanEverTick = true;

	//�޽�������Ʈ ����
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//�ڽ� �ݸ��� ����
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//�ڽ� �ݸ��� ����
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	//�ڽ��ݸ��� �̸�
	CollisionBox->SetCollisionProfileName("Speed_Trigger");
	//��Ʈ�� ����
	CollisionBox->SetupAttachment(Mesh);


	//�ݸ��� �ڽ� �̺�Ʈ ����
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpeedBlock::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ASpeedBlock::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ASpeedBlock::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ASpeedBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//�������� �ӵ� ���� ����
void ASpeedBlock::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		RunningInsideCharacter->SpeedBlock();
		UGameplayStatics::PlaySoundAtLocation(this, SpeedSound, GetActorLocation());
	}
}

void ASpeedBlock::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{

}