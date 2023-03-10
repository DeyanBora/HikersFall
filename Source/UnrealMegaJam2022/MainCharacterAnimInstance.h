
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainCharacterAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class UNREALMEGAJAM2022_API UMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		class APawn* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		class AMainCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsDodging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsStunned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsGrabbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bIsGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bGrabHoldUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		bool bFullBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float AimYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float AimRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance")
		float AimPitch;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		float L_IK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		float R_IK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		float IK_InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		float IK_DisplacementVariable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		FRotator R_Rot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Instance IK")
		FRotator L_Rot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimGraph Variables")
		bool bIsFullBodyPossible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimGraph Variables")
		bool bIsSpeeding;

public:

	UMainCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
		void FootDisplacement();

	UFUNCTION()
		bool DisplacementFunction(FName SocketName, float& Distance);

	UFUNCTION()
		bool LineTrace(FName SocketName, float& Distance, FVector& Normal);

	UFUNCTION()
		bool CalculateIsCharacterSpeeding();

	// Just for knowledge ==> meta = (BlueprintThreadSafe)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Calculations")
		bool CalculateFullBodyPossible();

};
