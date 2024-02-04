#include "RunningInsideCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "BoundBlock.h"



ARunningInsideCharacter::ARunningInsideCharacter()
{
    //콜리전 캡슐 사이즈
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    // 컨트롤러 회전시 캐릭터도 회전하면 큰일남
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // 캐릭터 이동 구성
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 입력   
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f); // 회전속도

    // 캐릭터 특성 값
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 200.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    // 카메라 지지대(콜리전과 부딪히면 카메라를 가까이)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent); //루트 컴포넌트에 카메라 붙이기
    CameraBoom->TargetArmLength = 250.0f; // 카메라가 캐릭터 뒤에 있는 거리   
    CameraBoom->bUsePawnControlRotation = true; //카메라 지지대간 컨트롤러 기준으로 회전 
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 100.0f); // 카메라 로케이션 변경

    //카메라 붙는 속도
    CameraBoom->bEnableCameraLag = true; //카메라 따라오는 속도 조정
    CameraBoom->CameraLagSpeed = 3.5f; //카메라 속도
    CameraBoom->bEnableCameraRotationLag = true; //카메라 따라오는 회전 속도 조정
    CameraBoom->CameraRotationLagSpeed = 8.5f; // 회전 카메라 속도
    
    // 따라가는 카메라
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 지지대에 카메라 붙인 후 컨트롤러 방향에 맞게 조정
    FollowCamera->bUsePawnControlRotation = false; // 카메라가 회전하지 않게 만듦

    IsSprinting = false; //스프린트 확인 변수

    PrimaryActorTick.bStartWithTickEnabled = true; //플레이하자마자 Tick처리
    PrimaryActorTick.bCanEverTick = true; // Tick처리를 가능하게함

    RestoreSpeedDelay = 5.0f; //스테미너 0일시 타이머 호출 시간
    PowerFullRestoreDelay = 10.0f; //파워풀 모드 지속시간
    SpeedBlockRestoreDelay = 6.0f; //스피드 블록 지속시간
    StartDashRestoreDelay = 5.0f; //스타드 대쉬 지속시간
    BoundDashRestoreDelay = 5.0f; //바운드 대쉬 지속시간
    SlowBlockRestoreDelay = 5.0f; //슬로우 블록 지속시간

    ReducedSpeed = 200.0f; // 스테미너 0일시 설정되는 속도
    OriginMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed; //복구되는 속도 값 
    Target_Restore_Speed = 600.0f; //함수 이후 타이머 사용시 돌아오는 속도

    //높이 한계점 측정 변수
    HeigthThreshHold = 0;

    //무브먼트 딜레이시간
    RestoreMovementDealy = 2.0f;

    //파워풀 모드인지
    IsPowerful = false;

    //땅 아래 있는지
    IsUnderGround = false;

    IsBoundDash = false;

    //각종 스피드 올려주는 함수들 활성화 시 Sprint 키보드 봉인 시간
    SpeedBlockRestoreShiftKey = 5.0f;
    PowerfulmodeRestoreShiftKey = 10.0f;
    StartDashRestoreShiftKey = 5.0f;
    TiredRestoreShiftKey = 5.0f;
    BoundDashRestoreShiftKey = 5.0f;
    SlowBlockRestoreShiftKey = 5.0f;

    //Shift를 이용하고 있는지
    bCanShiftKey = true;

    FallingStartDash_Height = 50.0f;
    BoundStartDash_Height = 15.0f;

    SuperJumpCamera_Heigth = -40.0f;
    SkyCamera_Heigth = 30.0f;

    SpeedBlockLandVal = false;
    SlowBlockLandVal = false;

    SpeedBlockSpeed = 1500.0f;
    PowerfulSpeed = 2000.0f;
    StartDashSpeed = 1500.0f;
    BoundDashSpeed = 1500.0f;
    SlowBlockSpeed = ReducedSpeed;

}

// 틱함수로 매 프레임마다 체크
void ARunningInsideCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    IsGround();
    ConsumeStamina();
    FallingCheck();
    StartPowerFullMode();
    StartDash();
    BoundDash();
}


