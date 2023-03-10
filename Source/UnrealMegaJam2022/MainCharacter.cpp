
#include "MainCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"




// Sets default values
AMainCharacter::AMainCharacter()
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set Tags
	this->Tags.Add("TAG");


	//Setting Up Variables
	SpaceButtonHoldTime = 0.315f;
	DodgeTimerBool = true;
	DodgeVelocity = 1500.0f;
	BlockReleaseDuration = 0.333f;
	AttackCount = 0;
	WalkSpeed = 600.0f;
	RunSpeed = 1000.0f;

	Health = 100.0f;
	Rage = 0.0f;

	CanMove = true;
	IsDodging = false;
	IsRolling = false;

	bIsGrabbing = false;
	bIsGrabbed = false;
	bIsGrabbedUp = false;
	bIsClimbing = false;

	// Instantiating Camera Components
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	//Set the location and rotation of the Character Mesh Transform
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	// Attaching class Components to the default character's Skeletal Mesh Component.
	SpringArmComp->SetupAttachment(GetMesh());
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Setting class variables of the spring arm
	SpringArmComp->bUsePawnControlRotation = true;

	//Setting class variables of the Character movement component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->SetWorldLocationAndRotation(FVector(0.0f, 0.0f, 140.0f), FRotator(-20.0f, 0, 0.0f));


	//Construction Helpers to find Assets
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMontage1(TEXT("/Game/MainCharacter/Mesh/Anim/AnimMontages/old_AnimMontages/MainC_anim_rig_001_Punch1_1_Montage"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMontage2(TEXT("/Game/MainCharacter/Mesh/Anim/AnimMontages/old_AnimMontages/MainC_anim_rig_001_Punch1_2_Montage"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMontage3(TEXT("/Game/MainCharacter/Mesh/Anim/AnimMontages/old_AnimMontages/MainC_anim_rig_001_Punch1_3_Montage"));
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> AnimMontage4(TEXT("/Game/MainCharacter/Animations/Montages/Punch2_4_AnimMontage"));

	//Set Hit animations
	HitAnimMontage1 = AnimMontage1.Object;
	HitAnimMontage2 = AnimMontage2.Object;
	HitAnimMontage3 = AnimMontage3.Object;

	DamagingBool = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDown)
	{
		CalculatePelvisMeshLocation();
		GetCapsuleComponent()->SetWorldLocation(MeshPelvisLoc, false);
		GetCharacterMovement()->SetMovementMode(EMovementMode(MOVE_None), 0);

	}
	else
	{
		MeshPelvisLoc = GetMesh()->GetSocketLocation(FName(TEXT("pelvis"))) + FVector(0.0, 0.0, 97.0);
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AMainCharacter::LookRight);

	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Pressed, this, &AMainCharacter::RunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), EInputEvent::IE_Released, this, &AMainCharacter::RunReleased);

	PlayerInputComponent->BindAction(TEXT("HitLeft"), EInputEvent::IE_Pressed, this, &AMainCharacter::HitLeft);

	PlayerInputComponent->BindAction(TEXT("Blocking"), EInputEvent::IE_Pressed, this, &AMainCharacter::BlockingPressed);
	PlayerInputComponent->BindAction(TEXT("Blocking"), EInputEvent::IE_Released, this, &AMainCharacter::BlockingReleased);

	PlayerInputComponent->BindAction(TEXT("DodgeRoll"), EInputEvent::IE_Pressed, this, &AMainCharacter::DodgeOrRollPressed);
	PlayerInputComponent->BindAction(TEXT("DodgeRoll"), EInputEvent::IE_Released, this, &AMainCharacter::DodgeOrRollReleased);
	PlayerInputComponent->BindAxis(TEXT("DodgeRoll"), this, &AMainCharacter::DodgeOrRollTick);
}

void AMainCharacter::MoveForward(float AxisValue)
{
	if (!CanMove || IsDodging || IsRolling)
	{
		FRotator NewForwardRotator = FRotator(0, GetControlRotation().Yaw, 0);

		// Make Forward Vector from Rotator
		FVector CharacterRotationVector = UKismetMathLibrary::GetForwardVector(NewForwardRotator);

		//Add Movement Input
		AddMovementInput(CharacterRotationVector, AxisValue / 10);
		
	}
	else
	{
		//Make Rotator for Camera Forward Vector
		FRotator NewForwardRotator = FRotator(0, GetControlRotation().Yaw, 0);

		// Make Forward Vector from Rotator
		FVector CharacterRotationVector = UKismetMathLibrary::GetForwardVector(NewForwardRotator);

		//Add Movement Input
		AddMovementInput(CharacterRotationVector, AxisValue);

	}

}

