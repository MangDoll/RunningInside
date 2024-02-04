#include "SpeedBlock.h"
#include "Components/BoxComponent.h" //Box컴포넌트 가져오기
#include "Engine//Engine.h" //로그 출력같은 기능 담당
#include "Runtime/Engine/Classes/Kismet/Gameplaystatics.h" //게임관련 요소 접근 및 상호작용
#include "Components/StaticMeshComponent.h" //StaticMesh컴포넌트 가져오기
#include "GameFramework/Actor.h" //엑터 가져오기
#include "Components/AudioComponent.h"

// Sets default values
ASpeedBlock::ASpeedBlock()
{
	//시작하자마자 틱 함수 트루
	PrimaryActorTick.bCanEverTick = true;

	//메쉬컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//박스 콜리전 생성
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//박스 콜리전 범위
	CollisionBox->SetBoxExtent(FVector(32.0f, 32.0f, 32.0f));
	//박스콜리전 이름
	CollisionBox->SetCollisionProfileName("Speed_Trigger");
	//루트에 붙임
	CollisionBox->SetupAttachment(Mesh);


	//콜리전 박스 이벤트 생성
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASpeedBlock::OnOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ASpeedBlock::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ASpeedBlock::BeginPlay()
{
	Super::BeginPlay();

	RunningInsideCharacter = Cast<ARunningInsideCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void ASpeedBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//오버랩시 속도 장판 생성
void ASpeedBlock::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (RunningInsideCharacter && RunningInsideCharacter == OtherActor)
	{
		RunningInsideCharacter->SpeedBlock();
		UGameplayStatics::PlaySoundAtLocation(this, SpeedSound, GetActorLocation());
	}
}

void ASpeedBlock::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{

}