// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AI/BTTask_MOVE.h"
#include <Global/Portfolio_GlobalCharacter.h>
#include <Game/AI/Portfolio_AIController.h>
#include <Game/AI/Portfolio_MonsterEnums.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

// �ϴ� ��ġ ���� �ٱ����� �̵��ϸ�
// Return ���·� ��ȯ�ϴ� ���� 2���� ��Ȳ�� �����Ѵ�.
// 1. �׳� �÷��̾ �ʹ� �־����� ���� ���ư���.
// 2. �� �����ڸ�(������� ������ �ڽ��� ��ġ.)���� �ʹ� �ָ� ��������.


UBTTask_MOVE::UBTTask_MOVE()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MOVE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	GetGlobalCharacter(OwnerComp)->SetAniState(AIState::MOVE);

	UCharacterMovementComponent* MoveCom = Cast<UCharacterMovementComponent>(GetGlobalCharacter(OwnerComp)->GetMovementComponent());

	if (nullptr != MoveCom)
	{
		MoveCom->MaxWalkSpeed = 100.0f;
	}


	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);
	FVector TargetPos = TargetActor->GetActorLocation();

	UNavigationPath* PathPoint = PathFindNavPath(OwnerComp, TargetPos);
	GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("NavPath"), PathPoint);
	GetBlackboardComponent(OwnerComp)->SetValueAsVector(TEXT("LastPos"), TargetPos);

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_MOVE::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelataSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DelataSeconds);

	UObject* TargetObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (nullptr == TargetActor)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);
		SetStateChange(OwnerComp, AIState::RETURN);
		return;
	}

	bool Target = GetBlackboardComponent(OwnerComp)->GetValueAsBool(TEXT("Death"));

	if (true == IsDeathCheck(OwnerComp))
	{
		SetStateChange(OwnerComp, AIState::DEATH);
		return;
	}

	if (true == IsStunCheck(OwnerComp))
	{
		SetStateChange(OwnerComp, AIState::STUN);
		return;
	}

	//�׺�޽�
	// �ʹ� ���� Ƚ���� ���� ã�� ��츦 ���� ���� ������ ���� ����� �ִ�.
	// ���� �ʹ� ���� ã���� ���߿� 

	FVector ThisPos = GetGlobalCharacter(OwnerComp)->GetActorLocation();
	FVector TargetPos = TargetActor->GetActorLocation();
	FVector LastPos = TargetActor->GetActorLocation();
	FVector CheckLastPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("LastPos"));

	if (LastPos != CheckLastPos)
	{
		// �̶��� ������ �մϴ�.
		UNavigationPath* NewPath = PathFindNavPath(OwnerComp, LastPos);
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("NavPath"), NewPath);
	}

	// �̹� �����ϱ� ������ �׳� �̹� �����ϴ� �Ϳ��� ã�ƿ��� �ȴ�.
	UObject* NavObject = GetBlackboardComponent(OwnerComp)->GetValueAsObject(TEXT("NavPath"));
	UNavigationPath* NavPath = Cast<UNavigationPath>(NavObject);

	if (nullptr == NavPath)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);
		SetStateChange(OwnerComp, AIState::RETURN);
		return;
	}

	// ���� ����� ��ã�Ҵٴ� �̾߱Ⱑ �ȴ�.
	if (nullptr != NavPath && true == NavPath->PathPoints.IsEmpty())
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);
		SetStateChange(OwnerComp, AIState::RETURN);
		return;
	}

	if (nullptr != NavPath)
	{
		TargetPos = NavPath->PathPoints[1];
	}


	TargetPos.Z = 0.0f;
	ThisPos.Z = 0.0f;
	LastPos.Z = 0.0f;

	{
		FVector Dir = TargetPos - ThisPos;
		Dir.Normalize();

		FVector OtherForward = GetGlobalCharacter(OwnerComp)->GetActorForwardVector();
		OtherForward.Normalize();

		FVector Cross = FVector::CrossProduct(OtherForward, Dir);

		float Angle0 = Dir.Rotation().Yaw;
		float Angle1 = OtherForward.Rotation().Yaw;

		if (FMath::Abs(Angle0 - Angle1) >= 10.0f)
		{
			FRotator Rot = FRotator::MakeFromEuler({ 0, 0, Cross.Z * 500.0f * DelataSeconds });
			GetGlobalCharacter(OwnerComp)->AddActorWorldRotation(Rot);
		}
		else 
		{
			FRotator Rot = Dir.Rotation();
			GetGlobalCharacter(OwnerComp)->SetActorRotation(Rot);
		}
	}



	{
		FVector Dir = TargetPos - ThisPos;

		GetGlobalCharacter(OwnerComp)->AddMovementInput(Dir);
		//GetGlobalCharacter(OwnerComp)->SetActorRotation(Dir.Rotation());

		FVector OriginPos = GetBlackboardComponent(OwnerComp)->GetValueAsVector(TEXT("OriginPos"));
		float SearchRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("SearchRange"));
		float AttackRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("AttackRange"));

		OriginPos.Z = 0.0f;
		FVector OriginDir = OriginPos - ThisPos;
		
		if (OriginDir.Size() >= SearchRange * 1.5f)
		{
			//GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);
			SetStateChange(OwnerComp, AIState::RETURN);
			int a = 0;
			return;
		}
		

		FVector LastDir = LastPos - ThisPos;

		
		if (SearchRange < LastDir.Size())
		{
			//GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);
			SetStateChange(OwnerComp, AIState::RETURN);
			return;
		}
		

		// ����� �����ߴ�.
		if (AttackRange >= LastDir.Size())
		{
			SetStateChange(OwnerComp, AIState::ATTACK);
			return;
		}
	}
}
