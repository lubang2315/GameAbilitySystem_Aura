// 由来时路褒贬不一制作

#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/Engine.h"
#include "GameplayEffectExtension.h"
#include "Aura_End/AuraAbilityInfoLogChannels.h"
#include "Character/AuraPlayerController.h"
#include "GameFramework/Character.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Interface/CombotInterface.h"
#include "Interface/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Tags/AuraGameplayTags.h"
#include "Tags/AuraGameplayTags.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	InitHP(50.f);
	InitMana(10.f);

	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	//FAttributeSignature StrengthDelegate;
	//StrengthDelegate.BindStatic(GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength,GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence,GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience,GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor,GetVigorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_Armor,GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_ArmorPenetration,GetArmorPenetratinonAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_BlockChance,GetblockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_CriticalHitChance,GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_CritiaclHitDamage,GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_CritialHitResistance,GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_HealthRegeneration,GetHealthRegerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondery_ManaRegeneration,GetManaRegerationAttribute);
	
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire,GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning,GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane,GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical,GetPhysicalResistanceAttribute);


	
}
/**声明要复制到客户端的属性，以及复制条件*/
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    /*主要属性*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Strength,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Intelligence,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Resilience,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Vigor,COND_None,REPNOTIFY_Always);
	/*次要属性*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,HP,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxHp,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,Armor,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArmorPenetratinon,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,blockChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitChance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitDamage,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,CriticalHitResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,HealthRegeration,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ManaRegeration,COND_None,REPNOTIFY_Always);

	/*Resistance Damage Types Attributes*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,FireResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,LightningResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,ArcaneResistance,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet,PhysicalResistance,COND_None,REPNOTIFY_Always);
	
	
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHpAttribute() && bFillHealth)
	{
		SetHP(GetMaxHp());
		bFillHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bFillMana)
	{
		SetMana(GetMaxMana());
		bFillMana = false;
	}
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f,GetMaxHp());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectPropreties Props;
	SetEffectProperties(Data, Props);
	
	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		SetHP(FMath::Clamp(GetHP(),0.f,GetMaxHp()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalValue = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalValue > 0.f)
		{
			const float NewHP = GetHP() - LocalValue;
			SetHP(FMath::Clamp(NewHP,0.f,GetMaxHp()));
			const bool IsDead = NewHP <=0;
			if (IsDead)
			{
				ICombotInterface* CombotInterface = Cast<ICombotInterface>(Props.TargetActor);
				if (CombotInterface)
				{
					CombotInterface->Die();
				}
				/*死亡时，发送经验事件*/
				SendXPEvent(Props);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMyGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);/*按标签激活技能*/
			}
			/*从自定义的函数中获取是否阻挡攻击和暴击*/
			const bool IsBlockHit = UMyFunctionLibrary::IsBlockHit(Props.EffectContextHandle);
			const bool IsCritiaclHit = UMyFunctionLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowFloatingText(Props,LocalValue,IsBlockHit,IsCritiaclHit);
		}
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalValue = GetIncomingXP();
		SetIncomingXP(0.f);
		
		/*先检测是不是玩家，然后将经验应用给自身*/
		if (Props.SourceActor->Implements<UPlayerInterface>() && Props.SourceActor->Implements<UCombotInterface>())
		{
			/*获取当前玩家当前等级和经验*/
			const int32 CurrentLevel = ICombotInterface::Execute_GetPlayerLevel(Props.SourceActor);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceActor);

			/*获取升级后的新等级*/
			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceActor,CurrentXP + LocalValue);
			const int32 NumLevelUps = NewLevel - CurrentLevel;

			if (NumLevelUps > 0)
			{
				for (int32 i = CurrentLevel; i < NewLevel; i++)
				{
					/*获取升级奖励的技能点和属性点*/
					const int32 RewardSpellPoints = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceActor,CurrentLevel);
					const int32 RewardAttributePoints  = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceActor,CurrentLevel);
                    
					/*提升等级，增加角色技能点和属性点*/
					IPlayerInterface::Execute_AddToSpellPoints(Props.SourceActor,RewardSpellPoints);
					IPlayerInterface::Execute_AddToAttributePoints(Props.SourceActor,RewardAttributePoints);
				}
				
				IPlayerInterface::Execute_LevelUp(Props.SourceActor);
				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceActor,NumLevelUps);

				/*每次升级都重置血量和蓝量*/
				bFillHealth = true;
				bFillMana = true;
				
			}
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter,LocalValue);
		}
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectPropreties& Props, float DamageValue,bool IsBlockHit,bool IsCriticalHit)
{
	
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		/*攻击发起者是玩家*/ 
		if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Props.SourceCharacter->Controller))
		{
			AuraPC->ShowDamageNumber(DamageValue,Props.TargetCharacter,IsBlockHit,IsCriticalHit);
		}
		/*攻击发起者是敌人*/
		if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Props.TargetCharacter->Controller))
		{
			AuraPC->ShowDamageNumber(DamageValue,Props.TargetCharacter,IsBlockHit,IsCriticalHit);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectPropreties& Props)
{
	int32 Level = 1;
	if (Props.TargetActor->Implements<UCombotInterface>())
	{
		/*获取敌人等级，获取敌人类型，获取经验值*/
		Level = ICombotInterface::Execute_GetPlayerLevel(Props.TargetActor);
		const ECharacterClass CharacterClass = ICombotInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UMyFunctionLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter,Level,CharacterClass);

		/*获取标签,和创建一个Payload因为发送标签在SendGameplayEventToActor函数中可以利用这个数据包传送XP值*/
		const FMyGameplayTags& GameplayTags= FMyGameplayTags::Get();
		FGameplayEventData EventData;
		EventData.EventTag = GameplayTags.Attributes_Meta_InComingXP;
		EventData.EventMagnitude = XPReward;

		/*通过标签激活然后GA那边收到就会在GA已经激活情况下在GA内部异步激活后续逻辑*/
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceActor,GameplayTags.Attributes_Meta_InComingXP,EventData);
	}
	
	
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectPropreties& Props)
{
	/**Source效果所有者，Target效果作用目标*/

	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(Props.SourceActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
	
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetActor);
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Strength,OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Intelligence,OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Resilience,OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Vigor,OldVigor);
}

void UAuraAttributeSet::OnRep_HP(const FGameplayAttributeData& OldHP) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,HP,OldHP);
}

void UAuraAttributeSet::OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxHp,OldMaxHp);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Mana,OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,MaxMana,OldMaxMana);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,Armor,OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetratinon(const FGameplayAttributeData& OldArmorPenetratinon) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArmorPenetratinon,OldArmorPenetratinon);
}

void UAuraAttributeSet::OnRep_blockChance(const FGameplayAttributeData& OldblockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,blockChance,OldblockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitChance,OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitDamage,OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,CriticalHitResistance,OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeration(const FGameplayAttributeData& OldHealthRegeration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,HealthRegeration,OldHealthRegeration);
}

void UAuraAttributeSet::OnRep_ManaRegeration(const FGameplayAttributeData& OldManaRegeration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ManaRegeration,OldManaRegeration);
}


/*Resistance Damage Types Attributes*/
inline void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,FireResistance,OldFireResistance);
}

inline void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,LightningResistance,OldLightningResistance);
}

inline void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,ArcaneResistance,OldArcaneResistance);
}

inline void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet,PhysicalResistance,OldPhysicalResistance);
}


