// 由来时路褒贬不一制作
#include "Character/AuraEnemy.h"
#include "AbilitySystemComponent.h"
#include "Aura_End/Aura_End.h"
#include "Components/WidgetComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "UI/Widget/AuraUserWidget.h"


AAuraEnemy::AAuraEnemy()
{
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    //加载能力系统在敌人基类里
    AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    //能力相同复制规模不同于玩家不需要获取UI信息所以用Minimal即可
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
    AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

    HPWidget = CreateDefaultSubobject<UWidgetComponent>("Widget");
    HPWidget->SetupAttachment(GetRootComponent());
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

      InitializePrimaryAttributes();

   if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HPWidget->GetUserWidgetObject()))
   {
       AuraUserWidget->SetWidgetController(this);
   }
    UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(AttributeSet);
   if ( AuraAttributes)
   {
       /*广播Enemy的血条*/
       AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetHPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnEnemyHPChangedEvent.Broadcast(Data.NewValue);});
       AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributes->GetMaxHpAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnEnemyMaxHPChangedEvent.Broadcast(Data.NewValue);});
   }
    /*在广播HP变化前进行初始化*/
    OnEnemyHPChangedEvent.Broadcast(AuraAttributes->GetHP());
    OnEnemyMaxHPChangedEvent.Broadcast(AuraAttributes->GetMaxHp());
}

void AAuraEnemy::InitializePrimaryAttributes() const
{
   UMyFunctionLibrary::InitializeDefaultAttribute(Level,CharacterClass,AbilitySystemComponent,this);
}

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}
