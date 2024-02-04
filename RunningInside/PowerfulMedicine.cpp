#include "PowerfulMedicine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h"

APowerfulMedicine::APowerfulMedicine()
{

	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision_Box"));
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	CollisionBox->SetCollisionProfileName("PowerFull_Medicine");
	CollisionBox->SetupAttachment(Mesh);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APowerfulMedicine::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &APowerfulMedicine::OnOverlapEnd);

	Fill_Angry = 10.0f;
}


void APowerfulMedicine::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void APowerfulMedicine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APowerfulMedicine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RunningInsideCharacter)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemSound, GetActorLocation());
		RunningInsideCharacter->AngryGage += Fill_Angry;
		Destroy();
	}

}

void APowerfulMedicine::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}