void ARunningInsideCharacter::BeginPlay()
{
    //기본 클래스 부름
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(CountTimerHandle, this, &ARunningInsideCharacter::CountDown, 1.0f, true, 0.0f);

    //Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void ARunningInsideCharacter::CountDown()
{
    if (Seconds >= 0)
    {
        Seconds = Seconds + 1;
        if (Seconds == 60)
        {
            Minutes += 1;
            Seconds = 0;
        }
        if (Minutes == 60)
        {
            Hours += 1;
            Minutes = 0;
        }
    }
}

//1초 뒤 CollisionDetection함수 호출
void ARunningInsideCharacter::Respawn()
{
    IsUnderGround = true;
    if (!GetWorldTimerManager().IsTimerActive(RespawnTimerHandle))
    {
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ARunningInsideCharacter::CollisionDetection, 1.0f, false);
    }
}

//콜리전 접근 시 리스폰 체크 지점으로 이동
void ARunningInsideCharacter::CollisionDetection()
{
    if (IsUnderGround)
    {   
        FVector RespawnLocation = GetCheckLocation;
        UE_LOG(LogTemp, Log, TEXT("Respawn Vector = %s"), *RespawnLocation.ToString());
        SetActorLocation(RespawnLocation);
        IsUnderGround = false;
    }
}

//입력
void ARunningInsideCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

        //움직임
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunningInsideCharacter::Move);

        //점프
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARunningInsideCharacter::DoubleJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARunningInsideCharacter::StopJumping);

        //카메라 움직임
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunningInsideCharacter::Look);
        
       
        //달리기
        PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARunningInsideCharacter::Sprint);
        PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARunningInsideCharacter::StopSprinting);
        

        PlayerInputComponent->BindAction("Turn_Left", IE_Pressed, this, &ARunningInsideCharacter::Turn);
        PlayerInputComponent->BindAction("Turn_Left", IE_Released, this, &ARunningInsideCharacter::Turn);

        PlayerInputComponent->BindAction("Turn_Right", IE_Pressed, this, &ARunningInsideCharacter::Turn);
        PlayerInputComponent->BindAction("Turn_Right", IE_Released, this, &ARunningInsideCharacter::Turn);

        PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ARunningInsideCharacter::Slide);
        PlayerInputComponent->BindAction("Slide", IE_Released, this, &ARunningInsideCharacter::StopSlide);
    }

}


//움직임
void ARunningInsideCharacter::Move(const FInputActionValue& Value)
{
    // 벡터 2D값 입력
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr) //컨트롤러에 입력이 들어오면
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        const float MovementSizeSquared = MovementVector.SizeSquared();

        //입력이 있는 경우 움직임
        if (MovementSizeSquared > 0.0f)
        {
            AddMovementInput(ForwardDirection, MovementVector.Y);
            AddMovementInput(RightDirection, MovementVector.X);    
        }                          
    }       
}

//카메라
void ARunningInsideCharacter::Look(const FInputActionValue& Value)
{
    // 2D 벡터 값 가져옴
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Yaw값(수직 축) Pitch값(횡 축) 
        AddControllerYawInput(LookAxisVector.X);

        AddControllerPitchInput(LookAxisVector.Y);
    }
}

//달리기
void ARunningInsideCharacter::Sprint()
{
    if (!IsSprinting && Stamina > 10.0f && bCanShiftKey)
    {
        IsSprinting = true;
        float Target_WalkSpeed = 1200.0f;

        GetCharacterMovement()->MaxWalkSpeed = Target_WalkSpeed;
        UE_LOG(LogTemp, Log, TEXT("RunCurrentSpeed = %.2f"), Target_WalkSpeed);               
    }
}

//걷기중일시
void ARunningInsideCharacter::StopSprinting()
{
    if (IsSprinting && bCanShiftKey)
    {
        float Target_Start_Speed = 600.0f;
        IsSprinting = false;

        if (Stamina < 10.0f)
        {
            GetCharacterMovement()->MaxWalkSpeed = 0.0f;
            UE_LOG(LogTemp, Log, TEXT("Stamina zero start WalkSpeed = %.2f"), GetCharacterMovement()->MaxWalkSpeed);
        }
        else {
            GetCharacterMovement()->MaxWalkSpeed = Target_Start_Speed;
            UE_LOG(LogTemp, Log, TEXT("Stamina none zero start WalkSpeed = %.2f"), Target_Start_Speed);
        }
    }
}

