// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLine.h"
#include "Components/BoxComponent.h" //Box������Ʈ ��������
#include "Engine//Engine.h" //�α� ��°��� ��� ���
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //���Ӱ��� ��� ���� �� ��ȣ�ۿ�
#include "Components/StaticMeshComponent.h" //StaticMesh������Ʈ ��������
#include "GameFramework/Actor.h" //���� ��������
#include "Components/AudioComponent.h"

// Sets default values
ALastLine::ALastLine()
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
	CollisionBox->SetCollisionProfileName("Record_Trigger");
	//��Ʈ�� ����
	CollisionBox->SetupAttachment(Mesh);


	//�ݸ��� �ڽ� �̺�Ʈ ����
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALastLine::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ALastLine::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ALastLine::BeginPlay()
{
	Super::BeginPlay();
	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ALastLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALastLine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		Record_hour = RunningInsideCharacter->Hours;
		Record_min = RunningInsideCharacter->Minutes;
		Record_sec = RunningInsideCharacter->Seconds;
	}
}

void ALastLine::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

