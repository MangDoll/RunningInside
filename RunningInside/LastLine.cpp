// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLine.h"
#include "Components/BoxComponent.h" //Box컴포넌트 가져오기
#include "Engine//Engine.h" //로그 출력같은 기능 담당
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //게임관련 요소 접근 및 상호작용
#include "Components/StaticMeshComponent.h" //StaticMesh컴포넌트 가져오기
#include "GameFramework/Actor.h" //엑터 가져오기
#include "Components/AudioComponent.h"

// Sets default values
ALastLine::ALastLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//메쉬컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//박스 콜리전 생성
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//박스 콜리전 범위
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	//박스콜리전 이름
	CollisionBox->SetCollisionProfileName("Record_Trigger");
	//루트에 붙임
	CollisionBox->SetupAttachment(Mesh);


	//콜리전 박스 이벤트 생성
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