void AMainCharacter::MoveRight(float AxisValue)
{
	if (!CanMove || IsDodging || IsRolling)
	{
		//Make Rotator for Camera Right Vector
		FRotator NewRightRotator = FRotator(0, GetControlRotation().Yaw, GetControlRotation().Roll);

		// Make Right Vector from Rotator
		FVector CharacterRotationVector = UKismetMathLibrary::GetRightVector(NewRightRotator);

		//Add Movement Input
		AddMovementInput(CharacterRotationVector, AxisValue / 10);
	}
	else
	{
		//Make Rotator for Camera Right Vector
		FRotator NewRightRotator = FRotator(0, GetControlRotation().Yaw, GetControlRotation().Roll);

		// Make Right Vector from Rotator
		FVector CharacterRotationVector = UKismetMathLibrary::GetRightVector(NewRightRotator);

		//Add Movement Input
		AddMovementInput(CharacterRotationVector, AxisValue);
	}

}

void AMainCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(-AxisValue);
}

void AMainCharacter::LookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AMainCharacter::RunPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	IsRunning = true;
}

void AMainCharacter::RunReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	IsRunning = false;
}


void AMainCharacter::HitLeft()
{
	if (bIsGrabbing)
	{

	}
	else
	{
		if (!IsRolling)
		{
			if (IsAttacking)
			{
				SaveAttack = true;
			}
			else
			{
				IsAttacking = true;

				switch (AttackCount)
				{
				case 0:
					AttackCount = 1;

					PlayAnimMontage(HitAnimMontage1);
					LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 1000, false, false);
					break;
				case 1:
					AttackCount = 2;
					PlayAnimMontage(HitAnimMontage2);
					LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 800, false, false);
					break;
				case 2:
					AttackCount = 0;
					PlayAnimMontage(HitAnimMontage3);
					LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 800, false, false);
					break;

			

				default:
					break;
				}
			}
		}
	}
}


void AMainCharacter::ComboAttackSave()
{
	if (SaveAttack)
	{
		SaveAttack = false;

		switch (AttackCount)
		{
		case 0:
			AttackCount = 1;
			PlayAnimMontage(HitAnimMontage1);
			LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 1000, false, false);
			break; 
		case 1:
			AttackCount = 2;
			PlayAnimMontage(HitAnimMontage2);
			LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 800, false, false);
			break;

		case 2:
			AttackCount = 3;
			PlayAnimMontage(HitAnimMontage3);
			LaunchCharacter(GetCapsuleComponent()->GetForwardVector() * 800, false, false);
			break;


		default:
			break;
		}
	}
}

void AMainCharacter::DodgeOrRollPressed()
{
	IsHoldingSpace = true;
	UKismetSystemLibrary::RetriggerableDelay(GetWorld(), SpaceButtonHoldTime,FLatentActionInfo());
	if (IsHoldingSpace)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		GetCharacterMovement()->MaxAcceleration = RunAcceleration;
		IsRolling = true;
	}
}

void AMainCharacter::DodgeOrRollReleased()
{
	float MaxTimerTime = 0.4f;
	IsHoldingSpace = false;
	if (!IsRolling && SpaceButtonHoldTime<= 0.310f && !IsAttacking)
		{
		GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, this, &AMainCharacter::ResetDodgeTimer, 0.4f, false);
		if (DodgeTimerBool)
		{
			DodgeTimerBool = false;
			IsDodging = true;
			FVector DodgeVector = GetCapsuleComponent()->GetForwardVector() * DodgeVelocity;
			LaunchCharacter(DodgeVector, false, false); */
			IsDodging = false;
		}
	}
}

void AMainCharacter::ResetDodgeTimer()
{
	DodgeTimerBool = true;
}

void AMainCharacter::DodgeReset()
{
	IsDodging = false;
}


void AMainCharacter::BlockingPressed()
{


	GetController()->SetIgnoreMoveInput(true);
	IsBlocking = true;
}

void AMainCharacter::BlockingReleased()
{

	UKismetSystemLibrary::Delay(GetWorld(), BlockReleaseDuration, FLatentActionInfo());
	GetController()->SetIgnoreMoveInput(false);
	IsBlocking = false;
}

void AMainCharacter::GrabEnemy()
{

}


