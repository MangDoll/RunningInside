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
    //�ݸ��� ĸ�� ������
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    // ��Ʈ�ѷ� ȸ���� ĳ���͵� ȸ���ϸ� ū�ϳ�
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // ĳ���� �̵� ����
    GetCharacterMovement()->bOrientRotationToMovement = true; // ĳ���� �Է�   
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f); // ȸ���ӵ�

    // ĳ���� Ư�� ��
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 200.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    // ī�޶� ������(�ݸ����� �ε����� ī�޶� ������)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent); //��Ʈ ������Ʈ�� ī�޶� ���̱�
    CameraBoom->TargetArmLength = 250.0f; // ī�޶� ĳ���� �ڿ� �ִ� �Ÿ�   
    CameraBoom->bUsePawnControlRotation = true; //ī�޶� �����밣 ��Ʈ�ѷ� �������� ȸ�� 
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 100.0f); // ī�޶� �����̼� ����

    //ī�޶� �ٴ� �ӵ�
    CameraBoom->bEnableCameraLag = true; //ī�޶� ������� �ӵ� ����
    CameraBoom->CameraLagSpeed = 3.5f; //ī�޶� �ӵ�
    CameraBoom->bEnableCameraRotationLag = true; //ī�޶� ������� ȸ�� �ӵ� ����
    CameraBoom->CameraRotationLagSpeed = 8.5f; // ȸ�� ī�޶� �ӵ�
    
    // ���󰡴� ī�޶�
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // �����뿡 ī�޶� ���� �� ��Ʈ�ѷ� ���⿡ �°� ����
    FollowCamera->bUsePawnControlRotation = false; // ī�޶� ȸ������ �ʰ� ����

    IsSprinting = false; //������Ʈ Ȯ�� ����

    PrimaryActorTick.bStartWithTickEnabled = true; //�÷������ڸ��� Tickó��
    PrimaryActorTick.bCanEverTick = true; // Tickó���� �����ϰ���

    RestoreSpeedDelay = 5.0f; //���׹̳� 0�Ͻ� Ÿ�̸� ȣ�� �ð�
    PowerFullRestoreDelay = 10.0f; //�Ŀ�Ǯ ��� ���ӽð�
    SpeedBlockRestoreDelay = 6.0f; //���ǵ� ��� ���ӽð�
    StartDashRestoreDelay = 5.0f; //��Ÿ�� �뽬 ���ӽð�
    BoundDashRestoreDelay = 5.0f; //�ٿ�� �뽬 ���ӽð�
    SlowBlockRestoreDelay = 5.0f; //���ο� ��� ���ӽð�

    ReducedSpeed = 200.0f; // ���׹̳� 0�Ͻ� �����Ǵ� �ӵ�
    OriginMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed; //�����Ǵ� �ӵ� �� 
    Target_Restore_Speed = 600.0f; //�Լ� ���� Ÿ�̸� ���� ���ƿ��� �ӵ�

    //���� �Ѱ��� ���� ����
    HeigthThreshHold = 0;

    //�����Ʈ �����̽ð�
    RestoreMovementDealy = 2.0f;

    //�Ŀ�Ǯ �������
    IsPowerful = false;

    //�� �Ʒ� �ִ���
    IsUnderGround = false;

    IsBoundDash = false;

    //���� ���ǵ� �÷��ִ� �Լ��� Ȱ��ȭ �� Sprint Ű���� ���� �ð�
    SpeedBlockRestoreShiftKey = 5.0f;
    PowerfulmodeRestoreShiftKey = 10.0f;
    StartDashRestoreShiftKey = 5.0f;
    TiredRestoreShiftKey = 5.0f;
    BoundDashRestoreShiftKey = 5.0f;
    SlowBlockRestoreShiftKey = 5.0f;

    //Shift�� �̿��ϰ� �ִ���
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

// ƽ�Լ��� �� �����Ӹ��� üũ
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
    //�⺻ Ŭ���� �θ�
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

