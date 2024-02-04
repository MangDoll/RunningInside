// Fill out your copyright notice in the Description page of Project Settings.


#include "DeystroyActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h"

// Sets default values
ADeystroyActor::ADeystroyActor()
{	
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	CollisionBox->SetCollisionProfileName("Stamina Liquid Medicine");
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ADeystroyActor::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ADeystroyActor::BeginPlay()
{
	Super::BeginPlay();
	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ADeystroyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeystroyActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		RunningInsideCharacter->DestroyBlock();		
	}
}

