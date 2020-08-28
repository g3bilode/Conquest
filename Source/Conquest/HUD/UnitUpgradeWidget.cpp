// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitUpgradeWidget.h"

void UUnitUpgradeWidget::EnableUpgrade()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UUnitUpgradeWidget::DisableUpgrade()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UUnitUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}
