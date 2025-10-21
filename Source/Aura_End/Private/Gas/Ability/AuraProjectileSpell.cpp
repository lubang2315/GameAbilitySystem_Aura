// 由来时路褒贬不一制作


#include "Gas/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/Auraprojectile.h"
#include "Tags/AuraGameplayTags.h"
#include "Interface/CombotInterface.h"
/*激活技能*/
void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
/*初始化投射物的位置和角度，并发射投射物*/
void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag)
{   /*判断此函数是否在服务器运行*/
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	/*从CombatInterFace获取玩家位置，和武器插槽位置*/
	if (ICombotInterface* CombotInterface = Cast<ICombotInterface>(GetAvatarActorFromActorInfo()))
	{
        const FVector SocketLocation =  ICombotInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		//Rotation.Pitch = 0.f;
	
		/*Set投射物角度*/
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
        SpawnTransform.SetRotation(Rotation.Quaternion());
      
		
		AAuraprojectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraprojectile>(
			ProjectileSpellClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		        UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
        		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,GetAbilityLevel(),SourceASC->MakeEffectContext());

		        /*按调用者设置伤害该功能可以传入动态可变伤害*/

				const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();

				for (auto& Pair : DamageTypes)
				{
					const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Pair.Key,ScaledDamage);
				}
		
		       
		        
		
        		Projectile->DamageEffectHandle = SpecHandle;
		
        Projectile->FinishSpawning(SpawnTransform);
		
	}
}




