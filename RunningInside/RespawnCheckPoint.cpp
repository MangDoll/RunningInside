// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnCheckPoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h"

// Sets default values
ARespawnCheckPoint::ARespawnCheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision_Box"));
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	CollisionBox->SetCollisionProfileName("PowerFull_Medicine");
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARespawnCheckPoint::OnOverlapBegin);

}

void ARespawnCheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter)
	{
		FVector CheckLocation = GetActorLocation();
		RunningInsideCharacter->GetCheckLocation = CheckLocation;
		UGameplayStatics::PlaySoundAtLocation(this, RespwanSound, GetActorLocation());
	}
}

// Called when the game starts or when spawned
void ARespawnCheckPoint::BeginPlay()
{
	Super::BeginPlay();
	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
}

// Called every frame
void ARespawnCheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

