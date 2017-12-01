// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "nElement.h"
#include "nAudio.generated.h"

/**
 * 
 */
UCLASS()
class AnAudio : 
	public AnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnAudio();

	// Run-time data
	FVector				vAudio;							// Audio position
	adtString			strAudio;						// Audio Id

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};
