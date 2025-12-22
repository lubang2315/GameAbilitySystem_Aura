// 由来时路褒贬不一制作


#include "Gas/FunctionLibrary/MyFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "Character/AuraGameModeBase.h"
#include "Interface/CombotInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/HUD/AuraHUDBase.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UMyFunctionLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams, AAuraHUDBase*& OutHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUDBase* HUD = Cast<AAuraHUDBase>(PC->GetHUD()))
		{
			OutHUD = HUD;
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC= PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
            
			OutWCParams.PlayerState = PS;
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;

			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UMyFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams OutWCParams;
	AAuraHUDBase* HUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,OutWCParams,HUD))
	{
		return HUD->GetOverlayWidgetController(OutWCParams);
	}	
		return nullptr;
}

UAttributeMenuWidgetController* UMyFunctionLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{

	FWidgetControllerParams OutWCParams;
	AAuraHUDBase* HUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,OutWCParams,HUD))
	{
		return HUD->GetAttributeMenuWidgetController(OutWCParams);
	}	
	return nullptr;
	
}

USpellMenuWidgetController* UMyFunctionLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams OutWCParams;
	AAuraHUDBase* HUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,OutWCParams,HUD))
	{
		return HUD->GetSpellMenuWidgetController(OutWCParams);
	}	
	return nullptr;
}

void UMyFunctionLibrary::InitializeDefaultAttribute(float Lever, ECharacterClass CharacterClass,
                                                    UAbilitySystemComponent* EnemyASC, const UObject* WordContextObject)
{
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WordContextObject));
	if (GameMode == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    AActor* AvactorActor = EnemyASC->GetAvatarActor();
	
	/*应用基础属性*/
	FGameplayEffectContextHandle PrimaryHandle = EnemyASC->MakeEffectContext();
	PrimaryHandle.AddSourceObject(AvactorActor);
	FGameplayEffectSpecHandle PrimaryAttribute = EnemyASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes,Lever,PrimaryHandle);
	EnemyASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttribute.Data.Get());

	/*应用次要属性*/
	FGameplayEffectContextHandle SecondaryHandle = EnemyASC->MakeEffectContext();
	SecondaryHandle.AddSourceObject(AvactorActor);
	FGameplayEffectSpecHandle SecondaryAttribute = EnemyASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,Lever,SecondaryHandle);
	EnemyASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttribute.Data.Get());

	/*应用血量和蓝量*/
	FGameplayEffectContextHandle VitalHandle = EnemyASC->MakeEffectContext();
	VitalHandle.AddSourceObject(AvactorActor);
	FGameplayEffectSpecHandle VitalAttribute = EnemyASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,Lever,VitalHandle);
	EnemyASC->ApplyGameplayEffectSpecToSelf(*VitalAttribute.Data.Get());

	GEngine->AddOnScreenDebugMessage(1,1,FColor::Black,FString::Printf(TEXT("HelloWord")));
	
}

void UMyFunctionLibrary::GiveStartupAbilities(const UObject* WordContextObject, UAbilitySystemComponent* EnemyASC,ECharacterClass CharacterClass)
{
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WordContextObject));
	if (GameMode == nullptr) return;
	
	UCharacterClassInfo* CharacterClassInfo = GameMode->CharacterClassInfo;/*从关卡中获取自己创建的数据资产，这是一种新的方式，以我现在的理解都是cast的*/

	/*应用Enemy共同能力*/
	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommitAbilities)
	{
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(AbilityClass,1);
		EnemyASC->GiveAbility(GASpec);
	}
	
	int32 Lever = 1;
	if (EnemyASC->GetAvatarActor()->Implements<UCombotInterface>())
	{
		Lever = ICombotInterface::Execute_GetPlayerLevel(EnemyASC->GetAvatarActor());
	}
	

	/*因为不同类型Enemy攻击技能不同所以与上述的共同能力区分，这里通过传入Enemy类型返回攻击技能*/
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	/*应用Enemy攻击技能*/
	for (const TSubclassOf<UGameplayAbility>& AttackAbility : ClassDefaultInfo.StartupAbilities)
	{
		FGameplayAbilitySpec GASpec = FGameplayAbilitySpec(AttackAbility,Lever);
		EnemyASC->GiveAbility(GASpec);
	}
}

UCharacterClassInfo* UMyFunctionLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;

	return GameMode->CharacterClassInfo;/*从关卡中获取自己创建的数据资产，这是一种新的方式，以我现在的理解都是cast的*/
	
}

UAbilityInfo* UMyFunctionLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (GameMode == nullptr) return nullptr;

	return GameMode->AbilityInfo;/*上面说的其实有点问题，从游戏模式获取，根本原因要知道为什么把数据创建在游戏模式里面，是应为游戏模式存在于服务器*/
}

bool UMyFunctionLibrary::IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  AuraGameplayEffectContext->IsblockedHit();
	}
	return false;
}	