//1�� �� CollisionDetection�Լ� ȣ��
void ARunningInsideCharacter::Respawn()
{
    IsUnderGround = true;
    if (!GetWorldTimerManager().IsTimerActive(RespawnTimerHandle))
    {
        GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ARunningInsideCharacter::CollisionDetection, 1.0f, false);
    }
}

//�ݸ��� ���� �� ������ üũ �������� �̵�
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

//�Է�
void ARunningInsideCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

        //������
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunningInsideCharacter::Move);

        //����
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARunningInsideCharacter::DoubleJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARunningInsideCharacter::StopJumping);

        //ī�޶� ������
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARunningInsideCharacter::Look);
        
       
        //�޸���
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


//������
void ARunningInsideCharacter::Move(const FInputActionValue& Value)
{
    // ���� 2D�� �Է�
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr) //��Ʈ�ѷ��� �Է��� ������
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector 
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        const float MovementSizeSquared = MovementVector.SizeSquared();

        //�Է��� �ִ� ��� ������
        if (MovementSizeSquared > 0.0f)
        {
            AddMovementInput(ForwardDirection, MovementVector.Y);
            AddMovementInput(RightDirection, MovementVector.X);    
        }                          
    }       
}

//ī�޶�
void ARunningInsideCharacter::Look(const FInputActionValue& Value)
{
    // 2D ���� �� ������
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Yaw��(���� ��) Pitch��(Ⱦ ��) 
        AddControllerYawInput(LookAxisVector.X);

        AddControllerPitchInput(LookAxisVector.Y);
    }
}

//�޸���
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

//�ȱ����Ͻ�
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

//���׹̳� �Ҹ� Ȯ��
void ARunningInsideCharacter::ConsumeStamina()
{
    //���׹̳� �ٴ� ��
    if (IsSprinting && bCanShiftKey)
    {
        float Target_MaxArmLength = 400.0f;
        float Target_MaxCameraLagSpeed = 8.0f;
        float Target_CameraRotationLag = 15.0f;

        //FInterpTo�Լ� -> ��ǥ �ӵ����� ���������� �����ϴ� �Լ�
        //ī�޶� �Ÿ� ������ ����
        float new_ArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, Target_MaxArmLength,
            GetWorld()->GetDeltaSeconds(), 2.0f);

        //ī�޶� ���󰡴� �ӵ� ������ ����
        float new_LagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, Target_MaxCameraLagSpeed,
            GetWorld()->GetDeltaSeconds(), 0.04f);

        //ī�޶� ���󰡴� ȸ�� �ӵ� ������ ����
        float new_CameraRotationLagSpeed = FMath::FInterpTo(CameraBoom->CameraRotationLagSpeed, Target_CameraRotationLag,
            GetWorld()->GetDeltaSeconds(), 0.075f);

        CameraBoom->TargetArmLength = new_ArmLength;
        CameraBoom->CameraLagSpeed = new_LagSpeed;
        CameraBoom->CameraRotationLagSpeed = new_CameraRotationLagSpeed;

        //���׹̳� ������ ����
        if (Stamina <= 20.0f)
        {
            Stamina_UsageRate = 8.0f;
            Stamina = FMath::FInterpTo(Stamina, 0.0f, GetWorld()->GetDeltaSeconds(), Stamina_UsageRate);
        }
        else {
            Stamina_UsageRate = 0.5f;
            Stamina = FMath::FInterpTo(Stamina, 0.0f, GetWorld()->GetDeltaSeconds(), Stamina_UsageRate);

        }

        //���׹̳� 0�Ͻ� �ӵ� ������
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

            //ī�޶� �Ÿ� ������ ����
            float Decrease_ArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, Target_ArmLength,
                GetWorld()->GetDeltaSeconds(), 2.0f);

            //ī�޶� ���󰡴� �ӵ� ������ ����
            float Decrease_LagSpeed = FMath::FInterpTo(CameraBoom->CameraLagSpeed, Target_CameraLagSpeed,
                GetWorld()->GetDeltaSeconds(), 0.04f);

            CameraBoom->TargetArmLength = Decrease_ArmLength;
            CameraBoom->CameraLagSpeed = Decrease_LagSpeed;

            if (Stamina >= 90.0f)
            {
                Stamina_RechargeRate = 5.0f;
                //Stamina�� 90�̻� �Ͻ� 5�� ����
                Stamina = FMath::FInterpTo(Stamina, MaxStamina, GetWorld()->GetDeltaSeconds(), Stamina_RechargeRate);
            }
            else {
                Stamina_RechargeRate = 0.2f;
                //Stamina ����
                Stamina = FMath::FInterpTo(Stamina, MaxStamina, GetWorld()->GetDeltaSeconds(), Stamina_RechargeRate);
            }
        }
    }
}

