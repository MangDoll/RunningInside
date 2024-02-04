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
		//���� �� ���� �ϸ� �����̴� ���� �ٲ�
		FVector TargetLocation = CurrentLocation + FVector(moveDistance, 0.0f, 0.0f);

		if (CurrentLocation.X < TargetLocation.X)
		{			
			//������ġ���� Ÿ����ġ�� ��� ����
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MoveSpeed);
			SetActorLocation(NewLocation);

			//���� x�� ������ ó�� ������ x�� ���� �� �Ÿ�
			Distance = CurrentLocation.X - InitialLocation.X;

			//�Ÿ� ��� �Լ�, �Ÿ��� 1.0f���� ������ False��Ŵ
			if (Distance > PlusLimitedDistance)
			{
				bMovingRight = false;
			}
		}
	}
	else 
	{
		//���� �� ���� �ϸ� �����̴� ���� �ٲ�
		FVector TargetLocation = CurrentLocation + FVector(-moveDistance, 0.0f, 0.0f);
		
		if (TargetLocation.X < CurrentLocation.X)
		{
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MoveSpeed);
			SetActorLocation(NewLocation);

			Distance = CurrentLocation.X - InitialLocation.X;

			//�Ÿ� ��� �Լ�, �Ÿ��� 1.0f���� ������ False��Ŵ
			if (Distance < MinusLimitedDistance)
			{
				bMovingRight = true;
			}
		}
	}
	
}

