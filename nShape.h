// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "nElement.h"
#include "nShape.generated.h"

/**
 * 
 */
UCLASS()
class AnShape : public AnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnShape();

	// Run-time data
	adtString						strName;				// Shape name
	UStaticMeshComponent			*pcShp;				// Shape mesh
//	UStaticMesh						*pMesh;
	UMaterialInterface			*pMat;				// Material
	UMaterialInstanceDynamic	*pMatDyn;			// Dynamic material

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	EndPlay	( const EEndPlayReason::Type ) override;
	virtual bool	tickMain	( float ) override;
	virtual bool	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & ) override;

	};