//스테미나 소모 확인
void ARunningInsideCharacter::ConsumeStamina()
{
    //스테미너 다는 중
    if (IsSprinting && bCanShiftKey)
    {
        float Target_MaxArmLength = 400.0f;
        float Target_MaxCameraLagSpeed = 8.0f;
        float Target_CameraRotationLag = 15.0f;

        //FInterpTo함수 -> 목표 속도까지 점진적으로 증가하는 함수
        //카메라 거리 점진적 증가
        float new_ArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, Target_MaxArmLength,
            GetWorld()->GetDeltaSeconds(), 2.0f);

        //카메라 따라가는 속도 점진적 증가
        float new_LagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, Target_MaxCameraLagSpeed,
            GetWorld()->GetDeltaSeconds(), 0.04f);

        //카메라 따라가는 회전 속도 점진적 증가
        float new_CameraRotationLagSpeed = FMath::FInterpTo(CameraBoom->CameraRotationLagSpeed, Target_CameraRotationLag,
            GetWorld()->GetDeltaSeconds(), 0.075f);

        CameraBoom->TargetArmLength = new_ArmLength;
        CameraBoom->CameraLagSpeed = new_LagSpeed;
        CameraBoom->CameraRotationLagSpeed = new_CameraRotationLagSpeed;

        //스테미너 점진적 감소
        if (Stamina <= 20.0f)
        {
            Stamina_UsageRate = 8.0f;
            Stamina = FMath::FInterpTo(Stamina, 0.0f, GetWorld()->GetDeltaSeconds(), Stamina_UsageRate);
        }
        else {
            Stamina_UsageRate = 0.5f;
            Stamina = FMath::FInterpTo(Stamina, 0.0f, GetWorld()->GetDeltaSeconds(), Stamina_UsageRate);

        }

        //스테미너 0일시 속도 느려짐
        if (Stamina == 0.0f)
        {
            IsSprinting = true;
            GetCharacterMovement()->MaxWalkSpeed = ReducedSpeed;

            if (!GetWorldTimerManager().IsTimerActive(RestoreMovementHandle))
            {
                GetWorldTimerManager().SetTimer(RestoreMovementHandle, this, &ARunningInsideCharacter::RestoreSpeed, RestoreMovementDealy, false);
            }
        }
    }

    else
    {
        if (Stamina < MaxStamina)
        {
            float Target_ArmLength = 200.0f;
            float Target_CameraLagSpeed = 3.5f;

            //카메라 거리 점진적 감소
            float Decrease_ArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, Target_ArmLength,
                GetWorld()->GetDeltaSeconds(), 2.0f);

            //카메라 따라가는 속도 점진적 감소
            float Decrease_LagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, Target_CameraLagSpeed,
                GetWorld()->GetDeltaSeconds(), 0.04f);

            CameraBoom->TargetArmLength = Decrease_ArmLength;
            CameraBoom->CameraLagSpeed = Decrease_LagSpeed;

            if (Stamina >= 90.0f)
            {
                Stamina_RechargeRate = 5.0f;
                //Stamina가 90이상 일시 5씩 증가
                Stamina = FMath::FInterpTo(Stamina, MaxStamina, GetWorld()->GetDeltaSeconds(), Stamina_RechargeRate);
            }
            else {
                Stamina_RechargeRate = 0.2f;
                //Stamina 증가
                Stamina = FMath::FInterpTo(Stamina, MaxStamina, GetWorld()->GetDeltaSeconds(), Stamina_RechargeRate);
            }
        }
    }
}

//다시 회복되는 속도
void ARunningInsideCharacter::RestoreSpeed()
{
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreSpeedTimerHandle);
}

//좌, 우 순간이동 함수
void ARunningInsideCharacter::Turn()
{
    const float MoveAmount = 200.0f;
    FVector RightVector = GetActorRightVector();

    if ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) && IsSprinting) || 
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) && IsSpeedBlock)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) && IsPowerful)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) && IsStartDash)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) && IsBoundDash)))
    {
        
       AddActorWorldOffset(-RightVector * MoveAmount);
       PlayAnimMontage(Turn_Left_Montage, 1, NAME_None);                
    }

    if ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) && IsSprinting) || 
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) && IsSpeedBlock)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) && IsPowerful)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) && IsStartDash)) ||
        ((GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) && IsBoundDash)))
    {
       
       AddActorWorldOffset(RightVector * MoveAmount);
       PlayAnimMontage(Turn_Right_Montage, 1, NAME_None);
            
    }

}

