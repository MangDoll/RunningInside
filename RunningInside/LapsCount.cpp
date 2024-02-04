#include "LapsCount.h"
#include "Components/BoxComponent.h" //Box������Ʈ ��������
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
#include "GameFramework/Actor.h" //���� ��������
#include "Components/AudioComponent.h"

// Sets default values
ALapsCount::ALapsCount()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�޽�������Ʈ ����
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//�ڽ� �ݸ��� ����
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//�ڽ� �ݸ��� ����
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	//�ڽ��ݸ��� �̸�
	CollisionBox->SetCollisionProfileName("Count_Trigger");
	//��Ʈ�� ����
	CollisionBox->SetupAttachment(Mesh);

	//�ݸ��� �ڽ� �̺�Ʈ ����
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALapsCount::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ALapsCount::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ALapsCount::BeginPlay()
{
	Super::BeginPlay();
	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ALapsCount::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALapsCount::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (RunningInsideCharacter)
	{
		RunningInsideCharacter->LapsCountVal++;
	}
}

void ALapsCount::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	Destroy();
}
