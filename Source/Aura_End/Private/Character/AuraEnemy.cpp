// 由来时路褒贬不一制作
#include "Character/AuraEnemy.h"
#include "AbilitySystemComponent.h"
#include "Aura_End/Aura_End.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "Tags/AuraGameplayTags.h"
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
    /*初始化角色技能*/
    if (HasAuthority())
    {
         UMyFunctionLibrary::GiveStartupAbilities(this,AbilitySystemComponent);
    }
   
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
    if (HasAuthority())
    {
    InitializePrimaryAttributes();
    }

   if (UAuraUserWidget* AuraUserWidget = Cast<UAuraUserWidget>(HPWidget->GetUserWidgetObject()))
   {
       AuraUserWidget->SetWidgetController(this);
   }
    UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(AttributeSet);
    
    
   if ( AuraAttributes)
   {
       /*广播Enemy的血条*/
       AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
       (AuraAttributes->GetHPAttribute()).AddLambda
       ([this](const FOnAttributeChangeData& Data)
       {
           OnEnemyHPChangedEvent.Broadcast(Data.NewValue);
       });
       
       AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
       (AuraAttributes->GetMaxHpAttribute()).AddLambda
       ([this](const FOnAttributeChangeData& Data)
       {
           OnEnemyMaxHPChangedEvent.Broadcast(Data.NewValue);
       });
       
       AbilitySystemComponent->RegisterGameplayTagEvent
       (FMyGameplayTags::Get().Effects_HitReact,EGameplayTagEventType::NewOrRemoved).AddUObject
       (this,&AAuraEnemy::HitReactTagChanged);
   }
    /*在广播HP变化前进行初始化，这里是按照作者写的，但是我会报错在客户端敌人MaxHP显示为0，经测试是客户端的ASC没准备好，接收不了数据，
     *而通过GE应用的MaxHP，因为不是像HP那样经常变化，而我设置的敌人同步模式是Minimal,只有属性变化才会同步，所以即使打包给客户端，
     *客户端没有准备好接受，但服务器默认同步完成，所以这里我延迟一帧等待加载，就可以解决问题了
     *OnEnemyHPChangedEvent.Broadcast(AuraAttributes->GetHP());
     *OnEnemyMaxHPChangedEvent.Broadcast(AuraAttributes->GetMaxHp());*/
    
    /*这里是我自己增加延迟一帧初始化*/
    FTimerHandle TimerHandle_InitBroadcast;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_InitBroadcast,
        [this, AuraAttributes]()
        {
            OnEnemyHPChangedEvent.Broadcast(AuraAttributes->GetHP());
            OnEnemyMaxHPChangedEvent.Broadcast(AuraAttributes->GetMaxHp());
        },
        0.05f, // 延迟一个 tick，让 AttributeSet 与 GE 应用完成
        false
    );
    /*end*/
}

void AAuraEnemy::InitializePrimaryAttributes() const
{
   UMyFunctionLibrary::InitializeDefaultAttribute(Level,CharacterClass,AbilitySystemComponent,this);
}

void AAuraEnemy::Die()
{
    SetLifeSpan(LifeSpan);
    Super::Die();
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0 : BaseWalkSpeed;
}

int32 AAuraEnemy::GetPlayerLevel()
{
    return Level;
}
