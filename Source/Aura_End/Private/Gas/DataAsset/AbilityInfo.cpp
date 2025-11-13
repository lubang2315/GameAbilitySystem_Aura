// 由来时路褒贬不一制作


#include "Gas/DataAsset/AbilityInfo.h"
#include "Aura_End/AuraAbilityInfoLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& Info : Abilities)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		/*如果获取不到相应数据打印错误信息*/
		UE_LOG(LogAura,Error,TEXT("无法通过技能标签[%s]在技能数据[%s]查找到对应的技能数据"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}