bool UMyFunctionLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  AuraGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UMyFunctionLibrary::SetIsBlockHit(FGameplayEffectContextHandle& EffectContextHandle, bool IsBlockHit)
{
	
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	AuraGameplayEffectContext->SetBlockedHit(IsBlockHit);
	
}

void UMyFunctionLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,bool IsCriticalHitHit)
 {
 
 	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
 	AuraGameplayEffectContext->SetCriticalHit(IsCriticalHitHit);
 	
 }

void UMyFunctionLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, float Radius,
	TArray<AActor*>& OutOverlappingActors, TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	/*创建一个存储碰撞的Actor数组*/
	TArray<FOverlapResult> Overlaps;
	/*获取当前场景，获取不到的话打印Null*/
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		/*遍历碰撞的Actor*/
		for (FOverlapResult& OverlapResult : Overlaps)
		{
			/*判断当前Actor是否包含战斗接口*/
			const bool Implements = OverlapResult.GetActor()->Implements<UCombotInterface>();
			if (Implements && !ICombotInterface::Execute_IsDead(OverlapResult.GetActor()))
			{
				/*如果存活并且战斗接口加载成功把他添加到输出重叠数组中*/
				OutOverlappingActors.AddUnique(OverlapResult.GetActor());
			}
		}
	}
}

bool UMyFunctionLibrary::ISNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UMyFunctionLibrary::ApplyGameplayEffect(const FDamageEffectPrams& DamageEffectPrams)
{
	/*获取标签*/
	FMyGameplayTags MyGameplayTags = FMyGameplayTags::Get();
	
	/*获取源对象*/
	const AActor* SourceActor = DamageEffectPrams.SourceAbilitySystemComponent->GetAvatarActor();
	
	/*创建上下文句柄*/
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectPrams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceActor);

	/*设置致命一击方向*/
	SetDeathImpulse(EffectContextHandle,DamageEffectPrams.DeathImpulse);

	/*设置击飞方向*/
	SetKnockBackForce(EffectContextHandle,DamageEffectPrams.KnockBackForce);

	/*创建GE规格句柄*/
	FGameplayEffectSpecHandle SpecHandle = DamageEffectPrams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectPrams.DamageGameplayEffectClass,DamageEffectPrams.AbilityLevel,EffectContextHandle);

	/*配置规格里面的具体参数*/
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,DamageEffectPrams.DamageType,DamageEffectPrams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Debuff_Chance,DamageEffectPrams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Debuff_Damage,DamageEffectPrams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Debuff_Frequency,DamageEffectPrams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Debuff_Duration,DamageEffectPrams.DebuffDuration);

	/*讲GE应用给目标*/
	DamageEffectPrams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return EffectContextHandle;
	
}

int32 UMyFunctionLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, int32 Level,ECharacterClass CharacterClass)
{
	/*从世界上下文对象中获得游戏模式然后从游戏模式中获得数据资产*/
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo) return 0;

	FCharacterClassDefaultInfo CharacterInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	int32 XPReward = CharacterInfo.XPReward.GetValueAtLevel(Level);

	return  static_cast<float>(XPReward);
}

bool UMyFunctionLibrary::GetIsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetIsSuccessfulDebuff();
	}
	return false;
}

float UMyFunctionLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetDebuffDamage();
	}
	return 0.f;
}

float UMyFunctionLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetDebuffFrequency();
	}
	return 0.f;
}

float UMyFunctionLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetDebuffDuration();
	}
	return 0.f;
	
}
FGameplayTag UMyFunctionLibrary::GetDebuffType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (GEContextHandle->GetDamageType().IsValid())
		{
			return *GEContextHandle->GetDamageType();
		}
	}
	return FGameplayTag();
}

void UMyFunctionLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle,bool IsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetIsSuccessfulDebuff(IsSuccessfulDebuff);
	}
}

void UMyFunctionLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle,float DebuffDamage)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetDebuffDamage(DebuffDamage);
	}
}

void UMyFunctionLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle,float DebuffFrequency)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetDebuffFrequency(DebuffFrequency);
	}
}

void UMyFunctionLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle,float DebuffDuration)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetDebuffDuration(DebuffDuration);
	}
}

void UMyFunctionLibrary::SetDebuffType(FGameplayEffectContextHandle& EffectContextHandle,FGameplayTag& DamagetDebuffType)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(DamagetDebuffType);
		GEContextHandle->SetDamageType(DamageType);
	}
}

FVector UMyFunctionLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

void UMyFunctionLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,const FVector& DeathImpluse)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetDeathImpulse(DeathImpluse);
	}
}

FVector UMyFunctionLibrary::GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetKnockBackForce();
	}
	return FVector::ZeroVector;
}

void UMyFunctionLibrary::SetKnockBackForce(FGameplayEffectContextHandle& EffectContextHandle,const FVector& KnockBackForce)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetKonckBackForce(KnockBackForce);
	}
}