//슬라이드 시작
void ARunningInsideCharacter::Slide()
{
    float NewCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 0.7f;
    GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHeight, true);

    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::LeftControl))
    {
        PlayAnimMontage(Slide_Montage, 1, NAME_None);
    }
}

//슬라이드 중지
void ARunningInsideCharacter::StopSlide()
{
    float OriginalCapsuleHeight = 96.0f;
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHeight, true);

    StopAnimMontage(Slide_Montage);
}
  
//더블점프
void ARunningInsideCharacter::DoubleJump()
{
    if (bFirstJump && JumpCount < JumpMaxCount)
    {
        bFirstJump = false;
        IsJumping = true;
        ACharacter::Jump();
        JumpCount++;
        PlayAnimMontage(FirstJump_Montage, 1, NAME_None);
        UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());

        if (Stamina < 10.0f)
        {
            if (!GetWorldTimerManager().IsTimerActive(RestoreSpeedTimerHandle))
            {
                GetWorldTimerManager().SetTimer(RestoreSpeedTimerHandle, this, &ARunningInsideCharacter::RestoreSpeed, RestoreSpeedDelay, false);
            }
            GetCharacterMovement()->MaxWalkSpeed = 0.0f;
        }
    }

    else if (!bFirstJump && JumpCount < JumpMaxCount)
    {
        LaunchCharacter(FVector(0.0f, 0.0f, 700.0f), false, true);
        JumpCount++;
        IsJumping = true;
        PlayAnimMontage(SecondJump_Montage, 1, NAME_None);
        UGameplayStatics::PlaySoundAtLocation(this, JumpSound2, GetActorLocation());

        if (Stamina < 10.0f)
        {
            if (!GetWorldTimerManager().IsTimerActive(RestoreSpeedTimerHandle))
            {
                GetWorldTimerManager().SetTimer(RestoreSpeedTimerHandle, this, &ARunningInsideCharacter::RestoreSpeed, RestoreSpeedDelay, false);
            }
            GetCharacterMovement()->MaxWalkSpeed = 0.0f;
        }
    }
}

//떨어질때 위치 값 체크
void ARunningInsideCharacter::FallingCheck()
{
    float CurrentHeight = GetActorLocation().Z;
    float fallAcceleration = 50.0f;
    HeigthThreshHold = (CurrentHeight - PreviousHeight) * -1;

    PreviousHeight = CurrentHeight;
    if (HeigthThreshHold > 0)
    {
        float Target_MaxCameraLagSpeed = 10.0f;
        float Target_CameraRotationLag = 20.0f;
        CameraBoom->CameraLagSpeed = Target_MaxCameraLagSpeed;
        CameraBoom->CameraRotationLagSpeed = Target_CameraRotationLag;
    }
}

//땅에 있는지 체크
void  ARunningInsideCharacter::IsGround()
{
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        if (GetVelocity().Size() > 0.0f && IsJumping)
        {
            IsJumping = false;
            if (JumpCount <= 1 && !IsStartDash)
            {
                PlayAnimMontage(Roll_Land2_Montage, 1, NAME_None);
            }
            if (JumpCount > 1 && !IsStartDash)
            {
                PlayAnimMontage(Roll_Land_Montage, 1, NAME_None);
            }           
        }

        JumpCount = 0;
        bFirstJump = true;

        GroundCamera();
    }

    if (GetCharacterMovement()->IsFalling() && HeigthThreshHold > SkyCamera_Heigth)
    {
        SkyCamera();
    }

    if (GetCharacterMovement()->IsFalling() && HeigthThreshHold < SuperJumpCamera_Heigth)
    {
        SuperJumpCamera();          
    }
}

