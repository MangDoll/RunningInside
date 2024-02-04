// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowBlock.h"
#include "Components/BoxComponent.h" //Box컴포넌트 가져오기
#include "Engine//Engine.h" //로그 출력같은 기능 담당
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //게임관련 요소 접근 및 상호작용
#include "Components/StaticMeshComponent.h" //StaticMesh컴포넌트 가져오기
#include "GameFramework/Actor.h" //엑터 가져오기
#include "Components/AudioComponent.h"

// Sets default values
ASlowBlock::ASlowBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	BoxCollision->SetCollisionProfileName("Slow_Trigger");
	BoxCollision->SetupAttachment(Mesh);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASlowBlock::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ASlowBlock::OnOverlapEnd);


}

// Called when the game starts or when spawned
void ASlowBlock::BeginPlay()
{
	Super::BeginPlay();
	
	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void ASlowBlock::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		RunningInsideCharacter->SlowBlock();
		UGameplayStatics::PlaySoundAtLocation(this, SlowSound, GetActorLocation());
	}
}

void ASlowBlock::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void ASlowBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

