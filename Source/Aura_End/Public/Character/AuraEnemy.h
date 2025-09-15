// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "Interface/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_END_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()


public:
	//人物描边需要设置视觉碰撞阻挡，所以通过代码把碰撞设为阻挡
	AAuraEnemy();

	virtual void BeginPlay() override;

	/**敌人描边显示接口，来自于玩家控制器的光标事件*/
	virtual void HighLightActor();
	virtual void UnHighLightActor();

	virtual void InitAbilityActorInfo() override;

	virtual void InitializePrimaryAttributes() const override;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Default");
	int32 Level = 1;
    /*设置敌人属性*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Default");
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	virtual int32 GetPlayerLevel() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Widget")
	TObjectPtr<UWidgetComponent> HPWidget;

	UPROPERTY(BlueprintAssignable,Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnEnemyHPChangedEvent;

	UPROPERTY(BlueprintAssignable,Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnEnemyMaxHPChangedEvent;
};
