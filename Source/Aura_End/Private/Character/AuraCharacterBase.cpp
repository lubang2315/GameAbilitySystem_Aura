// 由来时路褒贬不一制作


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//1.创建一个骨骼网格体实例
	//2.实例的插槽名称为WeaponHandSocket通过GetMesh（）附着在人物网格体下面
	//3.设置碰撞为没有碰撞
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::InitializePrimaryAttributes()
{
	ApplyEffectToTager(PrimaryAttribute,1.f);
	ApplyEffectToTager(SecondaryAttribute,1.f);
	ApplyEffectToTager(DefaultVitalAttribute,1.f);
}

void AAuraCharacterBase::ApplyEffectToTager(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Lever) const
{
	/*人物主要属性的值，采用GE模式赋值，以下是初始化主要属性值*/
 	check(AbilitySystemComponent);
 	check(PrimaryAttribute);
 	/*给人物应用GE*/
 	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);/*设置源对象是自己，以便在后续使用GE时可以直接从GE中获取作用目标*/
 	FGameplayEffectSpecHandle EffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Lever,ContextHandle);
 	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(),GetAbilitySystemComponent());
}



