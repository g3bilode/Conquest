// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionsWidget.h"
#include "UnitUpgradeWidget.h"


void UUnitActionsWidget::PopulateUpgradeGridData()
{
	for (UUnitUpgradeWidget* unitUpgradeWidget : UnitUpgradeGrid)
	{
		unitUpgradeWidget->EnableUpgrade();
	}
}