void ARunningInsideCharacter::StartDash()
{
    if (!IsPowerful)
    {
        // FallingStartDash_Height 값의 위치 보다 높게 떨어질때 z키를 누르면 속도 올림
        if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
        {
            if (HeigthThreshHold > FallingStartDash_Height)
            {
                IsStartDash = true;
                bCanShiftKey = false;

                UGameplayStatics::PlaySoundAtLocation(this, DashGround, GetActorLocation());

                GetCharacterMovement()->MaxWalkSpeed = StartDashSpeed;
                

                if (!GetWorldTimerManager().IsTimerActive(RestoreStartDashTimerHandle))
                {
                    GetWorldTimerManager().SetTimer(RestoreStartDashTimerHandle, this, &ARunningInsideCharacter::StartDashRestore, StartDashRestoreDelay, false);

                }
                PlayAnimMontage(LandMontage, 1, NAME_None);

                if (!bCanShiftKey)
                {
                    GetWorldTimerManager().SetTimer(RestoreCanShiftKeyHandle, this, &ARunningInsideCharacter::EnableShiftKey, StartDashRestoreShiftKey, false);
                }

                //스타트 대쉬 도중 스피드 블록 밟으면 스피드 블록 클리어
                if (IsSpeedBlock)
                {
                    IsSpeedBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
                }

                //스타트 대쉬 도중 바운드 대쉬 가동 시 클리어
                if (IsBoundDash)
                {
                    IsBoundDash = false;
                    GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
                }

                if (IsSlowBlock)
                {
                    IsSlowBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSlowBlockTimerHandle);
                }
            }                   
        }
    }
    else {

        if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
        {
            if (HeigthThreshHold > FallingStartDash_Height)
            {
                bCanShiftKey = false;

                UGameplayStatics::PlaySoundAtLocation(this, DashGround, GetActorLocation());

                GetCharacterMovement()->MaxWalkSpeed = PowerfulSpeed;
               
                PlayAnimMontage(LandMontage, 1, NAME_None);                
            }
        }
    }
}


//스타트 대쉬 후 다시 회복되는 함수
void ARunningInsideCharacter::StartDashRestore()
{
    IsStartDash = false;
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreStartDashTimerHandle);
}


void ARunningInsideCharacter::BoundDash()
{
    if (!IsPowerful)
    {
        // FallingStartDash_Height 값의 위치 보다 높게 떨어질때 z키를 누르면 속도 올림
        if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
        {
            if (HeigthThreshHold > BoundStartDash_Height)
            {
                IsBoundDash = true;
                bCanShiftKey = false;
                BoundDashLandVal = true;

                UGameplayStatics::PlaySoundAtLocation(this, DashGround, GetActorLocation());

                GetCharacterMovement()->MaxWalkSpeed = BoundDashSpeed;

                GetWorldTimerManager().SetTimer(RestoreBoundDashTimerHandle, this, &ARunningInsideCharacter::SpeedBlockRestoreSpeed, BoundDashRestoreDelay, false);

                if (BoundDashLandVal)
                {
                    GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
                    GetWorldTimerManager().SetTimer(RestoreBoundDashTimerHandle, this, &ARunningInsideCharacter::SpeedBlockRestoreSpeed, BoundDashRestoreDelay, false);
                }

                PlayAnimMontage(LandMontage, 1, NAME_None);

                if (!bCanShiftKey)
                {
                    GetWorldTimerManager().SetTimer(RestoreCanShiftKeyHandle, this, &ARunningInsideCharacter::EnableShiftKey, BoundDashRestoreShiftKey, false);
                }

                //바운드 대쉬 도중 스피드 블록 가동중이면 스피드 블록 클리어
                if (IsSpeedBlock)
                {
                    IsSpeedBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
                }

                //바운드 대쉬 도중 스타트 대쉬 가동 중이면 클리어
                if (IsStartDash)
                {
                    IsStartDash = false;
                    GetWorldTimerManager().ClearTimer(RestoreStartDashTimerHandle);
                }

                if (IsSlowBlock)
                {
                    IsSlowBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSlowBlockTimerHandle);
                }
            }
        }
    }
    else {

        if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
        {
            if (HeigthThreshHold > BoundStartDash_Height)
            {
                UGameplayStatics::PlaySoundAtLocation(this, DashGround, GetActorLocation());

                GetCharacterMovement()->MaxWalkSpeed = PowerfulSpeed;

                PlayAnimMontage(LandMontage, 1, NAME_None);                
            }
        }
    }
}

//스타트 대쉬 후 다시 회복되는 함수
void ARunningInsideCharacter::BoundDashRestore()
{
    IsBoundDash = false;
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
}

