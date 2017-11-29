#include "nPhysics.h"

AnPhysics::AnPhysics()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pcCmp			= NULL;
	bSimulate	= false;
	bGravity		= false;
	fMass			= 1.0f;
	vForce.Set(0,0,0);
	fAccum	= 0.0f;
	fUpdate	= 0.0f;
	tPrev.SetIdentity();
	}	// AnPhysics

void AnPhysics::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnPhysics::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();

	// Defaults
	pcCmp			= NULL;
	bSimulate	= false;
	bGravity		= false;
	fAccum		= 0.0f;
	fUpdate		= 0.0f;
	tPrev.SetIdentity();

	}	// BeginPlay

void AnPhysics :: EndPlay ( const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Ends gameplay for this component
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnPhysics::EndPlay"));

	// Clean up
	pcCmp	= NULL;

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

void AnPhysics :: onValue (	const WCHAR *pwRoot, 
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
	bool	bX		= false;
	bool	bY		= false;
	bool	bZ		= false;

	// Base behaviour
	AnElement::onValue(pwRoot,pwLoc,v);

	// Simulate physics
	if (!WCASECMP(pwLoc,L"Simulate/OnFire/Value"))
		{
		// Simulation
		bSimulate = adtBool(v);

		// Update if component valid
		if (pcCmp != NULL)
			pcCmp->SetSimulatePhysics(bSimulate);
		}	// if

	// Gravity enable
	else if (!WCASECMP(pwLoc,L"Gravity/OnFire/Value"))
		{
		// Simulation
		bGravity = adtBool(v);

		// Update if component valid
		if (pcCmp != NULL)
			pcCmp->SetSimulatePhysics(bGravity);
		}	// if

	// Mass
	else if (!WCASECMP(pwLoc,L"Mass/OnFire/Value"))
		{
		// New mass
		fMass = adtFloat(v);

		// Update if component valid
		if (pcCmp != NULL)
			pcCmp->SetMassScale(NAME_None,fMass);
		}	// if

	// Target location
	else if (!WCASECMP(pwLoc,L"Location/OnFire/Value"))
		{
		HRESULT				hr				= S_OK;
		IDictionary			*pDct			= NULL;
		nElementRef			*pElemRef	= NULL;
		USceneComponent	*pSc			= NULL;
		adtString			strLocAt(v);
		adtString			strFull;
		adtValue				vL;
		adtIUnknown			unkV;

		UE_LOG(LogTemp, Warning, TEXT("AnPhysics::Location"));

		// Zero active component
		pcCmp = NULL;

		// Generate path to target
		CCLTRY(adtValue::copy(strLoc,strFull));
		CCLTRY(strFull.append(strLocAt));

		// Retrieve object from render dictionary
		CCLTRY(nspcLoadPath(pLoc->pDctRen,strFull,vL));

		// Path will be a dictionary for the entry
		CCLTRY(_QISAFE((unkV=vL),IID_IDictionary,&pDct));

		// Access the actual element reference
		CCLTRY(pDct->load(adtString(L"Element"),vL));

		// Cast directly, depends on knowledge of how it is stored in "nLoc"
		CCLTRYE( (pElemRef = ((nElementRef *)vL.punk)) != NULL, E_UNEXPECTED );

		// Valid scene component ?
		CCLTRYE( (pSc = pElemRef->pElem->GetRootComponent()) != NULL, E_UNEXPECTED );

		// Valid primitive component ?
		CCLTRYE( pSc->IsA(UPrimitiveComponent::StaticClass()) == true, E_UNEXPECTED );

		// Assign
		CCLOK ( pcCmp = Cast<UPrimitiveComponent>(pSc); )

		// Update properties
		if (hr == S_OK)
			{
			pcCmp->bApplyImpulseOnDamage = false;
			pcCmp->SetSimulatePhysics(bSimulate);
			pcCmp->SetEnableGravity(bGravity);
			pcCmp->SetMassScale(NAME_None,fMass);
			}	// if

		// Clean up
//		_RELEASE(pDct);
		}	// if

	// Transform update
	else if (!WCASECMP(pwLoc,L"Update/OnFire/Value"))
		{
		adtInt iUp(v);

		// Reset timing
		fUpdate	= (iUp != 0) ? iUp/1000.0f : 0.0f;
		fAccum	= 0.0f;
		}	// else if

	// Force values
	else if (	(bX = !WCASECMP(pwLoc,L"Force/A1/OnFire/Value")) == true ||
					(bY = !WCASECMP(pwLoc,L"Force/A2/OnFire/Value")) == true ||
					(bZ = !WCASECMP(pwLoc,L"Force/A3/OnFire/Value")) == true )
		{
		// Assign
		if			(bX)	vForce.X = adtFloat(v);
		else if	(bY)	vForce.Y = adtFloat(v);
		else				vForce.Z = adtFloat(v);
		}	// else if

	}	// onReceive

void AnPhysics :: Tick( float DeltaTime )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called every frame
	//
	//	PARAMETERS
	//		-	DeltaTime is amount of elapased time.
	//
	////////////////////////////////////////////////////////////////////////

	// Base behaviour
	Super::Tick( DeltaTime );

	// It is possible for the target component to show up later...

	// If a valid component exists and there is a non-zero force specified,
	// add it in for the tick.  Check to see if physics is enabled first
	// otherwise lots of error messages are output.
	if (pcCmp != NULL && bSimulate)
		pcCmp->AddForce(vForce);

	// Time to update ?
	fAccum += DeltaTime;
	if (fUpdate > 0.0f && fAccum > fUpdate)
		{
		// Update any changes in the transform
//		UE_LOG(LogTemp, Warning, TEXT("Update!") );

		// Valid update component ?
		if (pcCmp != NULL)
			{
			// Current transform
			FTransform
			tNow = pcCmp->GetRelativeTransform();

			// Compare each set of components

			// Translation
			FVector
			fvNow		= tNow.GetTranslation();
			FVector
			fvPrev	= tPrev.GetTranslation();
			if (fvNow.X != fvPrev.X)
				updateValue ( L"Transform/Translate/A1/Fire", fvNow.X );
			if (fvNow.Y != fvPrev.Y)
				updateValue ( L"Transform/Translate/A2/Fire", fvNow.Y );
			if (fvNow.Z != fvPrev.Z)
				updateValue ( L"Transform/Translate/A3/Fire", fvNow.Z );

			// Scaling
			fvNow		= tNow.GetScale3D();
			fvPrev	= tPrev.GetScale3D();
			if (fvNow.X != fvPrev.X)
				updateValue ( L"Transform/Scale/A1/Fire", fvNow.X );
			if (fvNow.Y != fvPrev.Y)
				updateValue ( L"Transform/Scale/A2/Fire", fvNow.Y );
			if (fvNow.Z != fvPrev.Z)
				updateValue ( L"Transform/Scale/A3/Fire", fvNow.Z );

			// Rotation
			FQuat
			fqNow		= tNow.GetRotation();
			FQuat
			fqPrev	= tPrev.GetRotation();
			if (fqNow.X != fqPrev.X)
				updateValue ( L"Transform/Rotate/A1/Fire", fqNow.X );
			if (fqNow.Y != fqPrev.Y)
				updateValue ( L"Transform/Rotate/A2/Fire", fqNow.Y );
			if (fqNow.Z != fqPrev.Z)
				updateValue ( L"Transform/Rotate/A3/Fire", fqNow.Z );
			if (fqNow.W != fqPrev.W)
				updateValue ( L"Transform/Rotate/A4/Fire", fqNow.Z );

			// Latest transform
			tPrev = tNow;
			}	// if

		// Accumulated time
		fAccum = (fAccum-fUpdate);
		}	// if

	}	// Tick

void AnPhysics :: updateValue ( const WCHAR *pwVal, float fV )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Update(store) a value into the element.
	//
	//	PARAMETERS
	//		-	pwVal is the relative location of the value
	//		-	fV is the value to use
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocUp;

	// Generate location to value using element location
	CCLTRY ( adtValue::copy ( strLoc, strLocUp ) );
	CCLTRY ( strLocUp.append ( pwVal ) );

	// Store into path
	CCLTRY ( pLoc->addStore ( strLocUp, adtFloat(fV) ) );

	}	// updateValue
