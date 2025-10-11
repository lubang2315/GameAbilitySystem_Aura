// 由来时路褒贬不一制作


#include "Gas/FunctionLibrary/MyFunctionLibrary.h"

#include "AuraAbilityTypes.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "Character/AuraGameModeBase.h"
#include "Interface/CombotInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUDBase.h"

UOverlayWidgetController* UMyFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
 if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
 {
	 if (AAuraHUDBase* HUD = Cast<AAuraHUDBase>(PC->GetHUD()))
	 {

	 	AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	 	UAbilitySystemComponent* ASC= PS->GetAbilitySystemComponent();
	    UAttributeSet* AS = PS->GetAttributeSet();
	 	const FWidgetControllerParams WCParams(PC,PS,ASC,AS);
	 	return HUD->GetOverlayWidgetController(WCParams);
	 }
 }
	return nullptr;
}

UAttributeMenuWidgetController* UMyFunctionLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		if (AAuraHUDBase* HUD = Cast<AAuraHUDBase>(PC->GetHUD()))
		{

			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC= PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WCParams(PC,PS,ASC,AS);
			return HUD->GetAttributeMenuWidgetController(WCParams);
		}
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

	ICombotInterface* CombotInterface = Cast<ICombotInterface>(EnemyASC->GetAvatarActor());
	int32 Lever = CombotInterface->GetPlayerLevel();

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

inline void UMyFunctionLibrary::SetIsBlockHit(FGameplayEffectContextHandle& EffectContextHandle, bool IsBlockHit)
{
	
	FAuraGameplayEffectContext* AuraGameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
	AuraGameplayEffectContext->SetBlockedHit(IsBlockHit);
	
}

inline void UMyFunctionLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,bool IsCriticalHitHit)
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
	if (FirstActor->ActorHasTag("Player"))
	{
		return !SecondActor->ActorHasTag("Player");
	}
	if (SecondActor->ActorHasTag("Enemy"))
	{
		return !FirstActor->ActorHasTag("Enemy");
	}
	return false;
}
