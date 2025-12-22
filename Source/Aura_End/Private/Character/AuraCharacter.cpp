// 由来时路褒贬不一制作


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/AuraPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gas/DataAsset/LevelUpInfo.h"
#include "Gas/Player/AuraPlayerState.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "UI/HUD/AuraHUDBase.h"
#include "NiagaraComponent.h"

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

	/*设置人物类型，因为敌人需要类型而我把类型写在基类了所以Aura随便定义一个类型。*/
	CharacterClass = ECharacterClass::Elementalist;

	/*设置相机*/
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent,USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	/*End*/

	/*设置升级粒子特效附着在骨骼上*/
	LevelUpNiagara = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagara");
	LevelUpNiagara->SetupAttachment(GetRootComponent());
	LevelUpNiagara->bAutoActivate = false;
	
}
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();/*添加并激活能力一次*/
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToXP(InXP);
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return  AuraPlayerState->GetPlayerXP();
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		/*等级提升更新技能解锁状态*/
		AuraASC->UpdateAbilityStatuses(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::LevelUp_Implementation()
{
	//TODO :角色升级时播放升级效果
	MulticastLevelUpParticles();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	/*因为上面那个levelUp函数只能服务器调用，所以这里创建一个调用后会同步调用客户端执行函数来播放升级特效*/
	if (IsValid(LevelUpNiagara))
	{
		/*把粒子播放方向与相机相对*/
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagara->GetComponentLocation();
		const FRotator NiagaraRotation = (NiagaraLocation - CameraLocation).Rotation();
		LevelUpNiagara->SetWorldRotation(NiagaraRotation);
		LevelUpNiagara->Activate(true);
	}
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 XP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return  AuraPlayerState->LevelUpInfo->FindLevelForXP(XP);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return  AuraPlayerState->LevelUpInfo->LevelUpInfo[Level].AttributePointAward;
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return  AuraPlayerState->LevelUpInfo->LevelUpInfo[Level].SpellPointAward;
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerSpellPoints();
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
	InitializePrimaryAttributes();
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
	
}







