#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/AudioComponent.h"
#include "RunningInsideCharacter.generated.h"

UCLASS(config = Game)
class ARunningInsideCharacter : public ACharacter
{
	GENERATED_BODY()

		// ī�޶� �� ��ġ �� ĳ���� �ڿ��ִ� ī�޶� �ش� �� �Ʒ��δ� ��� private ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	//�����׼�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	//������ �Է�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	//ī�޶� �Է�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	//�޸��� �Է�
	UPROPERTY(VisibleAnywhere, Category = "Character Movement : Walking")
		bool IsSprinting;

	//�������� �Է�
	UPROPERTY(VisibleAnywhere, Category = "PlayerDoubleJump")
		bool bFirstJump = true;
	int JumpMaxCount = 2;
	int JumpCount = 0;
	float ReducedSpeed;
	bool IsJumping = false;

	//����Ʈ Ű �� ��
	bool bCanShiftKey;

public:
	//���׹̳�
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
		float MaxStamina = 100.0f;
	float Stamina_UsageRate;
	float Stamina_RechargeRate;

	//UI���׹̳�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
		float Stamina = MaxStamina;

	//�г� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AngryGage")
		float AngryGage = 0.0f;
	float MaxAngry = 100.0f;

	//��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* JumpSound2;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* PowerUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* DashGround;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* DestroySound;
	
	//�ִϸ��̼�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* LandMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Roll_Land_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Roll_Land2_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Turn_Left_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Turn_Right_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Powerful_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* Slide_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* FallDown_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* FirstJump_Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* SecondJump_Montage;

	//Ÿ�̸�
	FTimerHandle CountTimerHandle;
	void CountDown();

	UPROPERTY(BlueprintReadOnly)
		int Hours = 0;

	UPROPERTY(BlueprintReadOnly)
		int Minutes = 0;

	UPROPERTY(BlueprintReadOnly)
		int Seconds = 0;

	UPROPERTY(BlueprintReadOnly)
		int LapsCountVal = 0;

public:
	ARunningInsideCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//�޸���
	void Sprint();

	//�޸��� ����
	void StopSprinting();

	//��������
	void DoubleJump();

	//���� ��Ҵ��� Ȯ��
	void IsGround();

	//���׹̳� Ȯ��
	void ConsumeStamina();

	//Ÿ�̸� �Լ�(�⺻)
	FTimerHandle RestoreSpeedTimerHandle;
	float RestoreSpeedDelay;
	float OriginMaxWalkSpeed;

	//Ÿ�̸� �Լ�(�Ŀ�Ǯ ��� ��)
	FTimerHandle RestorePowerfulTimerHandle;
	float PowerFullRestoreDelay;

	//Ÿ�̸� �Լ�(���ǵ� ��� ���� ��)
	FTimerHandle RestoreSpeedBlockTimerHandle;
	float SpeedBlockRestoreDelay;

	//Ÿ�̸� �Լ�(��ŸƮ �뽬 ��)
	FTimerHandle RestoreStartDashTimerHandle;
	float StartDashRestoreDelay;

	//Ÿ�̸� �Լ�(�ٿ�� �뽬 ��)
	FTimerHandle RestoreBoundDashTimerHandle;
	float BoundDashRestoreDelay;

	FTimerHandle RestoreSlowBlockTimerHandle;
	float SlowBlockRestoreDelay;

	//���ƿ��� ���ǵ� ��
	float Target_Restore_Speed;

	//ShiftŰ ���� Ÿ�̸�
	FTimerHandle RestoreCanShiftKeyHandle;
	float SpeedBlockRestoreShiftKey;
	float PowerfulmodeRestoreShiftKey;
	float StartDashRestoreShiftKey;
	float TiredRestoreShiftKey;
	float BoundDashRestoreShiftKey;
	float SlowBlockRestoreShiftKey;

	//ȸ�� �Լ�
	void RestoreSpeed();
	void SpeedBlockRestoreSpeed();
	void PowerfullModeRestore();
	void StartDashRestore();
	void BoundDashRestore();
	void SlowBlockRestore();

	//���������� Ȯ���ϴ� �Լ�
	void FallingCheck();

	//�г��� �Լ�
	void StartPowerFullMode();

	//�� �Լ�
	void Turn();

	//�����̵� �Լ�
	void Slide();
	
	//�����̵� ��ž �Լ�
	void StopSlide();

	//�����Ʈ ȸ��
	FTimerHandle RestoreMovementHandle;
	float RestoreMovementDealy;

	//�����Ʈ ȸ�� �Լ�
	void EnableMovement();

	//����Ʈ Ű ��� ���� �Լ�
	void EnableShiftKey();

	//��Ȳ�� ���� ī�޶� ���� �Լ�
	void SkyCamera();
	void GroundCamera();
	void SuperJumpCamera();
	
	float SuperJumpCamera_Heigth;
	float SkyCamera_Heigth;


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//�ӵ� ���� �Լ�
	void SpeedBlock();
	bool IsSpeedBlock = false;
	bool SpeedBlockLandVal;

	//���ο� ��� �Լ�
	void SlowBlock();
	bool IsSlowBlock = false;
	bool SlowBlockLandVal;

	//�Ŀ�Ǯ ��� �Լ�
	void PowerFullMode();
	bool IsPowerful = false;
	
	//��ŸƮ �뽬 �Լ�
	void StartDash();
	bool IsStartDash = false;
	float FallingStartDash_Height;
	bool StartDashLandVal = false;
	

	//�ٿ�� ��� �뽬 �Լ�
	void BoundDash();
	bool IsBoundDash = false;
	float BoundStartDash_Height;
	bool BoundDashLandVal = false;

	//�ı� ���
	void DestroyBlock();

	//�� �Ʒ� �ִ��� üũ
	void CollisionDetection();

	//������ �Լ�
	void Respawn();

	//���������� üũ
	bool IsUnderGround;

	//���� �������� �� ������ ������
	FTimerHandle RespawnTimerHandle;

	//üũ ����Ʈ �ʱ�ȭ
	FVector GetCheckLocation = FVector(2000.0f, 2000.0f, 0.0f);

	//���� üũ
	float PreviousHeight;
	float HeigthThreshHold;

	float SpeedBlockSpeed;
	float PowerfulSpeed;
	float StartDashSpeed;
	float BoundDashSpeed;
	float SlowBlockSpeed;

};