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
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	/*上面创建了Key，在计算伤害的时候还有属性抗性予以减免，这里我创建几个相应的属性减免标签*/
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;
	
	/*这里创建了一个数组，在设置完有调用者计算伤害时，存在多种属性传递，接收者是按照标签接受属性值，
	 * 以前的方法是一个个找到标签在读取属性值，这样比较麻烦，这里直接创建一个数组，把要读取的属性
	 * 类型值标签都加到一个数组里面然后遍历一下。后面又优化了因为还要读取属性抗性减免以后的伤害所
	 * 以这里不在使用数组而是Map映射，在这里方便学习把源代码留在注释中TArray<FGameplayTag> DamageTypes;
	 */
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistance;
	
	/*Enemy受击通知标签*/
	FGameplayTag Effects_HitReact;

	/*Enemy Attrack tag*/
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	/*Enemy蒙太奇攻击位置标签*/
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	/*攻击技能标签传递，用来等待蒙太奇播放到攻击时间点告诉GA释放技能*/
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
private:
	static FMyGameplayTags GameplayTags;
 
};

