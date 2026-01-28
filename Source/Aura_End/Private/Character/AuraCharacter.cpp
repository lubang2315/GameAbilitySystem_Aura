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
#include "Aura_End/AuraAbilityInfoLogChannels.h"
#include "Character/AuraGameModeBase.h"
#include "Gas/Debuff/DebuffNiagaraComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/AuraGameInstance.h"
#include "Tags/AuraGameplayTags.h"

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
	/*开始游戏加载存档*/
	LoadProgress();
	//AddCharacterAbilities();/*添加并激活能力一次*/

	/*加载存档中关于场景中Actor信息*/
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag)
{
	/*先获取游戏模式，在游戏模式中我们写好了关于存档相关函数*/
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		/*获取存档*/
		ULoadScreenSaveGame* SaveGame = AuraGameModeBase->RetrieveInGameSaveData();
		if (SaveGame == nullptr) return;

		UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
		check(AuraGameInstance);
		
		/*更新并保存出生点标签*/
		SaveGame->PlayerStartTag = CheckPointTag;

		/*切换地图时设置默认出生点标签*/
		AuraGameInstance->PlayerStartTag = CheckPointTag;

		/*更新并保存玩家状态*/
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveGame->Level = AuraPlayerState->GetPlayerLevel();
			SaveGame->XP = AuraPlayerState->GetPlayerXP();
			SaveGame->AttributePoint = AuraPlayerState->GetPlayerAttributePoints();
			SaveGame->SpellPoint = AuraPlayerState->GetPlayerSpellPoints();
		}
		/*更新并保存玩家主要属性*/
		SaveGame->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveGame->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveGame->Resistance = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveGame->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		/*告诉下次从此存档进入游戏而不是加载空白文档*/
		SaveGame->bFirstTimeLoadIn = false;

		/*保存角色技能相关*/
		/*判断是不是在服务器运行*/
		if (!HasAuthority()) return;

		/*获取AuraASC*/
		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		
		/*使用AuraASC中创建的ForEachAbility函数循环，获取所有可激活技能规格*/
		FForEachAbility SaveGameAbilityDelegate;

		/*在添加技能进存档里面的数组里面时先清空数组*/
		SaveGame->SaveAbilities.Empty();

		SaveGameAbilityDelegate.BindLambda([this,AuraASC,SaveGame](const FGameplayAbilitySpec& AbilitySpec)
		{
			/*获取技能标签，和技能信息*/
			FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = UMyFunctionLibrary::GetAbilityInfo(this);
			FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			/*创建技能结构体并填充参数保存*/
			FSaveAbility SavedAbility;
			SavedAbility.GameplayAbility = AuraAbilityInfo.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilityInputTag = AuraASC->GetInputTagFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = AuraASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityType = AuraAbilityInfo.AbilityTypeTag;

			/*添加进存档*/
			SaveGame->SaveAbilities.AddUnique(SavedAbility);
		});

		/*调用ForEachAbility来执行存储到存档*/
		AuraASC->FForEachAbility(SaveGameAbilityDelegate);
		
		/*保存数据*/
		AuraGameModeBase->SaveGameProgress(SaveGame);
		
	}
}

void AAuraCharacter::LoadProgress() const
{
	/*先获取游戏模式，在游戏模式中我们写好了关于存档相关函数*/
	if (const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		/*获取存档*/
		ULoadScreenSaveGame* SaveGame = AuraGameModeBase->RetrieveInGameSaveData();
		if (SaveGame == nullptr) return;
		
		/*判断是不是第一次加载存档，如果是第一次直接使用默认GE初始化属性，初始化角色技能*/
		if (SaveGame->bFirstTimeLoadIn)
		{
			InitializePrimaryAttributes();
			AddCharacterAbilities();
		}
		else
		{
			/*加载技能，在作者看来对于技能加载宜在GAS中进行*/
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				UE_LOG(LogAura, Warning, TEXT("2技能 %d 数量"), SaveGame->SaveAbilities.Num());
				AuraASC->AddCharacterAbilitiesFromSaveGameData(SaveGame);
			}
			
			/*从存档数据中获取数据并设置玩家状态*/
			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetXP(SaveGame->XP);
				AuraPlayerState->SetLevel(SaveGame->Level);
				AuraPlayerState->SetAttributePoints(SaveGame->AttributePoint);
				AuraPlayerState->SetSpellPoints(SaveGame->SpellPoint);
			}
			UMyFunctionLibrary::InitializeDefaultAttributeFromSaveData(this,AbilitySystemComponent,SaveGame);
		}
	}
		
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

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPC->ShowMagicCircle(DecalMaterial);
		AuraPC->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPC->HideMagicCircle();
		AuraPC->bShowMouseCursor = true;
	}
}



void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
		FGameplayTagContainer BlockRemove;
		BlockRemove.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockRemove.AddTag(GameplayTags.Player_Block_InputHold);
		BlockRemove.AddTag(GameplayTags.Player_Block_InputPresseed);
		BlockRemove.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockRemove);
			StunDebuffComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockRemove);
			StunDebuffComponent->Deactivate();
		}
	}
	
}

void AAuraCharacter::OnRep_Burn()
{
	if (bIsBurn)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
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
	/*后期我们加入了存档机制，进入行存档进入游戏我们需要判断需不需要加载存档数据还是新建空白文档所以在这里加载不合适了*/
	//InitializePrimaryAttributes();
	OnASCRegistered.Broadcast(AbilitySystemComponent);

	/*注册监听负面眩晕回调*/
	AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().Debuff_Stun,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&AAuraCharacter::StunTagChange);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
	
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	//创建一个委托，用于绑定委托回调
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
	{
		if(const AAuraGameModeBase* RPGGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			RPGGameMode->PlayerDied(this);
		}
	});

	//通过定时器触发对应的委托广播
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);

	//防止相机在玩家角色死亡后跟随移动，将相机固定在世界坐标位置
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}







