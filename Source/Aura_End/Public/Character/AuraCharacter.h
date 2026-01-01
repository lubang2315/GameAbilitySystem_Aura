// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interface/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class AURA_END_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/*玩家接口*/
	/*增加XP*/
	virtual void AddToXP_Implementation(int32 InXP) override;
	/*获取当前经验值*/
	virtual int32 GetXP_Implementation() const override; 
	/*增加等级*/
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	/*升级*/
	virtual void LevelUp_Implementation() override;
	/*根据当前经验获取等级*/
	virtual int32 FindLevelForXP_Implementation(int32 XP) const override;
	/*增加属性点*/
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	/*获取属性点*/
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	/*增加技能点*/
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	/*获取技能点*/
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	/*获取可分配的属性点数*/
	virtual int32 GetAttributePoints_Implementation() const override;
	/*获取可分配的技能点数*/
	virtual int32 GetSpellPoints_Implementation() const override;
	/*End*/

	/*在C++里面写摄像机组件和弹簧臂，主要是可以方便获取旋转*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	/*End*/

	/*升级粒子特效*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagara;

	/*受到眩晕阻止玩家移动*/
	virtual void OnRep_Stunned() override;

	/*受到眩晕阻止玩家移动*/
	virtual void OnRep_Burn() override;

private:
	virtual void InitAbilityActorInfo() override;

	virtual int32 GetPlayerLevel_Implementation() override;

	/*在升级后播放升级特效，在每个客户端都调用这个函数*/
	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles() const;
	
};
