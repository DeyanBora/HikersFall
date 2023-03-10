// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"

#include "MainCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

//Constructor
UMainCharacterAnimInstance::UMainCharacterAnimInstance()
{

	Owner = nullptr;
	PlayerCharacter = nullptr;

	bIsInAir = false;
	bIsAttacking = false;
	bIsRunning = false;
	bIsDodging = false;
	bIsStunned = false;
	bIsBlocking = false;
	bIsGrabbing = false;
	bIsGrabbed = false;
	bGrabHoldUp = false;
	bFullBody = false;

	Speed = 0.0f;
	AimYaw = 0.0f;
	AimRoll = 0.0f;
	AimPitch = 0.0f;
	L_IK = 0.0f;
	R_IK = 0.0f;
	IK_InterpSpeed = 0.0f;
	IK_DisplacementVariable = 0.0f;

	R_Rot = FRotator::ZeroRotator;
	L_Rot = FRotator::ZeroRotator;

}

//Init Function (BeginPlay had a problem about Call order. Dont use it if BeginPlay values have relation with update)
void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = TryGetPawnOwner();

	if (Owner && Owner->IsA(AMainCharacter::StaticClass()))
	{
		PlayerCharacter = Cast<AMainCharacter>(Owner);
	}
}

//Tick function
void UMainCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Main character animation owner is not set!!"));
		return;
	}

	if (Owner->IsA(AMainCharacter::StaticClass()))
	{
		
		PlayerCharacter = Cast<AMainCharacter>(Owner);

		if (PlayerCharacter)
		{
			
			//Here, GET means get it from character code.

			//Get Air Status
			bIsInAir = PlayerCharacter->GetMovementComponent()->IsFalling();

			//Get Speed
			Speed = Owner->GetVelocity().Size();

			//Get Camera position roll, pitch, yaw.
			FRotator Delta = PlayerCharacter->GetBaseAimRotation() - PlayerCharacter->GetActorRotation();
			Delta.Roll;
			Delta.Pitch;
			Delta.Yaw;

			//Get if player is running
			PlayerCharacter->IsRunning;

			//Get if animation is fullbody
			bFullBody = GetCurveValue(FName(TEXT("Fullbody"))) > 0.0f;

			//Get if is attacking
			bIsAttacking = PlayerCharacter->IsAttacking;

			//Get if its dodging
			bIsDodging = PlayerCharacter->IsDodging;

			//Get if the character is stunned
			bIsStunned = PlayerCharacter->IsStunned;

			//Get if character is blocking
			bIsBlocking = PlayerCharacter->IsBlocking;

			//Get if character is grabbed by enemy
			bIsGrabbed = PlayerCharacter->bIsGrabbed;
			//Get if character is grabbing enemy
			bIsGrabbing = PlayerCharacter->bIsGrabbing;

			//AnimNotify

			//If Player Character can perform mixed (non-fullbody animations)
			bIsFullBodyPossible = CalculateFullBodyPossible();
			bIsSpeeding = CalculateIsCharacterSpeeding();
		}

	}

}

