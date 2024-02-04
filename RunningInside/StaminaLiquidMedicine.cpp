#include "StaminaLiquidMedicine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFrameWork/Actor.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h"

AStaminaLiquidMedicine::AStaminaLiquidMedicine()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	BoxCollision->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	BoxCollision->SetCollisionProfileName("Stamina Liquid Medicine");
	BoxCollision->SetupAttachment(Mesh);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AStaminaLiquidMedicine::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AStaminaLiquidMedicine::OnOverlapEnd);
}

void AStaminaLiquidMedicine::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

void AStaminaLiquidMedicine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStaminaLiquidMedicine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemSound, GetActorLocation());
		RunningInsideCharacter->Stamina = 100.0f;
		Destroy();
	}

}

void AStaminaLiquidMedicine::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
