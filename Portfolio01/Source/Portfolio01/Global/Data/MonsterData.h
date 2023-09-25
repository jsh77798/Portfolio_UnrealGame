#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include <Game/AI/Portfolio_MonsterEnums.h>
#include "MonsterData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct PORTFOLIO01_API FMonsterData : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterData(){}
	~FMonsterData(){}

	// �𸮾� �����Ϳ��� DT_MonsterData���̺� �� �ش缳������ �����ϰ� ������ �� �ִ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data") //HP����
	    int HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data") //ATT����
		int ATT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data") //AI����
		class UBehaviorTree* AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data") //�ִϸ��̼� ����
		TMap<AIState, class UAnimMontage*> MapAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float SearchRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float AttackRange = 200.0f;
};
