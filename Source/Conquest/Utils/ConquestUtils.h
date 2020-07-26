// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CONQUEST_API ConquestUtils
{
public:
	ConquestUtils();
	~ConquestUtils();

	/* Representation of capture point count per lane */
	static const TArray<int32> LaneSpec;
};
