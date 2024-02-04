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

		// 카메라 붐 위치 및 캐릭터 뒤에있는 카메라 해당 줄 아래로는 모두 private 임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	//점프액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	//움직임 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	//카메라 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	//달리기 입력
	UPROPERTY(VisibleAnywhere, Category = "Character Movement : Walking")
		bool IsSprinting;

	//더블점프 입력
	UPROPERTY(VisibleAnywhere, Category = "PlayerDoubleJump")
		bool bFirstJump = true;
	int JumpMaxCount = 2;
	int JumpCount = 0;
	float ReducedSpeed;
	bool IsJumping = false;

	//쉬프트 키 불 값
	bool bCanShiftKey;

public:
	//스테미나
	UPROPERTY(VisibleAnywhere, Category = "Stamina")
		float MaxStamina = 100.0f;
	float Stamina_UsageRate;
	float Stamina_RechargeRate;

	//UI스테미나
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
		float Stamina = MaxStamina;

	//분노 게이지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AngryGage")
		float AngryGage = 0.0f;
	float MaxAngry = 100.0f;

	//점프사운드
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
	
	//애니메이션
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

	//타이머
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

	//달리기
	void Sprint();

	//달리기 멈춤
	void StopSprinting();

	//더블점프
	void DoubleJump();

	//땅에 닿았는지 확인
	void IsGround();

	//스테미나 확인
	void ConsumeStamina();

	//타이머 함수(기본)
	FTimerHandle RestoreSpeedTimerHandle;
	float RestoreSpeedDelay;
	float OriginMaxWalkSpeed;

	//타이머 함수(파워풀 모드 시)
	FTimerHandle RestorePowerfulTimerHandle;
	float PowerFullRestoreDelay;

	//타이머 함수(스피드 블록 밟을 시)
	FTimerHandle RestoreSpeedBlockTimerHandle;
	float SpeedBlockRestoreDelay;

	//타이머 함수(스타트 대쉬 시)
	FTimerHandle RestoreStartDashTimerHandle;
	float StartDashRestoreDelay;

	//타이머 함수(바운드 대쉬 시)
	FTimerHandle RestoreBoundDashTimerHandle;
	float BoundDashRestoreDelay;

	FTimerHandle RestoreSlowBlockTimerHandle;
	float SlowBlockRestoreDelay;

	//돌아오는 스피드 값
	float Target_Restore_Speed;

	//Shift키 봉인 타이머
	FTimerHandle RestoreCanShiftKeyHandle;
	float SpeedBlockRestoreShiftKey;
	float PowerfulmodeRestoreShiftKey;
	float StartDashRestoreShiftKey;
	float TiredRestoreShiftKey;
	float BoundDashRestoreShiftKey;
	float SlowBlockRestoreShiftKey;

	//회복 함수
	void RestoreSpeed();
	void SpeedBlockRestoreSpeed();
	void PowerfullModeRestore();
	void StartDashRestore();
	void BoundDashRestore();
	void SlowBlockRestore();

	//떨어지는지 확인하는 함수
	void FallingCheck();

	//분노모드 함수
	void StartPowerFullMode();

	//턴 함수
	void Turn();

	//슬라이드 함수
	void Slide();
	
	//슬라이드 스탑 함수
	void StopSlide();

	//무브먼트 회복
	FTimerHandle RestoreMovementHandle;
	float RestoreMovementDealy;

	//무브먼트 회복 함수
	void EnableMovement();

	//쉬프트 키 사용 가능 함수
	void EnableShiftKey();

	//상황에 따른 카메라 변경 함수
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

	//속도 장판 함수
	void SpeedBlock();
	bool IsSpeedBlock = false;
	bool SpeedBlockLandVal;

	//슬로우 블록 함수
	void SlowBlock();
	bool IsSlowBlock = false;
	bool SlowBlockLandVal;

	//파워풀 모드 함수
	void PowerFullMode();
	bool IsPowerful = false;
	
	//스타트 대쉬 함수
	void StartDash();
	bool IsStartDash = false;
	float FallingStartDash_Height;
	bool StartDashLandVal = false;
	

	//바운드 블록 대쉬 함수
	void BoundDash();
	bool IsBoundDash = false;
	float BoundStartDash_Height;
	bool BoundDashLandVal = false;

	//파괴 블록
	void DestroyBlock();

	//땅 아래 있는지 체크
	void CollisionDetection();

	//리스폰 함수
	void Respawn();

	//떨어졌는지 체크
	bool IsUnderGround;

	//지면 떨어졌을 시 리스폰 딜레이
	FTimerHandle RespawnTimerHandle;

	//체크 포인트 초기화
	FVector GetCheckLocation = FVector(2000.0f, 2000.0f, 0.0f);

	//공중 체크
	float PreviousHeight;
	float HeigthThreshHold;

	float SpeedBlockSpeed;
	float PowerfulSpeed;
	float StartDashSpeed;
	float BoundDashSpeed;
	float SlowBlockSpeed;

};