//하늘에 있을때 카메라
void ARunningInsideCharacter::SkyCamera()
{
    FVector OriginCameraLocation = CameraBoom->GetRelativeLocation();
    FRotator OriginCameraRotation = FollowCamera->GetRelativeRotation();

    FVector TargetCameraLocation = FVector(100.0f, 0, 100.0f);
    FRotator TargetCameraRotation = FRotator(-60.0f, 0, 0);

    float ChangeSpeed = 5.0f;

    FVector NewLocation = FMath::VInterpTo(OriginCameraLocation, TargetCameraLocation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);
    FRotator NewRotation = FMath::RInterpTo(OriginCameraRotation, TargetCameraRotation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);

    CameraBoom->SetRelativeLocation(NewLocation);
    FollowCamera->SetRelativeRotation(NewRotation);
}

//지상에 있을때 카메라
void ARunningInsideCharacter::GroundCamera()
{
    FVector OriginCameraLocation = CameraBoom->GetRelativeLocation();
    FRotator OriginCameraRotation = FollowCamera->GetRelativeRotation();

    FVector TargetCameraLocation = FVector(0, 0, 0);
    FRotator TargetCameraRotation = FRotator(-20.0f, 0, 0);

    float ChangeSpeed = 2.5f;

    FVector NewLocation = FMath::VInterpTo(OriginCameraLocation, TargetCameraLocation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);
    FRotator NewRotation = FMath::RInterpTo(OriginCameraRotation, TargetCameraRotation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);

    CameraBoom->SetRelativeLocation(TargetCameraLocation);
    FollowCamera->SetRelativeRotation(TargetCameraRotation);
}

void ARunningInsideCharacter::SuperJumpCamera()
{
    FVector OriginCameraLocation = CameraBoom->GetRelativeLocation();
    FRotator OriginCameraRotation = FollowCamera->GetRelativeRotation();

    FVector TargetCameraLocation = FVector(-150.0f, 0, -150.0f);
    FRotator TargetCameraRotation = FRotator(20.0f, 0, 0);

    float ChangeSpeed = 5.0f;

    FVector NewLocation = FMath::VInterpTo(OriginCameraLocation, TargetCameraLocation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);
    FRotator NewRotation = FMath::RInterpTo(OriginCameraRotation, TargetCameraRotation, GetWorld()->GetDeltaSeconds(), ChangeSpeed);

    CameraBoom->SetRelativeLocation(NewLocation);
    FollowCamera->SetRelativeRotation(NewRotation);
}

//디스트로이 블록
void ARunningInsideCharacter::DestroyBlock()
{
    if (!IsPowerful)
    {
        GetCharacterMovement()->DisableMovement();
        GetCharacterMovement()->StopMovementImmediately();

        if (!GetWorldTimerManager().IsTimerActive(RestoreMovementHandle))
        {
            GetWorldTimerManager().SetTimer(RestoreMovementHandle, this, &ARunningInsideCharacter::EnableMovement, RestoreMovementDealy, false);
        }
        PlayAnimMontage(FallDown_Montage, 1, NAME_None);
        UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());

        AngryGage += FMath::FInterpTo(0, MaxAngry, GetWorld()->GetDeltaSeconds(), 1.0f);
    }
    else
    {
        UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
    }
}

