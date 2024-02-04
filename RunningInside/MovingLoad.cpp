#include "MovingLoad.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h"
#include "cstdlib"

// Sets default values
AMovingLoad::AMovingLoad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box_Collision"));
	BoxCollision->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	BoxCollision->SetCollisionProfileName("MovingLoad");
	BoxCollision->SetupAttachment(Mesh);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMovingLoad::OnOverlapBegin);

	Distance = 0;
	PlusLimitedDistance;
	MinusLimitedDistance;
	bMovingRight = true;
}

void AMovingLoad::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AMovingLoad::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when the game starts or when spawned
void AMovingLoad::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
}

// Called every frame
void AMovingLoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovingPlatform();
}

void AMovingLoad::MovingPlatform()
{

	FVector CurrentLocation = GetActorLocation();

	if(bMovingRight)
	{
		//여기 값 수정 하면 움직이는 방향 바뀜
		FVector TargetLocation = CurrentLocation + FVector(moveDistance, 0.0f, 0.0f);

		if (CurrentLocation.X < TargetLocation.X)
		{			
			//현재위치에서 타겟위치로 계속 보간
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MoveSpeed);
			SetActorLocation(NewLocation);

			//현재 x축 값에서 처음 리스폰 x축 값을 뺀 거리
			Distance = CurrentLocation.X - InitialLocation.X;

			//거리 계산 함수, 거리가 1.0f보다 작으면 False시킴
			if (Distance > PlusLimitedDistance)
			{
				bMovingRight = false;
			}
		}
	}
	else 
	{
		//여기 값 수정 하면 움직이는 방향 바뀜
		FVector TargetLocation = CurrentLocation + FVector(-moveDistance, 0.0f, 0.0f);
		
		if (TargetLocation.X < CurrentLocation.X)
		{
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MoveSpeed);
			SetActorLocation(NewLocation);

			Distance = CurrentLocation.X - InitialLocation.X;

			//거리 계산 함수, 거리가 1.0f보다 작으면 False시킴
			if (Distance < MinusLimitedDistance)
			{
				bMovingRight = true;
			}
		}
	}
	
}

