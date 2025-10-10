/*
* Author: Eyan Martucci
* Description: Manages enemy healthbar UI
*/

#include "UI/EnemyHealthbarWidget.h"
#include "Components/ProgressBar.h"


// Initializes healthbar by setting value and hiding it
void UEnemyHealthbarWidget::NativeConstruct()
{
	SetBarValuePercent(1.0f);
	HideHealthbar();
}

// Sets the value of the healthbar widget
void UEnemyHealthbarWidget::SetBarValuePercent(float const value)
{
	Healthbar->SetPercent(value);
}

// Collapses healthar widget
void UEnemyHealthbarWidget::HideHealthbar()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

// Makes healthbar widget visible
void UEnemyHealthbarWidget::ShowHealthbar()
{
	SetVisibility(ESlateVisibility::Visible);
}
