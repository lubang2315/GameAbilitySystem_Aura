// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Gas/DataAsset/LootTiers.h"
#include "SaveGame/LoadScreenSaveGame.h"
#include "AuraGameModeBase.generated.h"

class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_END_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Default Info")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityInfo")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	/*创建新存档*/
	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot/*需要保存的视图模型示例*/,int32 SlotIndex/*存档索引*/);

	/*获取保存的存档*/
	ULoadScreenSaveGame* GetLoadScreenSaveGame(const FString& SaveSlotName/*存档名称*/,int32 SlotIndex/*存档索引*/) const;

	/*存档使用的数据结构类*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	/*删除相应的游戏存档*/
	static  void DelegateSlotData(const FString& SaveSlotName/*存档名称*/,int32 SlotIndex/*存档索引*/);

	/*默认地图名称*/
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	/*默认起始地图*/
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	/*所有可存档地图*/
	UPROPERTY(EditDefaultsOnly)
	TMap<FString,TSoftObjectPtr<UWorld>> Maps;

	/*根据地图资产名称获取地图名称,在此作用主要是显示存档页面中地图名称*/
	FString GatMapNameFromMapAssetName(const FString& MapAssetName) const;

	/*玩家出生点，按标签存储，注意不能存在空格*/
	UPROPERTY(EditDefaultsOnly)
	FName PlayerStartTag;

	/*根据传入参数进行关卡切换*/
	void TravelToMap(UMVVM_LoadSlot* Slot);

	/*覆写父类GameModeBase中的选择玩家出生点，用于可以根据标签选择玩家出生点*/
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/*获取到当前游戏进行中所使用的存档数据*/
	ULoadScreenSaveGame* RetrieveInGameSaveData() const;

	/*保存游戏的进度*/
	void SaveGameProgress(ULoadScreenSaveGame* SaveGameData) const;

	/*保存关卡中的状态到存档中*/
	void SaveWorldState(UWorld* World,const FString& DestinationMapAssetName = FString("")) const;

	/*从存档中加载当前关卡信息*/
	void LoadWorldState(UWorld* World) const;

	/*角色死亡调用此函数*/
	void PlayerDied(const ACharacter* DeadCharater) const;

	/*战利品数据配置*/
	UPROPERTY(EditDefaultsOnly, Category="Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;
	
protected:
	virtual void BeginPlay() override;
	
};
