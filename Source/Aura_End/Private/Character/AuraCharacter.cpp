// 由来时路褒贬不一制作


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/AuraPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gas/Player/AuraPlayerState.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "UI/HUD/AuraHUDBase.h"

AAuraCharacter::AAuraCharacter()
{
    //人物往朝向方向运动，我的理解就是动画蓝图会跟随人物移动方向播放动画
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
    //把运动约束到平面，俯视角游戏必做设置
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//就是视角不跟随控制器移动了
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
}
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}
void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

    /**需要在这里进行激活HUD初始化相关，注意目前没有获得PC所以要先获得PC，获得PC不仅仅是为了传入值要想与HUD通信也需要通过PC调用*/
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
	    if (AAuraHUDBase* HUD = Cast<AAuraHUDBase>(AuraPlayerController->GetHUD()))
	 {
		HUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
	 }
	}
}




