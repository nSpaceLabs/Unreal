#include "nShape.h"

// Globals
static ConstructorHelpers::FObjectFinder<UObject> *FCub	= NULL;
static ConstructorHelpers::FObjectFinder<UObject> *FSph	= NULL;
static ConstructorHelpers::FObjectFinder<UObject> *FCyl	= NULL;
static ConstructorHelpers::FObjectFinder<UObject> *FCon	= NULL;
static ConstructorHelpers::FObjectFinder<UObject> *FMat	= NULL;

AnShape::AnShape()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pcShp		= NULL;
	pMatDyn	= NULL;
	pMat		= NULL;
	iColor	= -1;

	// Globals
	if (FCub == NULL)
		FCub = new ConstructorHelpers::FObjectFinder<UObject> 
					( TEXT("/Engine/BasicShapes/Cube") );
	if (FSph == NULL)
		FSph = new ConstructorHelpers::FObjectFinder<UObject> 
					( TEXT("/Engine/BasicShapes/Sphere") );
	if (FCyl == NULL)
		FCyl = new ConstructorHelpers::FObjectFinder<UObject> 
					( TEXT("/Engine/BasicShapes/Cylinder") );
	if (FCon == NULL)
		FCon = new ConstructorHelpers::FObjectFinder<UObject> 
					( TEXT("/Engine/BasicShapes/Cone") );
	if (FMat == NULL)
		FMat = new ConstructorHelpers::FObjectFinder<UObject> 
					( TEXT("/Engine/BasicShapes/BasicShapeMaterial") );

	}	// AnShape

void AnShape :: BeginPlay ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Starts gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnShape::BeginPlay"));

	// Base behaviour
	AnElement::BeginPlay();

	// Create static mesh component to use for shape
	pcShp					= NewObject<UStaticMeshComponent>
								(this,UStaticMeshComponent::StaticClass());
	pcShp->bVisible	= true;

	SetRootComponent(pcShp);
//	pcShp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);

	// Testing
	pcShp->SetMobility(EComponentMobility::Movable);
//	pcShp->SetSimulatePhysics(true);
//	pcShp->SetEnableGravity(false);
//	pcShp->SetSimulatePhysics(true);

	// Assign material
	if (FMat->Succeeded() && (pMat = Cast<UMaterialInterface> ( FMat->Object )) != NULL)
		{
		// In order to change the color a dynamic material must be used
		pMatDyn = UMaterialInstanceDynamic::Create(pMat,this);

		// Default color
		pMatDyn->SetVectorParameterValue ( FName("Color"), FLinearColor(.5,.5,.5) );

		// Assign material to component
		pcShp->SetMaterial(0,pMatDyn);
		}	// if

	// Place component in world
	pcShp->RegisterComponent();
//pcShp->SetVisibility(true);

	}	// BeginPlay

void AnShape :: EndPlay ( const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnShape::EndPlay"));

	// Base behaviour
	AnElement::EndPlay(rsn);
	}	// EndPlay

void AnShape :: onValue (	const WCHAR *pwRoot, 
									const WCHAR *pwLoc,
									const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when a listened location receives a value.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
//	UE_LOG(LogTemp, Log, TEXT("AnShape::onValue:%s:%s"), pwRoot, pwLoc );

	// Base behaviour
	AnElement::onValue(pwRoot,pwLoc,v);

	// Color
	if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		// Set base color of dynamic texture
		if (pMatDyn != NULL)
			pMatDyn->SetVectorParameterValue (	FName("Color"), 
						FLinearColor(	((iColor>>16)&0xff)/255.0,
											((iColor>>8)&0xff)/255.0,
											((iColor>>0)&0xff)/255.0,
											((iColor>>24)&0xff)/255.0 ) );
//															FLinearColor(FColor(iColor)) );
		}	// if

	// Shape name
	else if (!WCASECMP(pwLoc,L"Name/OnFire/Value"))
		{
		adtString strName(v);

		// Shape
		if (strName[0] != '\0')
			{
			UStaticMesh	*pMesh	= NULL;
			dbgprintf ( L"AnShape::mainTick:Shape:%s\r\n", (LPCWSTR)strName );

			// Previous shape
	//		if (pcShp != NULL)
	//			{
	//			pcShp->UninitializeComponent();
	//			pcShp = NULL;
	//			}	// if

			// Load the appropriate mesh
			if			(	!WCASECMP(strName,L"Sphere")	&& 
							FSph != NULL						&& 
							FSph->Succeeded() )
				pMesh = Cast<UStaticMesh>(FSph->Object);
			else if	(	!WCASECMP(strName,L"Cylinder")	&& 
							FCyl != NULL							&& 
							FCyl->Succeeded() )
				pMesh = Cast<UStaticMesh>(FCyl->Object);
			else if	(	!WCASECMP(strName,L"Cone")			&& 
							FCon != NULL							&& 
							FCon->Succeeded() )
				pMesh = Cast<UStaticMesh>(FCon->Object);

			// Rather than error out, default to using a cube
			else if	(	FCub != NULL							&& 
							FCub->Succeeded() )
				pMesh = Cast<UStaticMesh>(FCub->Object);

			// Success ?
			if (pMesh == NULL)
				dbgprintf ( L"AnShape::mainTick:Failed to create mesh for %s\r\n", (LPCWSTR)strName );

			// Assign the loaded mesh
			if (pMesh != NULL)
				pcShp->SetStaticMesh ( pMesh );

			// Shape specific adjustments
			if	(!WCASECMP(strName,L"Cone"))
				pcShp->SetRelativeLocation(FVector(0,0,+0.20));

			// Update size of shape
			if (pMesh != NULL)
				{
				FVector	fMin,fMax;
				float		fSclMax;

				// Compute the max. bounds of the mesh in order to compute scaling
				// to a unit cube.
				pcShp->GetLocalBounds( fMin, fMax );
				fSclMax = 0;
				if ((fMax.X-fMin.X) > fSclMax)
					fSclMax = (fMax.X-fMin.X);
				if ((fMax.Y-fMin.Y) > fSclMax)
					fSclMax = (fMax.Y-fMin.Y);
				if ((fMax.Z-fMin.Z) > fSclMax)
					fSclMax = (fMax.Z-fMin.Z);

				// Scale to unit cube centered at origin (-0.5 to +0.5)
				fSclLcl.X = +1.0/fSclMax;
				fSclLcl.Y = fSclLcl.X;
				fSclLcl.Z = fSclLcl.X;

				// Initial scaling
				pcShp->SetRelativeScale3D(fSclLcl);

				// Root transform was updated
				rootUpdate();
				}	// if

		//
		// Collision
		//	- Unreal seems to need the collision mesh to be the root component
		//		of an actor.  Determine the closest fit shape based on extent
		//		and the appropriate component type.
		//

	/*
		// Collision sphere
		pcColl = NewObject<USphereComponent>(pcShp,USphereComponent::StaticClass());
		pcColl->InitSphereRadius(1.0f);
	//	pcColl->SetupAttachment(pcShp);
	//	pcColl->AttachToComponent(pcShp,FAttachmentTransformRules::KeepRelativeTransform);
		pcColl->WeldTo(this);
		pcColl->RegisterComponent();

		pcColl->bVisible		= true;
		pcColl->bHiddenInGame	= false;

			pcColl->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			pcColl->SetCollisionProfileName("Default");
			pcColl->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	*/

			// Testing
	//		pcShp->SetEnableGravity(false);
	//		pcShp->SetSimulatePhysics(true);
	//		pcShp->SetVisibility(true,true);
			}	// if

		}	// if

	}	// onValue
