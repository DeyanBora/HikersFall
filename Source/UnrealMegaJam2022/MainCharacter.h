// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainCharacter.generated.h"



UCLASS()
class UNREALMEGAJAM2022_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	//Camera Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArmComp;

	//Player following camera
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UCameraComponent* CameraComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsAttacking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsJumping;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsRolling;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		bool IsRunning;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsDodging;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsLeftClicking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsRightClicking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool CanPickSomething;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsPickingSomething;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool Finisher;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsCharacterLockedOnEnemy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool SaveAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool DamagingBool;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsStunned;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsHoldingSpace;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool IsBlocking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		bool bIsClimbing;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsGrabbing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsGrabbed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsGrabbedUp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		float SpaceButtonHoldTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		float RunAcceleration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		float RunSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
		float WalkSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		float DodgeVelocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		float Health;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		float Rage;

	/* Handle to manage the timer */
	UPROPERTY()
		FTimerHandle DodgeTimerHandle;

	UPROPERTY()
		bool DodgeTimerBool;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action Movement")
		float BlockReleaseDuration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Action")
		int32 AttackCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* HitAnimMontage1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* HitAnimMontage2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* HitAnimMontage3;

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* HitAnimMontage4;*/

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* HangAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		UAnimMontage* ClimbLedgeWallAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CombatAnim")
		bool CanMove;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		bool bIsDown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		bool bIsFacingUp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		FVector MeshPelvisLoc;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		UAnimMontage* FallUpAnimMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		UAnimMontage* FallDownAnimMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FallAnim")
		UAnimMontage* GetupMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClimbAnim")
		FVector WallNormal;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClimbAnim")
		FVector WallLocation;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClimbAnim")
		FVector HeightWallLocation;

	//Widget Actions

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//MovementInputs
	UFUNCTION(BlueprintCallable)
		void MoveForward(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void MoveRight(float AxisValue);

	//Mouse Axis
	UFUNCTION(BlueprintCallable)
		void LookUp(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void LookRight(float AxisValue);

	//Run Action
	UFUNCTION(BlueprintCallable)
		void RunPressed();
	UFUNCTION(BlueprintCallable)
		void RunReleased();

	//Left Click Hit Action
	UFUNCTION(BlueprintCallable)
		void HitLeft();

	//Combo Save
	UFUNCTION(BlueprintCallable)
		void ComboAttackSave();

	//Dodge and Roll
	UFUNCTION(BlueprintCallable)
		void DodgeOrRollPressed();
	UFUNCTION(BlueprintCallable)
		void DodgeOrRollReleased();
	UFUNCTION(BlueprintCallable)
		void ResetDodgeTimer();
	UFUNCTION(BlueprintCallable)
		void DodgeReset();
	UFUNCTION(BlueprintCallable)
		void DodgeOrRollTick(float AxisValue);

	//Blocking (Shift Button)
	UFUNCTION(BlueprintCallable)
		void BlockingPressed();
	UFUNCTION(BlueprintCallable)
		void BlockingReleased();

	//Grab Enemy (E key)
	UFUNCTION(BlueprintCallable)
		void GrabEnemy();

	//Anim Notifies
	UFUNCTION(BlueprintCallable)
		void NotifyDamageIsOn();
	UFUNCTION(BlueprintCallable)
		void NotifyDamageIsOff();

	//Reset Combo
	UFUNCTION(BlueprintCallable)
		void ResetCombo();

	//PickupAction
	UFUNCTION(BlueprintCallable)
		void PickupItem();

	//Respawn
	UFUNCTION(BlueprintCallable)
		void Respawn();

	UFUNCTION(BlueprintCallable)
		void ResetAttackBool();

	UFUNCTION(BlueprintCallable)
		void DamagingOn();

	UFUNCTION(BlueprintCallable)
		void DamagingOff();

	UFUNCTION(BlueprintCallable)
		void CanMoveOff();

	UFUNCTION(BlueprintCallable)
		void CanMoveOn();

	//Fall functions
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool AfterFallFacingDirection();

	UFUNCTION(BlueprintCallable)
		void SetupGetupOrientation();

	UFUNCTION(BlueprintCallable)
		void CalculatePelvisMeshLocation();

	UFUNCTION(BlueprintCallable)
		void GettingUp();

	UFUNCTION(BlueprintCallable)
		void CachePose();

	UFUNCTION(BlueprintCallable)
		void SetupGetupMontage();

	UFUNCTION(BlueprintCallable)
		void EnableWalking();

	UFUNCTION(BlueprintCallable)
		void AnimNotify_Standing();

	/*UFUNCTION(BlueprintCallable)
		void ForwardClimbTrace();

	UFUNCTION(BlueprintCallable)
		void ForwardClimbHeightTrace();

	UFUNCTION(BlueprintCallable)
		void HangWall();

	UFUNCTION(BlueprintCallable)
		void ClimbUp();

	UFUNCTION(BlueprintCallable)
		void DropDown();/*/

};

