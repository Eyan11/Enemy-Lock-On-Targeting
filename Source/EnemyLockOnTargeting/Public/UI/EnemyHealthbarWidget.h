/*
* Author: Eyan Martucci
* Description: Manages enemy healthbar UI
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthbarWidget.generated.h"


UCLASS()
class ENEMYLOCKONTARGETING_API UEnemyHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	void SetBarValuePercent(float const value);
	void HideHealthbar();
	void ShowHealthbar();

private:

	UPROPERTY()
	class UProgressBar* Healthbar = nullptr;
};