//다시 움직이게 하는 함수
void ARunningInsideCharacter::EnableMovement()
{
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

//스피드 블록 밟았을 때
void ARunningInsideCharacter::SpeedBlock()
{
    //파워풀 모드가 아닐 때 스피드 블록 작동
    if (!IsPowerful)
    {
        IsSpeedBlock = true;
        SpeedBlockLandVal = true;

        GetWorldTimerManager().SetTimer(RestoreSpeedBlockTimerHandle, this, &ARunningInsideCharacter::SpeedBlockRestoreSpeed, SpeedBlockRestoreDelay, false);

        if (SpeedBlockLandVal)
        {
            GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
            GetWorldTimerManager().SetTimer(RestoreSpeedBlockTimerHandle, this, &ARunningInsideCharacter::SpeedBlockRestoreSpeed, SpeedBlockRestoreDelay, false);
        }


        bCanShiftKey = false;
        if (!bCanShiftKey)
        {
            GetWorldTimerManager().SetTimer(RestoreCanShiftKeyHandle, this, &ARunningInsideCharacter::EnableShiftKey, SpeedBlockRestoreShiftKey, false);
        }

        if (IsStartDash)
        {
            IsStartDash = false;
            GetWorldTimerManager().ClearTimer(RestoreStartDashTimerHandle);
        }

        if (IsBoundDash)
        {
            IsBoundDash = false;
            GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
        }

        if (IsSlowBlock)
        {
            IsSlowBlock = false;
            GetWorldTimerManager().ClearTimer(RestoreSlowBlockTimerHandle);
        }

        GetCharacterMovement()->MaxWalkSpeed = SpeedBlockSpeed;
    }

    else {
        GetCharacterMovement()->MaxWalkSpeed = PowerfulSpeed;       
    }
}

//스피드블록 밟았을 시 다시 회복되는 함수
void ARunningInsideCharacter::SpeedBlockRestoreSpeed()
{
    IsSprinting = false;
    IsSpeedBlock = false;
    SpeedBlockLandVal = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
}

void ARunningInsideCharacter::SlowBlock()
{
    if (!IsPowerful)
    {
        IsSlowBlock = true;
        SlowBlockLandVal = true;

        GetWorldTimerManager().SetTimer(RestoreSlowBlockTimerHandle, this, &ARunningInsideCharacter::SlowBlockRestore, SlowBlockRestoreDelay, false);

        if (SlowBlockLandVal)
        {
            GetWorldTimerManager().ClearTimer(RestoreSlowBlockTimerHandle);
            GetWorldTimerManager().SetTimer(RestoreSlowBlockTimerHandle, this, &ARunningInsideCharacter::SlowBlockRestore, SlowBlockRestoreDelay, false);
        }


        bCanShiftKey = false;
        if (!bCanShiftKey)
        {
            GetWorldTimerManager().SetTimer(RestoreCanShiftKeyHandle, this, &ARunningInsideCharacter::EnableShiftKey, SlowBlockRestoreShiftKey, false);
        }

        if (IsStartDash)
        {
            IsStartDash = false;
            GetWorldTimerManager().ClearTimer(RestoreStartDashTimerHandle);
        }

        if (IsBoundDash)
        {
            IsBoundDash = false;
            GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
        }

        if (IsSpeedBlock)
        {
            IsSpeedBlock = false;
            GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
        }

        GetCharacterMovement()->MaxWalkSpeed = SlowBlockSpeed;
    }

    else {
        GetCharacterMovement()->MaxWalkSpeed = PowerfulSpeed;
    }
}

void ARunningInsideCharacter::SlowBlockRestore()
{
    IsSprinting = false;
    IsSlowBlock = false;
    SlowBlockLandVal = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreSlowBlockTimerHandle);
}

//Q누를시 파워풀 모드 작동
void ARunningInsideCharacter::StartPowerFullMode()
{
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Q))
    {
        PowerFullMode();
    }
}

//파워풀 모드
void ARunningInsideCharacter::PowerFullMode()
{
    if (AngryGage >= 100.0f && Stamina >= 100.0f)
    {
        Stamina = 10.0f;
        AngryGage = 0.0f;
        IsPowerful = true;
        bCanShiftKey = false;

        UGameplayStatics::PlaySoundAtLocation(this, PowerUp, GetActorLocation());
        PlayAnimMontage(Powerful_Montage, 1, NAME_None);

        GetCharacterMovement()->MaxWalkSpeed = PowerfulSpeed;
 

        if (!GetWorldTimerManager().IsTimerActive(RestorePowerfulTimerHandle))
        {
            GetWorldTimerManager().SetTimer(RestorePowerfulTimerHandle, this, &ARunningInsideCharacter::PowerfullModeRestore, PowerFullRestoreDelay, false);
        }

        if (!bCanShiftKey)
        {
            GetWorldTimerManager().SetTimer(RestoreCanShiftKeyHandle, this, &ARunningInsideCharacter::EnableShiftKey, PowerfulmodeRestoreShiftKey, false);
        }
    }
    //Powerful모드일때 스피드 블록을 밟았을 시
    if (IsSpeedBlock){ 
        IsSpeedBlock = false;
        GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
    }
    if (IsStartDash){
        IsStartDash = false;
        GetWorldTimerManager().ClearTimer(RestoreStartDashTimerHandle);
    }
    if (IsBoundDash){
        IsBoundDash = false;
        GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
    }
}

//파워풀 모드시 다시 회복되는 함수
void ARunningInsideCharacter::PowerfullModeRestore()
{
    IsPowerful = false;
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestorePowerfulTimerHandle);
}

//shift 사용가능 함수
void ARunningInsideCharacter::EnableShiftKey()
{
    bCanShiftKey = true;
}