// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AI/BTTask_IDLE.h"
#include <Global/Portfolio_GlobalCharacter.h>
#include <Game/AI/Portfolio_AIController.h>
#include <Game/AI/Portfolio_MonsterEnums.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"

EBTNodeResult::Type UBTTask_IDLE::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	FRandomStream RandomStream(FMath::Rand());
	// 1���� 5������ ������ ���� ����
	int32 RandomNumber = RandomStream.RandRange(1, 3);
	if (RandomNumber == 1)
	{
	    GetGlobalCharacter(OwnerComp)->SetAniState(AIState::IDLE);
	}
	if (RandomNumber == 2)
	{
		GetGlobalCharacter(OwnerComp)->SetAniState(AIState::IDLE2);
	}
	if (RandomNumber == 3)
	{
		GetGlobalCharacter(OwnerComp)->SetAniState(AIState::IDLE3);
	}

	//GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), nullptr);

	return EBTNodeResult::Type::InProgress;
}

void UBTTask_IDLE::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelataSeconds)
{
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

	Super::TickTask(OwnerComp, NodeMemory, DelataSeconds);


	if (2.0f <= GetStateTime(OwnerComp))
	{
		// ���������� ������ �̿��ϴ°�.
		FRandomStream Stream;
		// �˾Ƽ� �ð��� ���� ���ϴ� ���� �õ尪���� ��´�.
		Stream.GenerateNewSeed();

		int Value = Stream.RandRange(0, 1);

		// UE_LOG(LogTemp, Error, TEXT("%S(%u)> %d"), __FUNCTION__, __LINE__, Value);

		ResetStateTime(OwnerComp);

		//if (Value == 0)
		//{
		SetStateChange(OwnerComp, AIState::PATROL);
		return;
		//}

		// �׷� �����ұ�?
		// �׳� 5�ʰ� �ٽ� �������.
		// SetStateChange(OwnerComp, AIState::MOVE);
		// return;
	}

	AActor* ResultActor = GetTargetSearch(OwnerComp);

	if (nullptr != ResultActor)
	{
		GetBlackboardComponent(OwnerComp)->SetValueAsObject(TEXT("TargetActor"), ResultActor);
		SetStateChange(OwnerComp, AIState::MOVE);
		return;
	}

	//�ٽ��� �÷��̾ ã�Ƴ��°�.
	// GetGlobalCharacter(OwnerComp)->GetLevel()->;
	return;
}