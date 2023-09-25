// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AI/BTTask_AIBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UBTTask_AIBase::UBTTask_AIBase()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;


}

void UBTTask_AIBase::OnGameplayTaskActivated(class UGameplayTask&)
{

}

float UBTTask_AIBase::GetStateTime(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return 0.0f;
	}

	float StateTime = BlockBoard->GetValueAsFloat(TEXT("StateTime"));

	return StateTime;
}

AIState UBTTask_AIBase::GetAiState(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return AIState::IDLE;
	}

	uint8 Enum = BlockBoard->GetValueAsEnum(TEXT("AIState"));

	return static_cast<AIState>(Enum);

}

void UBTTask_AIBase::ResetStateTime(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	BlockBoard->SetValueAsFloat(TEXT("StateTime"), 0.0f);
}

APortfolio_GlobalCharacter* UBTTask_AIBase::GetGlobalCharacter(UBehaviorTreeComponent& OwnerComp)
{
	APortfolio_AIController* AiCon = OwnerComp.GetOwner<APortfolio_AIController>();
	if (nullptr == AiCon || false == AiCon->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == Chracter || false == Chracter->IsValidLowLevel())"), __FUNCTION__, __LINE__);
		// ���������� ���� ������ 
		return nullptr;
	}

	// ��Ʈ�ѷ����� ���� �����ϴ� Pawn�� GetPawn�� ����Ѵ�.
	APortfolio_GlobalCharacter* Character = AiCon->GetPawn<APortfolio_GlobalCharacter>();
	//PrePos = Character->GetActorLocation();
	//A++;
	//BTPrePos();

	if (nullptr == Character || false == Character->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == Chracter || false == Chracter->IsValidLowLevel())"), __FUNCTION__, __LINE__);
		// ���������� ���� ������ 
		return nullptr;
	}

	return Character;
}

UBlackboardComponent* UBTTask_AIBase::GetBlackboardComponent(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return nullptr;
	}

	return BlockBoard;
}

EBTNodeResult::Type UBTTask_AIBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type::InProgress;
}

void UBTTask_AIBase::SetStateChange(UBehaviorTreeComponent& OwnerComp, uint8 _State)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	BlockBoard->SetValueAsEnum(TEXT("AIState"), _State);

	ResetStateTime(OwnerComp);

	FinishLatentTask(OwnerComp, EBTNodeResult::Type::Failed);
}


void UBTTask_AIBase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelataSeconds)
{
	UBlackboardComponent* BlockBoard = OwnerComp.GetBlackboardComponent();

	if (nullptr == BlockBoard)
	{
		UE_LOG(LogTemp, Error, TEXT("%S(%u)> if (nullptr == BlockBoard)"), __FUNCTION__, __LINE__);
		return;
	}

	float StateTime = BlockBoard->GetValueAsFloat(TEXT("StateTime"));
	StateTime += DelataSeconds;
	BlockBoard->SetValueAsFloat(TEXT("StateTime"), StateTime);
}




class AActor* UBTTask_AIBase::GetTargetSearch(UBehaviorTreeComponent& OwnerComp)
{
	FString TargetTag = GetBlackboardComponent(OwnerComp)->GetValueAsString(TEXT("TargetTag"));
	TArray<AActor*> TargetActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *TargetTag, TargetActors);

	APortfolio_GlobalCharacter* Pawn = GetGlobalCharacter(OwnerComp);

	float SearchRange = GetBlackboardComponent(OwnerComp)->GetValueAsFloat(TEXT("SearchRange"));


	AActor* ResultActor = nullptr;

	if (0 != TargetActors.Num())
	{
		float Range = TNumericLimits<float>::Max();

		for (size_t i = 0; i < TargetActors.Num(); i++)
		{
			float Dis = (Pawn->GetActorLocation() - TargetActors[i]->GetActorLocation()).Size();

			if (SearchRange <= Dis)
			{
				continue;
			}

			if (Range > Dis)
			{
				Range = Dis;
				ResultActor = TargetActors[i];
			}
		}
	}

	return ResultActor;
}

bool UBTTask_AIBase::IsDeathCheck(UBehaviorTreeComponent& OwnerComp)
{
	if (0 >= GetGlobalCharacter(OwnerComp)->GetHP())
	{
		SetStateChange(OwnerComp, AIState::DEATH);
		return true;
	}

	return false;
}

bool UBTTask_AIBase::IsAttackCheck(UBehaviorTreeComponent& OwnerComp)
{
	if (1000 > GetGlobalCharacter(OwnerComp)->GetHP())
	{
		SetStateChange(OwnerComp, AIState::MOVE);
		return true;
	}

	return false;
}

bool UBTTask_AIBase::IsStunCheck(UBehaviorTreeComponent& OwnerComp)
{
	int Att = 0;
	Att += GetGlobalCharacter(OwnerComp)->GetATT();
	if (true == GetGlobalCharacter(OwnerComp)->GetBoneDamageCheck() && 250 <= Att)
	{
		SetStateChange(OwnerComp, AIState::STUN);
		GetGlobalCharacter(OwnerComp)->SetBoneDamageCheck(false);
		return true;
	}
	return false;
}

TArray<FVector> UBTTask_AIBase::PathFind(UBehaviorTreeComponent& _OwnerComp, AActor* _Actor)
{
	return PathFind(_OwnerComp, _Actor->GetActorLocation());
}

TArray<FVector> UBTTask_AIBase::PathFind(UBehaviorTreeComponent& _OwnerComp, FVector _EndPos)
{
	// ã�Ƴ��� ��θ� TArray<FVector> �׳� �̷� �����ͷ� ������ �ʴ´�.

	// ã�Ƴ��� ��ΰ� ��ȿ�Ѱ�?
	UNavigationPath* PathObject = nullptr;
	FVector StartPos = GetGlobalCharacter(_OwnerComp)->GetActorLocation();
	FVector EndPos = _EndPos;
	// FindPathAsync <= �ٷ� ����� �ȳ����� �Լ� �����͸� �־��ָ� ����� ������ �˷��ٰ�.
	// Sync , 

	PathObject = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		StartPos,
		EndPos
	);

	if (nullptr == PathObject)
	{
		return TArray<FVector>();
	}

	if (false == PathObject->IsValid())
	{
		return TArray<FVector>();
	}

	TArray<FVector> PathPoints = PathObject->PathPoints;

	if (0 != PathPoints.Num())
	{
		int a = 0;
	}
	// 
	return PathObject->PathPoints;
}


UNavigationPath* UBTTask_AIBase::PathFindNavPath(UBehaviorTreeComponent& OwnerComp, AActor* _Actor)
{
	return PathFindNavPath(OwnerComp, _Actor->GetActorLocation());
}

UNavigationPath* UBTTask_AIBase::PathFindNavPath(UBehaviorTreeComponent& _OwnerComp, FVector _EndPos)
{
	// ã�Ƴ��� ��θ� TArray<FVector> �׳� �̷� �����ͷ� ������ �ʴ´�.

// ã�Ƴ��� ��ΰ� ��ȿ�Ѱ�?
	UNavigationPath* PathObject = nullptr;
	FVector StartPos = GetGlobalCharacter(_OwnerComp)->GetActorLocation();
	FVector EndPos = _EndPos;

	PathObject = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		StartPos,
		EndPos
	);
	return PathObject;
}