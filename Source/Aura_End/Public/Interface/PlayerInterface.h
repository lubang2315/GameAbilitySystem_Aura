// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_END_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*增加经验*/
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	/*获取当前经验值*/
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	/*增加等级*/
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	/*升级*/
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	/*根据当前经验值获取等级*/
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 XP) const;

	/*增加属性点*/
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	/*获取属性点*/
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;

	/*增加技能点*/
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	/*获取技能点*/
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;

	/*获取可分配属性点数*/
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	
	/*获取可分配属性点数*/
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	/*显示魔法圈，并设置魔法圈贴画材质*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	/*隐藏魔法光圈*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
	
};