void AMainCharacter::NotifyDamageIsOn()
{
	//Weapon Damage On
}


void AMainCharacter::NotifyDamageIsOff()
{
	//WeaponDamageOff
}

void AMainCharacter::ResetCombo()
{
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
}

void AMainCharacter::DodgeOrRollTick(float AxisValue)
{
	FName CurveName = FName(TEXT("RollCurve"));

	float MaxCurveValue = 1.3f;
	float MinCurveValue = 0.0f;

	float LerpValue = UKismetMathLibrary::Lerp(MinCurveValue, MaxCurveValue, GetMesh()->GetAnimInstance()->GetCurveValue(CurveName));
	float InputKeyDownValue = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputKeyTimeDown(FKey(FName(TEXT("Space Bar"))));

	AddMovementInput(GetCapsuleComponent()->GetForwardVector(), LerpValue*AxisValue, false);*/
}

void AMainCharacter::PickupItem()
{

}

void AMainCharacter::Respawn()
{

}

void AMainCharacter::ResetAttackBool()
{
	IsAttacking = false;
}

void AMainCharacter::DamagingOn()
{
	DamagingBool = true;
}

void AMainCharacter::DamagingOff()
{
	DamagingBool = false;
}

void AMainCharacter::CanMoveOff()
{
	CanMove = false;
}

void AMainCharacter::CanMoveOn()
{
	CanMove = true;
}

bool AMainCharacter::AfterFallFacingDirection()
{
	bool bIsFacingUpF = false;
	FRotator SocketRotation = GetMesh()->GetSocketRotation(FName(TEXT("pelvis")));
	FVector RightVector = UKismetMathLibrary::GetRightVector(SocketRotation);
	double DotProduct = FVector::DotProduct(RightVector, FVector(0.0, 0.0, 1.0));

	if (DotProduct >= 0)
	{
		bIsFacingUpF = true;
	}
	else
	{
		bIsFacingUpF = false;
	}


	return bIsFacingUpF;
}

void AMainCharacter::SetupGetupOrientation()
{
	FVector NeckSocketLocation = GetMesh()->GetSocketLocation(FName(TEXT("neck_01")));
	FVector PelvisSocketLocation = GetMesh()->GetSocketLocation(FName(TEXT("pelvis")));
	FVector SomeVector = FVector(0.0, 0.0, 0.0);
	if (bIsFacingUp)
	{
		SomeVector = PelvisSocketLocation - NeckSocketLocation;
	}
	else
	{
		SomeVector = NeckSocketLocation - PelvisSocketLocation;
	}
	FRotator ActorRot = UKismetMathLibrary::MakeRotFromZX(FVector(0.0, 0.0, 1.0), SomeVector);
	FTransform PlayerTransform = FTransform(ActorRot, GetActorLocation(), FVector(1.0, 1.0, 1.0));
	SetActorTransform(PlayerTransform, false);
}

void AMainCharacter::CalculatePelvisMeshLocation()
{
	FVector SocLoc = GetMesh()->GetSocketLocation(FName(TEXT("pelvis")));

	FVector FinalLoc = SocLoc + FVector(0.0, 0.0, -1.0) * 100;
	FHitResult HitRes = FHitResult();
	FCollisionQueryParams TraceParams;
	GetWorld()->LineTraceSingleByChannel(HitRes, SocLoc, FinalLoc, ECC_Visibility, TraceParams);
	FVector CapsuleLoc;
	if (HitRes.bBlockingHit)
	{
		CapsuleLoc = HitRes.Location + FVector(0.0, 0.0, 97.0);
	}
	else
	{
		CapsuleLoc = GetMesh()->GetSocketLocation(FName(TEXT("pelvis")));
	}

	MeshPelvisLoc = UKismetMathLibrary::VInterpTo(MeshPelvisLoc, CapsuleLoc, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 5.0);

	return;
}


void AMainCharacter::GettingUp()
{
	CachePose();

	UKismetSystemLibrary::Delay(GetWorld(), 2.0, FLatentActionInfo());

	GetMesh()->SetSimulatePhysics(false);
	bIsDown = false;
	GetMesh()->GetAnimInstance()->Montage_Play(GetupMontage, 1.0);

}

void AMainCharacter::CachePose()
{
	bIsFacingUp = AfterFallFacingDirection();
	SetupGetupOrientation();
	SetupGetupMontage();
	UKismetSystemLibrary::Delay(GetWorld(), 0, FLatentActionInfo());
	UKismetSystemLibrary::Delay(GetWorld(), 0, FLatentActionInfo());
	GetMesh()->GetAnimInstance()->SavePoseSnapshot(FName(TEXT("finalpose")));
}

