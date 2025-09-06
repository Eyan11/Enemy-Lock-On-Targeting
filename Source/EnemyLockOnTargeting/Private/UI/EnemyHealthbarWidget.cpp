/*
* Author: Eyan Martucci
* Description: Manages enemy healthbar UI
*/

#include "UI/EnemyHealthbarWidget.h"
#include "Components/ProgressBar.h"


void UEnemyHealthbarWidget::NativeConstruct()
{
	SetBarValuePercent(1.0f);
	HideHealthbar();
}

// Sets the value of the healthbar widget
void UEnemyHealthbarWidget::SetBarValuePercent(float const value)
{
	if (!Healthbar && GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Healthbar is null in EnemyHealthbarWidget"));
		return;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Setting Healthbar Percent to ") + FString::SanitizeFloat(value));
	}

	Healthbar->SetPercent(value);
}

// Collapses healthar widget
void UEnemyHealthbarWidget::HideHealthbar()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Hiding Healthbar"));
	}
}

// Makes healthbar widget visible
void UEnemyHealthbarWidget::ShowHealthbar()
{
	SetVisibility(ESlateVisibility::Visible);
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Showing Healthbar"));
	}
}
