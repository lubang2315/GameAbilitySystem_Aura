// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
};