//�ٽ� ȸ���Ǵ� �ӵ�
void ARunningInsideCharacter::RestoreSpeed()
{
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreSpeedTimerHandle);
}

//��, �� �����̵� �Լ�
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

//�����̵� ����
void ARunningInsideCharacter::Slide()
{
    float NewCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 0.7f;
    GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHeight, true);

    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::LeftControl))
    {
        PlayAnimMontage(Slide_Montage, 1, NAME_None);
    }
}

//�����̵� ����
void ARunningInsideCharacter::StopSlide()
{
    float OriginalCapsuleHeight = 96.0f;
    GetCapsuleComponent()->SetCapsuleHalfHeight(OriginalCapsuleHeight, true);

    StopAnimMontage(Slide_Montage);
}
  
//��������
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

//�������� ��ġ �� üũ
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

//���� �ִ��� üũ
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
        // FallingStartDash_Height ���� ��ġ ���� ���� �������� zŰ�� ������ �ӵ� �ø�
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

                //��ŸƮ �뽬 ���� ���ǵ� ��� ������ ���ǵ� ��� Ŭ����
                if (IsSpeedBlock)
                {
                    IsSpeedBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
                }

                //��ŸƮ �뽬 ���� �ٿ�� �뽬 ���� �� Ŭ����
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


//��ŸƮ �뽬 �� �ٽ� ȸ���Ǵ� �Լ�
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
        // FallingStartDash_Height ���� ��ġ ���� ���� �������� zŰ�� ������ �ӵ� �ø�
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

                //�ٿ�� �뽬 ���� ���ǵ� ��� �������̸� ���ǵ� ��� Ŭ����
                if (IsSpeedBlock)
                {
                    IsSpeedBlock = false;
                    GetWorldTimerManager().ClearTimer(RestoreSpeedBlockTimerHandle);
                }

                //�ٿ�� �뽬 ���� ��ŸƮ �뽬 ���� ���̸� Ŭ����
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

//��ŸƮ �뽬 �� �ٽ� ȸ���Ǵ� �Լ�
void ARunningInsideCharacter::BoundDashRestore()
{
    IsBoundDash = false;
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestoreBoundDashTimerHandle);
}

//�ϴÿ� ������ ī�޶�
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

//���� ������ ī�޶�
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

//��Ʈ���� ���
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

//�ٽ� �����̰� �ϴ� �Լ�
void ARunningInsideCharacter::EnableMovement()
{
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

//���ǵ� ��� ����� ��
void ARunningInsideCharacter::SpeedBlock()
{
    //�Ŀ�Ǯ ��尡 �ƴ� �� ���ǵ� ��� �۵�
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

//���ǵ��� ����� �� �ٽ� ȸ���Ǵ� �Լ�
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

//Q������ �Ŀ�Ǯ ��� �۵�
void ARunningInsideCharacter::StartPowerFullMode()
{
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Q))
    {
        PowerFullMode();
    }
}

//�Ŀ�Ǯ ���
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
    //Powerful����϶� ���ǵ� ����� ����� ��
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

//�Ŀ�Ǯ ���� �ٽ� ȸ���Ǵ� �Լ�
void ARunningInsideCharacter::PowerfullModeRestore()
{
    IsPowerful = false;
    IsSprinting = false;

    GetCharacterMovement()->MaxWalkSpeed = Target_Restore_Speed;
    GetWorldTimerManager().ClearTimer(RestorePowerfulTimerHandle);
}

//shift ��밡�� �Լ�
void ARunningInsideCharacter::EnableShiftKey()
{
    bCanShiftKey = true;
}