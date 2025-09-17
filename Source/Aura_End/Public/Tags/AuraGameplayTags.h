// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * GameplayTags 标签 单例模式
 * 内部包含原生的项目中使用的标签
 */

struct FMyGameplayTags
{
public:
	static const FMyGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();
	/**Primary Attribute*/
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/**Secondary Attribute*/
	FGameplayTag Attributes_Secondery_MaxHP;
	FGameplayTag Attributes_Secondery_MaxMana;
	FGameplayTag Attributes_Secondery_Armor;
	FGameplayTag Attributes_Secondery_ArmorPenetration;
	FGameplayTag Attributes_Secondery_BlockChance;
	FGameplayTag Attributes_Secondery_CriticalHitChance;
	FGameplayTag Attributes_Secondery_CritiaclHitDamage;
	FGameplayTag Attributes_Secondery_CritialHitResistance;
	FGameplayTag Attributes_Secondery_HealthRegeneration;
	FGameplayTag Attributes_Secondery_ManaRegeneration;

	/**InputAction Ability Tag*/
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	/*GE里面有个按调用者计算伤害，在这里创建标签来当Key应用伤害*/
	FGameplayTag Damage;

	/*Enemy受击通知标签*/
	FGameplayTag Effects_HitReact;
private:
	static FMyGameplayTags GameplayTags;
 
};

