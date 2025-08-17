// 由来时路褒贬不一制作
#include "Character/AuraEnemy.h"
#include "AbilitySystemComponent.h"
#include "Aura_End/Aura_End.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"


AAuraEnemy::AAuraEnemy()
{
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    //加载能力系统在敌人基类里
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    //能力相同复制规模不同于玩家不需要获取UI信息所以用Minimal即可
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

void AAuraEnemy::BeginPlay()
{
    Super::BeginPlay();
    InitAbilityActorInfo();
}

void AAuraEnemy::HighLightActor()
{
    GetMesh()->SetRenderCustomDepth(true);
    GetMesh()->SetCustomDepthStencilValue(Custom_Depth_CharacterStroke);
    Weapon->SetRenderCustomDepth(true);
    Weapon->SetCustomDepthStencilValue(Custom_Depth_CharacterStroke);
    
}

void AAuraEnemy::UnHighLightActor()
{
    GetMesh()->SetRenderCustomDepth(false);
    Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::InitAbilityActorInfo()
{
   //初始化能力系统
      AbilitySystemComponent->InitAbilityActorInfo(this,this);
      Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}
