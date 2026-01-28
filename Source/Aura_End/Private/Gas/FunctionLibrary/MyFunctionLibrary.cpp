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
#include "limits.h"

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

void UMyFunctionLibrary::InitializeDefaultAttribute(float Lever, ECharacterClass CharacterClass,UAbilitySystemComponent* EnemyASC, const UObject* WordContextObject)
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
	
	
}

void UMyFunctionLibrary::InitializeDefaultAttributeFromSaveData(const UObject* WorldContextObject,UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	/*获取标签*/
	FMyGameplayTags MyGameplayTags = FMyGameplayTags::Get();
	
	/*获取源对象*/
	const AActor* SourceActor = ASC->GetAvatarActor();

	/*从实例获取关卡角色配置*/
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	
	/*创建上下文句柄*/
	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceActor);
	
	/*创建GE规格句柄*/
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryAttributes_SetByCaller,1.f,EffectContextHandle);

	/*配置规格里面的具体参数*/
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Attributes_Primary_Strength,SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Attributes_Primary_Intelligence,SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Attributes_Primary_Resilience,SaveGame->Resistance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,MyGameplayTags.Attributes_Primary_Vigor,SaveGame->Vigor);

	/*应用属性*/
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	/*应用次要属性*/
	FGameplayEffectContextHandle SecondaryHandle = ASC->MakeEffectContext();
	SecondaryHandle.AddSourceObject(SourceActor);
	FGameplayEffectSpecHandle SecondaryAttribute = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes_Infinite,1.f,SecondaryHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttribute.Data.Get());

	/*应用血量和蓝量*/
	FGameplayEffectContextHandle VitalHandle = ASC->MakeEffectContext();
	VitalHandle.AddSourceObject(SourceActor);
	FGameplayEffectSpecHandle VitalAttribute = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,1.f,VitalHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttribute.Data.Get());
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
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, const FVector& SphereOrigin)
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

void UMyFunctionLibrary::GetClosestTargets(int32 MaxTarget, const FVector& Origin, const TArray<AActor*>& Actors,TArray<AActor*>& OutTargets)
{
	/*输入参数分别为：获取最大Actor数量，目标点位置，捕获范围内所有Actor，经过筛选的离目标点最近的Actor*/

	if (Actors.Num() <= MaxTarget)
	{
		OutTargets = Actors;
		return;
	}

	/*创建一个Actor本地数组*/
	TArray<AActor*> LocalActor = Actors;
	/*while循环累计*/
	int32 NumTargetFound = 0;

	/*直到找完MaxTarget个停止*/
	while (NumTargetFound < MaxTarget)
	{
		/*如果没有可遍历的退出*/
		if (LocalActor.Num() == 0) break;
		/*创建一个默认最远距离*/
		double ClosestDistance = TNumericLimits<double>::Max();
		/*缓存当前距离最近的Actor*/
		AActor* ClosestActor;
		for (AActor* Actor : LocalActor)
		{
			/*获取Actor与目标点距离*/
			const double Distance = (Actor->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
		/*从遍历数组中移除最近对象并添加到输出数组中*/
		LocalActor.Remove(ClosestActor);
		OutTargets.AddUnique(ClosestActor);
		/*递增数量*/
		++NumTargetFound;
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

	/*设置具有范围伤害功能GE的范围伤害的相关参数*/
	SetIsRadialDamage(EffectContextHandle,DamageEffectPrams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle,DamageEffectPrams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle,DamageEffectPrams.RadialDamageOuterRadius);
	SetRadialDamagetOrigin(EffectContextHandle,DamageEffectPrams.RadialDamageOrigin);

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

TArray<FRotator> UMyFunctionLibrary::EvenlySpacedRotators(const FVector& ForWard, const FVector& Axis, float Spread,int32 NumRotators)
{
	/*新建一个输出数组*/
	TArray<FRotator> Rotators;

	/*获取最左侧开始计算分段的初始角度*/
	const FVector LeftOfSpread = ForWard.RotateAngleAxis(-Spread / 2.f, Axis);

	/*根据分段不同决定是否进行均分*/
	if (NumRotators > 1)
	{
		/*根据均分段数获取均分角度*/
		const float DeltaSpread = Spread / NumRotators - 1.f;
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		/*如果只有一个均分那只返回正前方向即可*/
		Rotators.Add(ForWard.Rotation());
	}
	return Rotators;
}

TArray<FVector> UMyFunctionLibrary::EvenlySpacedVectors(const FVector& ForWard, const FVector& Axis, float Spread,int32 NumRotators)
{
	/*新建一个输出数组*/
	TArray<FVector> Vectors;

	/*获取最左侧开始计算分段的初始角度*/
	const FVector LeftOfSpread = ForWard.RotateAngleAxis(-Spread / 2.f, Axis);

	/*根据分段不同决定是否进行均分*/
	if (NumRotators > 1)
	{
		/*根据均分段数获取均分角度*/
		const float DeltaSpread = Spread / NumRotators - 1.f;
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		/*如果只有一个均分那只返回正前方向即可*/
		Vectors.Add(ForWard);
	}
	return Vectors;
}

bool UMyFunctionLibrary::GetIsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetIsRadialDamage();
	}
	return false;
}

float UMyFunctionLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetRadialDamagetInnerRadius();
	}
	return 0.f;
}

float UMyFunctionLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

FVector UMyFunctionLibrary::GetRadialDamagetOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* GEContextHandle = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return  GEContextHandle->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UMyFunctionLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UMyFunctionLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,float InRadialDamageInnerRadius)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetRadialDamagetInnerRadius(InRadialDamageInnerRadius);
	}
}

void UMyFunctionLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,float InRadialDamageOuterRadius)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UMyFunctionLibrary::SetRadialDamagetOrigin(FGameplayEffectContextHandle& EffectContextHandle,FVector InRadialDamagetOrigin)
{
	if (FAuraGameplayEffectContext* GEContextHandle = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		GEContextHandle->SetRadialDamageOrigin(InRadialDamagetOrigin);
	}
}

void UMyFunctionLibrary::SetIsRadialDamageEffectParams(FDamageEffectPrams& DamageEffectParams, bool bIsRadial,float InnerRadius, float OutRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OutRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UMyFunctionLibrary::SetKnockbackDirection(FDamageEffectPrams& DamageEffectParams, FVector KnockbackDirection,float Magnitude)
{
	KnockbackDirection.Normalize();
	if(Magnitude == 0.f)
	{
		DamageEffectParams.KnockBackForce = KnockbackDirection * DamageEffectParams.KnockBackMagnitude;
	}
	else
	{
		DamageEffectParams.KnockBackForce = KnockbackDirection * Magnitude;
	}
}

void UMyFunctionLibrary::SetDeathImpulseDirection(FDamageEffectPrams& DamageEffectParams, FVector ImpulseDirection,float Magnitude)
{
	ImpulseDirection.Normalize();
	if(Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UMyFunctionLibrary::SetEffectParamsTargetASC(FDamageEffectPrams& DamageEffectParams,UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

ULootTiers* UMyFunctionLibrary::GetLootTiers(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return nullptr;

	//返回敌人战利品配置，需要设置到GameMode上
	return  GameMode->LootTiers;
}

