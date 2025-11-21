// 由来时路褒贬不一制作


#include "Gas/DataAsset/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		if (LevelUpInfo.Num() <= level) return level;
		if (XP >= LevelUpInfo[level].LevelUpRequirement)
		{
			++level; 
		}
		else
		{
			bSearching = false;
		}
	}
	return level;
}