void AMainCharacter::SetupGetupMontage()
{

	if (bIsFacingUp)
	{
		GetupMontage = FallUpAnimMontage;
	}
	else
	{
		GetupMontage = FallDownAnimMontage;
	}
}
void AMainCharacter::EnableWalking()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode(MOVE_Walking), 0);

	GetMesh()->GetAnimInstance()->Montage_Stop(0.25);
}

void AMainCharacter::AnimNotify_Standing()
{
	EnableWalking();
}

/*
//This is Tick usage function (Climb is used for Enemy Actors)
void AMainCharacter::ForwardClimbTrace()
{
	float TraceRadius = 10;
	TArray<AActor*, FDefaultAllocator> EmptyArray;
	FVector ForwardTraceEnd = GetActorLocation() + GetActorForwardVector() * 150;
	FHitResult OutHitResult;

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), ForwardTraceEnd, TraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, EmptyArray, EDrawDebugTrace::None, OutHitResult, true);

	WallNormal = OutHitResult.Normal;
	WallLocation = OutHitResult.Location;


}
/*
void AMainCharacter::ForwardClimbHeightTrace()
{

	float TraceRadius = 10;
	TArray<AActor*, FDefaultAllocator> EmptyArray;
	FVector ForwardTraceStart = (GetActorForwardVector() * 75) + (GetActorLocation() + 500);
	FVector ForwardTraceEnd = (GetActorForwardVector() * 75) + GetActorLocation();
	FHitResult OutHitResult;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), ForwardTraceStart, ForwardTraceEnd, TraceRadius, ETraceTypeQuery::TraceTypeQuery1, false, EmptyArray, EDrawDebugTrace::None, OutHitResult, true);

	if (bHit)
	{
		HeightWallLocation = OutHitResult.Location;
		float TheZ = GetMesh()->GetSocketLocation(FName(TEXT("pelvisSocket"))).Z - OutHitResult.Location.Z;
		bool inRange = UKismetMathLibrary::InRange_FloatFloat(TheZ, -50.0, 0.0, true, true);

		if (inRange)
		{
			if (!bIsClimbing)
			{
				GetCharacterMovement()->SetMovementMode(MOVE_Flying);
				GetCharacterMovement()->StopMovementImmediately();
				HangWall();
			}
		}

	}
}*/
/*
//IMPORTANT!!! This ACTION is a FULLBODY ANIMATION!!!! Any problem can affect AnimInstance and AnimBlueprint of this character.
void AMainCharacter::HangWall()
{
	//PlayRate is 0 because climb starts right start of the animation
	PlayAnimMontage(HangAnimation, 0);
	//Might create error on montage since we might not give it a constant object (lessl likely)
	GetMesh()->GetAnimInstance()->Montage_Pause(HangAnimation);

	bIsClimbing = true;

	//Tweak 10 (22)
	FVector XYVector = (WallNormal * 10) + WallLocation;
	//Can be Tweakable (87)
	float HalfHeight = 96.0f;
	FVector ClimbLoc = FVector(XYVector.X, XYVector.Y, HeightWallLocation.Z - HalfHeight);
	FRotator WallRotation = UKismetMathLibrary::MakeRotFromX(WallNormal * -1);
	FLatentActionInfo LatentInfo;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), ClimbLoc, WallRotation, true, true, 0.2f, false, EMoveComponentAction::Move, LatentInfo);
	
}*/
/*
void AMainCharacter::ClimbUp()
{
	//Do Once in Action Mapping
	PlayAnimMontage(HangAnimation, 1.0f);
	GetMesh()->GetAnimInstance()->Montage_Play(HangAnimation, 1.0f, EMontagePlayReturnType::Duration, 0.0f);
	//Notify Happens 
	bIsClimbing = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);


	FVector CompVector = (GetCapsuleComponent()->GetComponentLocation()) + (GetActorForwardVector() * 50.0f);
	FRotator CapsuleRot = UKismetMathLibrary::MakeRotFromX(GetActorForwardVector());
	FLatentActionInfo LatentInfo;

	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), CompVector, CapsuleRot, true, true, 0.2f, false, EMoveComponentAction::Move, LatentInfo);

}

void AMainCharacter::DropDown()
{
	bIsClimbing = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	StopAnimMontage(HangAnimation);
}

*/
