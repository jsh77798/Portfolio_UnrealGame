// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/AI/BTTask_AIBase.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ATTACKCHECK.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO01_API UBTTask_ATTACKCHECK : public UBTTask_AIBase
{
	GENERATED_BODY()

	UBTTask_ATTACKCHECK();

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DelataSeconds);

};