//The function that calculates actual foot displacement position which used as a final function of all IK functions (SHOULD BE REWRITTEN ITS NOT WORKING PRECISE)
void UMainCharacterAnimInstance::FootDisplacement()
{
	bool bBlockingHitL = false;
	float DistanceL = 0.0f;

	bool bBlockingHitR = false;
	float DistanceR = 0.0f;

	float LineTraceDistanceL = 0.0f;
	FVector NormalL = FVector::ZeroVector;

	float LineTraceDistanceR = 0.0f;
	FVector NormalR = FVector::ZeroVector;

	float Distance = 0.0f;
	if (!Owner->GetMovementComponent()->IsFalling())
	{
		//get hit variables from feet
		bBlockingHitL = DisplacementFunction(FName(TEXT("DEF-foot_L")), DistanceL);

		bBlockingHitR = DisplacementFunction(FName(TEXT("DEF-foot_R")), DistanceR);

		if (bBlockingHitL || bBlockingHitR)
		{
			if (DistanceL >= DistanceR)
			{
				Distance = DistanceL;
			}
			else
			{
				DistanceR;
			}
			//Calculate Displacement variable
			IK_DisplacementVariable = UKismetMathLibrary::FInterpTo
			(
				IK_DisplacementVariable,
				((Distance - 98.0f) * -1),
				UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
				IK_InterpSpeed
			);

			LineTrace(FName(TEXT("DEF-foot_L")), LineTraceDistanceL, NormalL);

			//LEFT Line Trace Operations.

			FRotator RotL = FRotator
			(
				UKismetMathLibrary::Atan2(NormalL.Y, NormalL.Z),
				UKismetMathLibrary::Atan2(NormalL.X, NormalL.Z) * -1.0,
				0.0
			);

			L_Rot = UKismetMathLibrary::RInterpTo(L_Rot, RotL, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), IK_InterpSpeed);

			//Problematic values (185 i equal to character height, 45 should be half capsule length but it creates problems!!!)
			float LValue = FMath::Abs((LineTraceDistanceL - 185) / 45);
			L_IK = UKismetMathLibrary::FInterpTo(L_IK, LValue, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), IK_InterpSpeed);
			//Left Line Trace Operations are DONE

			LineTrace(FName(TEXT("DEF-foot_R")), LineTraceDistanceR, NormalR);
			//Right Line Trace Operations

			FRotator RotR = FRotator
			(
				UKismetMathLibrary::Atan2(NormalR.Y, NormalR.Z),
				UKismetMathLibrary::Atan2(NormalR.X, NormalR.Z) * -1.0,
				0.0
			);

			R_Rot = UKismetMathLibrary::RInterpTo(R_Rot, RotR, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), IK_InterpSpeed);

			//Problematic values (185 i equal to character height, 45 should be half capsule length but it creates problems!!!)
			float RValue = FMath::Abs((LineTraceDistanceR - 185) / 45);
			R_IK = UKismetMathLibrary::FInterpTo(R_IK, RValue, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), IK_InterpSpeed);
			//Right Line Trace Operations are DONE

		}


	}


}

//Foot Placement Calculation Function
bool UMainCharacterAnimInstance::DisplacementFunction(FName SocketName, float& Distance)
{
	return false;

	FVector SocketLocationFootName = PlayerCharacter->GetMesh()->GetSocketLocation(SocketName);

	FVector CapsuleVec = PlayerCharacter->GetActorLocation() + FVector(0.0, 0.0, PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());


	FVector StartTraceVec = FVector(SocketLocationFootName.X, SocketLocationFootName.Y, CapsuleVec.Z);
	FVector EndTraceVec = StartTraceVec - FVector(0.0, 0.0, 190.0);

	FHitResult OutHitResult;
	bool bHitResult = GetWorld()->LineTraceSingleByChannel(OutHitResult, StartTraceVec, EndTraceVec, ECC_Visibility);

	//Return results
	Distance = OutHitResult.Distance;
	return bHitResult;
}

//Foot Line Trace Function
bool UMainCharacterAnimInstance::LineTrace(FName SocketName, float& Distance, FVector& Normal)
{
	FVector SocketLocationFootName = PlayerCharacter->GetMesh()->GetSocketLocation(SocketName);

	FVector SocketLocationRoot = PlayerCharacter->GetMesh()->GetSocketLocation(FName(TEXT("root")));

	FVector LineTraceVector = FVector(SocketLocationFootName.X, SocketLocationFootName.Y, SocketLocationRoot.Z);
	FVector StartLineTraceVector = LineTraceVector + FVector(0.0, 0.0, 150.0);
	FVector EndLineTraceVector = LineTraceVector + FVector(0.0, 0.0, -120.0);
	FHitResult OutHitResult;
	bool bHitResult = GetWorld()->LineTraceSingleByChannel(OutHitResult, StartLineTraceVector, EndLineTraceVector, ECC_Visibility);

	if (bHitResult)
	{
		Normal = OutHitResult.Normal;
		Distance = OutHitResult.Distance;
		return OutHitResult.bBlockingHit;
	}
	else
	{
		Distance = 9999.0f;
		Normal = FVector::ZeroVector;
		return OutHitResult.bBlockingHit;
	}

}

//Calculate if speed is more than 0 (this function is created for FAST PATHing)
bool UMainCharacterAnimInstance::CalculateIsCharacterSpeeding()
{
	return Speed > 0.001;
}

// Calculates if Player Character can perform mixed (non-fullbody animations)
//Its Thread risky so i will add it to Update and call from there
bool UMainCharacterAnimInstance::CalculateFullBodyPossible()
{
	return ((Speed > 0.001) && !bFullBody);
}

