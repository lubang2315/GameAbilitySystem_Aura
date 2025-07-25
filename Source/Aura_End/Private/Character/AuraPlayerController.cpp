// 由来时路褒贬不一制作

#include "Character/AuraPlayerController.h"

#include "BlueprintEditor.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext)
	//使用ULocalPlayer里面的GetSubsystem节点获取UEnhancedInputLocalPlayerSubsystem类型的本地玩家控制器GetLocalPlayer()
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem)
	Subsystem->AddMappingContext(AuraContext,0);

	//在该模式下设置光标，在游戏开始时显示光标，样式为默认。
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//设置光标不会锁定到视口，同时当光标在视口上方，显示光标。
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//虚幻引擎默认启用的是输入组件，需要把输入组件转换成增强输入组件（增强输入组件是继承自输入组件的）
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//给增强输入绑定一个回调函数Move，当MoverAction有值时就会激发回调函数（ETriggerEvent::Triggered此参数是触发方式）
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	
}

void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	//分别获取二维值和旋转，然后新建一个旋转参数（俯视角镜头）
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

	//从旋转中分离出来x和Y轴5方向
	const FVector ForWardDirecction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//如果成功获得人物，把值返回给人物移动
	if (APawn* controlledPawn = GetPawn<APawn>())
	{
		controlledPawn->AddMovementInput(ForWardDirecction,InputAxisVector.Y);
		controlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
	
}
