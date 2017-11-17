// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "nElement.h"
#include "nLabel.generated.h"

UCLASS()
class AnLabel : 
	public AnElement
	{
	GENERATED_BODY()
	
	public:	
	// Sets default values for this actor's properties
	AnLabel();
	virtual ~AnLabel();

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	EndPlay	( const EEndPlayReason::Type ) override;
	virtual void	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );

	// Text rendering
	UTextRenderComponent	*pcLbl;
	UTextRenderComponent	*pcCrt;
	UBoxComponent			*pcBox;

	// Update label
	adtString	strHorz,strVert;
	float			*pfXs;
	int			iXs;
	int			iCaret,iCaretP;

	private :

	// Internal utilities	
	void measure ( void );
	};
