// Fill out your copyright notice in the Description page of Project Settings.

#include "nElement.h"
#include "nPlayerController.h"

// Radians <-> degrees
#define	RAD_TO_DEG(a)		(a)*(180.0/3.14159265358979323846)
#define	DEG_TO_RAD(a)		(a)*(3.14159265358979323846/180.0)

AnElement :: AnElement ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pLoc			= NULL;
	strLoc		= L"";
	bRun			= false;
	iState		= ELEM_STATE_INIT;
	fTi.Set(0,0,0);
	fTf.Set(0,0,0);
	fS.Set(1,1,1);
	fR.Set(0,0,0);
	}	// AnElement

AnElement :: ~AnElement ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Destructor for the object
	//
	////////////////////////////////////////////////////////////////////////
	}	// ~AnElement

void AnElement::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play beings for this actor.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr			= S_OK;

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnElement::BeginPlay"));

	// Base beahviour
	Super::BeginPlay();

	// Running
	bRun = true;

	// Schedule work for initial setup
	CCLTRY ( pLoc->addMain ( this ) );
	}	// BeginPlay

void AnElement::EndPlay(const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play ends for this actor.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnElement::EndPlay"));

	// Add work to unlisten element
	if (pLoc != NULL)
		{
		iState = ELEM_STATE_STOP;
		pLoc->addWork(this);
		}	// if

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

void AnElement :: init ( AnLoc *_pLoc, const WCHAR *pwLoc )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Initializer for the object.
	//
	//	PARAMETERS
	//		-	_pLoc is the render location object
	//		-	pwLoc is the location for the element.
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pLoc		= _pLoc;
	strLoc	= pwLoc;	strLoc.at();
	bRun		= false;
	iState	= ELEM_STATE_INIT;
	}	// init
/*
void AnElement :: onButton ( IDictionary *pDct, const WCHAR *wName,
											const WCHAR *wState )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called for a button event.
	//
	//	PARAMETERS
	//		-	pDct contains and will receive event information
	//		-	wName is the button name
	//		-	wState is the button state
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocEv;

	// Generate button event for element
	CCLTRY ( pDct->store ( adtString(L"Name"), adtString(wName) ) );
	CCLTRY ( pDct->store ( adtString(L"State"), adtString(wState) ) );

	// Generate location to button value
	CCLTRY ( adtValue::copy ( strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Button/Fire/Value" ) );

	// Send
	CCLTRY ( pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );
	}	// onButton
*/
bool AnElement :: onValue (	const WCHAR *pwRoot, 
										const WCHAR *pwLoc,
										const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		nSpaceClientCB
	//
	//	PURPOSE
	//		-	Called when a listened location receives a value.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	//	RETURN VALUE
	//		true if there is main game loop to be scheduled.
	//
	////////////////////////////////////////////////////////////////////////
	bool	bA1	= false;
	bool	bA2	= false;
	bool	bA3	= false;
	bool	bSch	= false;

//	adtString	strV;

	// Debug
//	adtValue::toString(v,strV);
//	lprintf ( LOG_INFO, L"%s:%s:%s\r\n", pwRoot, pwLoc, (LPCWSTR)strV );

	// Debug
//	if (adtValue::empty(v))
//		{
//		dbgprintf ( L"UAnElement::onReceive:Empty!:%s:%s:%s\r\n", 
//						(LPCWSTR)pParent->strLoc, pwRoot, pwLoc );
//		}	// if

	//
	// Handle paths common to all elements
	//

	// Translation
	if (	(bA1 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A1/OnFire/Value")) == true ||
			(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A2/OnFire/Value")) == true ||
			(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

			// Debug
//			dbgprintf ( L"UAnElement::onReceive:Translate:%s:%s:%s:%g\r\n", 
//							(LPCWSTR)pParent->strLoc, pwRoot, pwLoc, (double)dV );

			// Set component
			if			(bA1)
				{
				fTf.X	= dV;//*10;
				fTt.X	= SZ_TIME_MOVE;
				}	// if
			else if	(bA2)
				{
				fTf.Y	= dV;//*10;
				fTt.Y	= SZ_TIME_MOVE;
				}	// else if
			else				
				{
				fTf.Z	= dV;//*10;
				fTt.Z	= SZ_TIME_MOVE;
				}	// else

			// Schedule work on game thread to perform update
			bTrans	= true;
			bSch		= true;
			}	// if

	// Scale
	else if ((bA1 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A1/OnFire/Value")) == true ||
				(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A2/OnFire/Value")) == true ||
				(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

//			if (dV != 1)
//				dbgprintf ( L"Hi\r\n" );

			// Set component
			if			(bA1)
				fS.X	= (dV != 0.0) ? (double)dV : 1.0;
			else if	(bA2)
				fS.Y	= (dV != 0.0) ? (double)dV : 1.0;
			else				
				fS.Z	= (dV != 0.0) ? (double)dV : 1.0;

			// Debug
//			if (fS.X == 0 || fS.Y == 0 || fS.Z == 0)
//				dbgprintf ( L"fS (%g,%g,%g)\r\n", fS.X, fS.Y, fS.Z );

			// Schedule work on game thread to perform update
			bScl	= true;
			bSch	= true;
			}	// if

	// Rotation
	else if ((bA1 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A1/OnFire/Value")) == true ||
				(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A2/OnFire/Value")) == true ||
				(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

			// Debug
//			if (dV != 1)
//				dbgprintf ( L"Hi\r\n" );

			// Set component
			if			(bA1)
				{
				fR.X		= dV;
				bRot[0]	= true;
				}	// if
			else if	(bA2)
				{
				fR.Y		= dV;
				bRot[1]	= true;
				}	// if
			else				
				{
				fR.Z		= dV;
				bRot[2]	= true;
				}	// if

			// Schedule work on game thread to perform update
			bSch = true;
			}	// if

	// Visible
	else if (!WCASECMP(pwLoc,L"Element/Visible/OnFire/Value"))
		{
		// Notify of new state
//		dbgprintf ( L"Visible %d\r\n", v.vbool );
		iVisible = (adtBool(v) == true) ? 1 : 0;
		bSch = true;
		}	// else if

	// Color
	else if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		// Notify of new state
		iColor	= adtInt(v);
		bColor	= true;
		bSch		= true;
		}	// else if

	return bSch;
	}	// onValue
/*
void AnElement :: onRay ( IDictionary *pDct, const FVector &vLoc,
									const FVector &vDir )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called for a ray event.
	//
	//	PARAMETERS
	//		-	pDct contains and will receive event information
	//		-	vLoc is the ray intersection location
	//		-	vDir is the ray direction
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocEv;

	// Coordinates for ray
	CCLTRY ( pDct->store ( adtString(L"X"), adtDouble(vLoc.X) ) );
	CCLTRY ( pDct->store ( adtString(L"Y"), adtDouble(vLoc.Y) ) );
	CCLTRY ( pDct->store ( adtString(L"Z"), adtDouble(vLoc.Z) ) );
	CCLTRY ( pDct->store ( adtString(L"Xn"), adtDouble(vDir.X) ) );
	CCLTRY ( pDct->store ( adtString(L"Yn"), adtDouble(vDir.Y) ) );
	CCLTRY ( pDct->store ( adtString(L"Zn"), adtDouble(vDir.Z) ) );

	// Generate location to button value
	CCLTRY ( adtValue::copy ( strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Ray/Fire/Value" ) );

	// Send
	CCLTRY ( pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );
	}	// onRay
*/
HRESULT AnElement :: onReceive (	const WCHAR *pwRoot, 
											const WCHAR *pwLoc,
											const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		nSpaceClientCB
	//
	//	PURPOSE
	//		-	Called when a listened location receives a value.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	bool			bSch	= false;

	// Debug
//	adtString	strV;
//	adtValue::toString(v,strV);
//	UE_LOG(LogTemp, Warning, TEXT("AnElement::onReceive:%s:%s:%s"), 
//				pwRoot, pwLoc, (LPCWSTR)strV );

	// Process value
	bSch = onValue(pwRoot,pwLoc,v);

	// Forward to root component
//	if (pOuter != NULL)
//		bSch = pOuter->onReceive ( this, pwRoot, pwLoc, v );

	// Schedule game loop work if requested
	if (bSch)
		pLoc->addMain ( this );

	return S_OK;
	}	// onReceive

void AnElement :: rootUpdate ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the root component has changed.
	//
	////////////////////////////////////////////////////////////////////////

	// Update internal state on next game loop
	bTrans	= true;
	bScl		= true;
	bRot[0]	= true;
	bRot[1]	= true;
	bRot[2]	= true;
	bColor	= true;

	// Schedule main loop work
	pLoc->addMain(this);
	}	// rootUpdate

bool AnElement :: tickMain ( float fD )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Execute work for main game thread.
	//
	//	PARAMETERS
	//		-	fD is the amount of elapsed time since last game loop tick.
	//
	//	RETURN VALUE
	//		true if work is still needed
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr		= S_OK;
	bool		bWrk	= false;

	// Most states affects root component
	USceneComponent	
	*pRoot = GetRootComponent();

	// Actor is running
	if (iState == ELEM_STATE_RUN && pRoot != NULL)
		{
		// Update states

		// Translation
		if (fTt.X > 0 || fTt.Y > 0 || fTt.Z > 0)
			{
			// Current transform
			FTransform	fX		= pRoot->GetRelativeTransform();
			FVector		fNow	= fX.GetTranslation();

			// Debug.  'Snap to' location.
	//		fD = 10;

			// Initial 'from' location
			if (fTt.X == SZ_TIME_MOVE)
				fTi.X = fNow.X;
			if (fTt.Y == SZ_TIME_MOVE)
				fTi.Y = fNow.Y;
			if (fTt.Z == SZ_TIME_MOVE)
				fTi.Z = fNow.Z;

			// Debug
	//		if (fTf.X != 0 || fTf.Y != 0 || fTf.Z != 0)
	//			dbgprintf ( L"(%g,%g,%g) -> (%g,%g,%g)\r\n", 
	//							fTi.X, fTi.Y, fTi.Z, fTf.X, fTf.Y, fTf.Z );
	//		if (fTf.X != 0 || fTf.Y != 0 || fTf.Z != 0)
	//			dbgprintf ( L"Hi\r\n" );

			// Move component into position over given time.
			if (fTt.X > 0)
				{
				fTt.X		= (fTt.X < fD) ? 0 : (fTt.X-fD);
				fNow.X	= ((fTi.X-fTf.X)*(fTt.X/SZ_TIME_MOVE))+fTf.X;
				}	// if
			if (fTt.Y > 0)
				{
				fTt.Y		= (fTt.Y < fD) ? 0 : (fTt.Y-fD);
				fNow.Y	= ((fTi.Y-fTf.Y)*(fTt.Y/SZ_TIME_MOVE))+fTf.Y;
				}	// if
			if (fTt.Z > 0)
				{
				fTt.Z		= (fTt.Z < fD) ? 0 : (fTt.Z-fD);
				fNow.Z	= ((fTi.Z-fTf.Z)*(fTt.Z/SZ_TIME_MOVE))+fTf.Z;
				}	// if

			// Need work again ?
			bWrk = (fTt.X > 0 || fTt.Y > 0 || fTt.Z > 0);

	//		if (!bWrk)
	//			dbgprintf ( L"Hi\r\n" );
			// Update transform
	//		fTt.Set(0,0,0);
	//		fNow.Set(fTf.X,fTf.Y,fTf.Z);
	//		FVector	fS = fX.GetScale3D();
	//		if (!bWrk)
	//			dbgprintf ( L"%p (%g,%g,%g) (%g,%g,%g)\r\n", this, 
	//							fNow.X, fNow.Y, fNow.Z, fS.X, fS.Y, fS.Z );
			fX.SetTranslation ( fNow );
			pRoot->SetRelativeTransform(fX);
			}	// if

		// Rotation
		if (bRot[0] || bRot[1] || bRot[2])
			{
			FTransform	t;

			// Set new rotation values
			if (bRot[0])	fRotNow.X = fR.X;
			if (bRot[1])	fRotNow.Y = fR.Y;
			if (bRot[2])	fRotNow.Z = fR.Z;

			// Current transform
			t = pRoot->GetRelativeTransform();

			// Set rotation
			t.SetRotation(FQuat::MakeFromEuler(fRotNow));

			// New transform
			pRoot->SetRelativeTransform ( t );

			// Done
			fR.Set(0,0,0);
			bRot[0] = bRot[1] = bRot[2] = false;
			}	// if

		// Scale
		if (bScl)
			{
			// Current transform
			FTransform	fX		= pRoot->GetRelativeTransform();
			FVector		fScl	= fX.GetScale3D();

			// Update scale
			if (fS.X != 0)
				fScl.X = fS.X;
			if (fS.Y != 0)
				fScl.Y = fS.Y;
			if (fS.Z != 0)
				fScl.Z = fS.Z;

			// Update transform
			fX.SetScale3D ( fScl );
			pRoot->SetRelativeTransform ( fX );

			// Done
			bScl = false;
			}	// if

		// Visibility
		if (iVisible != -1)
			{
			// Change ?
			if (	(iVisible == 1 && !pRoot->bVisible) ||
					(iVisible == 0 && pRoot->bVisible) )
				{
				dbgprintf ( L"UAnElement::mainTick:Visible %d\r\n", iVisible );

				// Set new visible state
				pRoot->SetVisibility ( (iVisible == 1) ? true : false, true );
				pRoot->SetActive(pRoot->bVisible);
				}	// if

			// Updated
			iVisible = -1;
			}	// if

		}	// if

	// Initializing
	else if (iState == ELEM_STATE_INIT)
		{
		// Schedule worker thread for listening to remote location
		if (hr == S_OK)// && strLoc.length() > 0)
			{
			iState = ELEM_STATE_LISTEN;
			hr = pLoc->addWork ( this );
			}	// if
		}	// else if

	return bWrk;
	}	// tickMain

bool AnElement :: tickWork ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Execute work for worker thread.
	//
	//	RETURN VALUE
	//		true if element needs more work
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr	= S_OK;

	// Element state
	switch (iState)
		{
		// Listening to remote element needs to be setup
		case ELEM_STATE_LISTEN :

			// Generate path to visual minus the _Location field
			CCLTRY ( adtValue::copy ( pLoc->strLocRen, strLstn ) );
			CCLTRY ( strLstn.append ( strLoc ) );

			// Request listen of location for this element
			dbgprintf ( L"AnElement::workTick:Listen %s\r\n", (LPCWSTR)strLstn );
			CCLTRY ( pLoc->pSpc->pCli->listen ( strLstn, true, this ) );

			// Transition into running state
			iState = (hr == S_OK) ? ELEM_STATE_RUN : ELEM_STATE_ERROR;
			break;

		// Unlisten from current state :
		case ELEM_STATE_STOP :
			// Clean up (worker thread)
			if (pLoc != NULL && strLstn.length() > 0)
				pLoc->pSpc->pCli->listen ( strLstn, false );
			iState = ELEM_STATE_ERROR;
			break;
		}	// switch

	return false;
	}	// workTick

//
// AnElementRef
//

//
// UAnElement
//
/*
UAnElement :: UAnElement ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	//	PARAMETERS
	//		-	_pParent is the parent object.
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pOwner	= NULL;
	pOuter	= NULL;
	fTi.Set(0,0,0);
	fTf.Set(0,0,0);
	fTt.Set(0,0,0);
	fS.Set(0,0,0);
	fRotNow.Set(0,0,0);
	fSclLcl.Set(1,1,1);
	iVisible = -1;
	bRot[0] = bRot[1] = bRot[2] = false;
	}	// UAnElement

void UAnElement :: inputAdd ( UPrimitiveComponent *pC )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add component to input handling list
	//
	//	PARAMETERS
	//		-	pC is the component
	//
	////////////////////////////////////////////////////////////////////////

	// Delgates for receing input
	pC->OnBeginCursorOver.AddDynamic(this,&UAnElement::OnOverBegin);
	pC->OnEndCursorOver.AddDynamic(this,&UAnElement::OnOverEnd);
	pC->OnClicked.AddDynamic(this,&UAnElement::OnClicked);
	pC->OnReleased.AddDynamic(this,&UAnElement::OnReleased);

	}	// inputAdd

bool UAnElement :: mainTick ( float fD )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Execute work for main game thread.
	//
	//	PARAMETERS
	//		-	fD is the amount of elapsed time since last game loop tick.
	//
	//	RETURN VALUE
	//		true if work is still needed
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr		= S_OK;
	bool		bWrk = false;

	// Translate
	if (fTt.X > 0 || fTt.Y > 0 || fTt.Z > 0)
		{
		// Current transform
		FTransform	fX		= GetRelativeTransform();
		FVector		fNow	= fX.GetTranslation();

		// Debug.  'Snap to' location.
//		fD = 10;

		// Initial 'from' location
		if (fTt.X == SZ_TIME_MOVE)
			fTi.X = fNow.X;
		if (fTt.Y == SZ_TIME_MOVE)
			fTi.Y = fNow.Y;
		if (fTt.Z == SZ_TIME_MOVE)
			fTi.Z = fNow.Z;

		// Debug
//		if (fTf.X != 0 || fTf.Y != 0 || fTf.Z != 0)
//			dbgprintf ( L"(%g,%g,%g) -> (%g,%g,%g)\r\n", 
//							fTi.X, fTi.Y, fTi.Z, fTf.X, fTf.Y, fTf.Z );
//		if (fTf.X != 0 || fTf.Y != 0 || fTf.Z != 0)
//			dbgprintf ( L"Hi\r\n" );

		// Move component into position over given time.
		if (fTt.X > 0)
			{
			fTt.X		= (fTt.X < fD) ? 0 : (fTt.X-fD);
			fNow.X	= ((fTi.X-fTf.X)*(fTt.X/SZ_TIME_MOVE))+fTf.X;
			}	// if
		if (fTt.Y > 0)
			{
			fTt.Y		= (fTt.Y < fD) ? 0 : (fTt.Y-fD);
			fNow.Y	= ((fTi.Y-fTf.Y)*(fTt.Y/SZ_TIME_MOVE))+fTf.Y;
			}	// if
		if (fTt.Z > 0)
			{
			fTt.Z		= (fTt.Z < fD) ? 0 : (fTt.Z-fD);
			fNow.Z	= ((fTi.Z-fTf.Z)*(fTt.Z/SZ_TIME_MOVE))+fTf.Z;
			}	// if

		// Need work again ?
		bWrk = (fTt.X > 0 || fTt.Y > 0 || fTt.Z > 0);

//		if (!bWrk)
//			dbgprintf ( L"Hi\r\n" );
		// Update transform
//		fTt.Set(0,0,0);
//		fNow.Set(fTf.X,fTf.Y,fTf.Z);
//		FVector	fS = fX.GetScale3D();
//		if (!bWrk)
//			dbgprintf ( L"%p (%g,%g,%g) (%g,%g,%g)\r\n", this, 
//							fNow.X, fNow.Y, fNow.Z, fS.X, fS.Y, fS.Z );
		fX.SetTranslation ( fNow );
		SetRelativeTransform(fX);
		}	// if

	// Rotation
	if (bRot[0] || bRot[1] || bRot[2])
		{
		FTransform	t;

		// Set new rotation values
		if (bRot[0])	fRotNow.X = fR.X;
		if (bRot[1])	fRotNow.Y = fR.Y;
		if (bRot[2])	fRotNow.Z = fR.Z;

		// Current transform
		t = GetRelativeTransform();

		// Set rotation
		t.SetRotation(FQuat::MakeFromEuler(fRotNow));

		// New transform
		SetRelativeTransform ( t );

		// Done
		fR.Set(0,0,0);
		bRot[0] = bRot[1] = bRot[2] = false;
		}	// if

	// Scale
	if (fS.X != 0 || fS.Y != 0 || fS.Z != 0)
		{
		// Current transform
		FTransform	fX		= GetRelativeTransform();
		FVector		fScl	= fX.GetScale3D();

		// Update scale
		if (fS.X != 0)
			fScl.X = fS.X;
		if (fS.Y != 0)
			fScl.Y = fS.Y;
		if (fS.Z != 0)
			fScl.Z = fS.Z;

		// Update transform
		fX.SetScale3D ( fScl );
		SetRelativeTransform ( fX );

		// Done
		fS.Set(0,0,0);
		}	// if

	// Visibility
	if (iVisible != -1)
		{
		// Change ?
		if (	(iVisible == 1 && !bVisible) ||
				(iVisible == 0 && bVisible) )
			{
			dbgprintf ( L"UAnElement::mainTick:Visible %d\r\n", iVisible );

			// Set new visible state
			SetVisibility ( (iVisible == 1) ? true : false, true );
			SetActive(bVisible);
			}	// if

		// Updated
		iVisible = -1;
		}	// if

	return bWrk;
	}	// mainTick

void UAnElement :: onButton ( IDictionary *pDct, const WCHAR *wName,
											const WCHAR *wState )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called for a button event.
	//
	//	PARAMETERS
	//		-	pDct contains and will receive event information
	//		-	wName is the button name
	//		-	wState is the button state
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocEv;

	// Generate button event for element
	CCLTRY ( pDct->store ( adtString(L"Name"), adtString(wName) ) );
	CCLTRY ( pDct->store ( adtString(L"State"), adtString(wState) ) );

	// Generate location to button value
	CCLTRY ( adtValue::copy ( pParent->strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Button/Fire/Value" ) );

	// Send
	CCLTRY ( pParent->pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );
	}	// onButton

void UAnElement :: OnClicked ( UPrimitiveComponent *pComponent, 
										FKey ButtonPressed )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Function delegate callback for left mouse button click event.
	//
	//	PARAMETERS
	//		-	pComponent is the component
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT						hr		= S_OK;
//	AnSpcPlayerController	*pCtl	= NULL;

	// Setup
	dbgprintf ( L"UAnElement::OnClicked:%s:%p\r\n", (LPCWSTR)pParent->strLoc,
					pComponent );

	// Notify player controller of click.
//	pCtl = Cast<AnSpcPlayerController> (GetWorld()->GetFirstPlayerController());
//	if (pCtl != NULL)
//		pCtl->onClick ( this );

	IDictionary	*pDct	= NULL;
	adtString	strLocEv;


	// Generate location to button value
	CCLTRY ( adtValue::copy ( pParent->strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Button/Fire/Value" ) );

	// Generate button event for element
	CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pDct ) );
	CCLTRY ( pDct->store ( adtString(L"Name"), adtString(L"Button1") ) );
	CCLTRY ( pDct->store ( adtString(L"State"), adtString(L"Down") ) );

	// Schedule store
	CCLTRY ( pParent->pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );

	// Clean up
	_RELEASE(pDct);

	}	// OnClicked

void UAnElement :: OnOverBegin ( UPrimitiveComponent *pComponent )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Function delegate callback for begin mouse over event.
	//
	//	PARAMETERS
	//		-	pComponent is the component
	//
	////////////////////////////////////////////////////////////////////////
//	dbgprintf ( L"UAnElement::OnOverBegin:%p\r\n", pComponent );
	}	// OnOverBegin

void UAnElement :: OnOverEnd ( UPrimitiveComponent *pComponent )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Function delegate callback for end mouse over event.
	//
	//	PARAMETERS
	//		-	pComponent is the component
	//
	////////////////////////////////////////////////////////////////////////
//	dbgprintf ( L"UAnElement::OnOverEnd:%p\r\n", pComponent );
	}	// OnOverEnd

void UAnElement :: OnReleased ( UPrimitiveComponent *pComponent,
											FKey ButtonPressed )

	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Function delegate callback for left mouse button release event.
	//
	//	PARAMETERS
	//		-	pComponent is the component
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr		= S_OK;
	IDictionary	*pDct	= NULL;
	adtString	strLocEv;

	// Setup
//	dbgprintf ( L"UAnElement::OnReleased:%s:%p\r\n", (LPCWSTR)pParent->strLoc,
//					pComponent );

	// Generate location to button value
	CCLTRY ( adtValue::copy ( pParent->strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Button/Fire/Value" ) );

	// Generate button event for element
	CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pDct ) );
	CCLTRY ( pDct->store ( adtString(L"Name"), adtString(L"Button1") ) );
	CCLTRY ( pDct->store ( adtString(L"State"), adtString(L"Up") ) );

	// Schedule store
	CCLTRY ( pParent->pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );

	// Clean up
	_RELEASE(pDct);

	}	// OnReleased

void UAnElement :: onRay ( IDictionary *pDct, const FVector &vLoc,
										const FVector &vDir )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called for a ray event.
	//
	//	PARAMETERS
	//		-	pDct contains and will receive event information
	//		-	vLoc is the ray intersection location
	//		-	vDir is the ray direction
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLocEv;

	// Coordinates for ray
	CCLTRY ( pDct->store ( adtString(L"X"), adtDouble(vLoc.X) ) );
	CCLTRY ( pDct->store ( adtString(L"Y"), adtDouble(vLoc.Y) ) );
	CCLTRY ( pDct->store ( adtString(L"Z"), adtDouble(vLoc.Z) ) );
	CCLTRY ( pDct->store ( adtString(L"Xn"), adtDouble(vDir.X) ) );
	CCLTRY ( pDct->store ( adtString(L"Yn"), adtDouble(vDir.Y) ) );
	CCLTRY ( pDct->store ( adtString(L"Zn"), adtDouble(vDir.Z) ) );

	// Generate location to button value
	CCLTRY ( adtValue::copy ( pParent->strLoc, strLocEv ) );
	CCLTRY ( strLocEv.append ( L"Element/Input/Ray/Fire/Value" ) );

	// Send
	CCLTRY ( pParent->pRenLoc->addStore ( strLocEv, adtIUnknown(pDct) ) );
	}	// onRay

bool UAnElement :: onReceive (	AnElement *pElem,
											const WCHAR *pwRoot, 
											const WCHAR *pwLoc,
											const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		nSpaceClientCB
	//
	//	PURPOSE
	//		-	Called when a listened location receives a value.
	//
	//	PARAMETERS
	//		-	pElem is the nSpace element
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	v is the value
	//
	//	RETURN VALUE
	//		true if there is main game loop to be scheduled.
	//
	////////////////////////////////////////////////////////////////////////
	bool	bA1	= false;
	bool	bA2	= false;
	bool	bA3	= false;
	bool	bSch	= false;

	// Debug
//	if (adtValue::empty(v))
//		{
//		dbgprintf ( L"UAnElement::onReceive:Empty!:%s:%s:%s\r\n", 
//						(LPCWSTR)pParent->strLoc, pwRoot, pwLoc );
//		}	// if

	//
	// Handle paths common to all elements
	//

	// Translation
	if (	(bA1 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A1/OnFire/Value")) == true ||
			(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A2/OnFire/Value")) == true ||
			(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Translate/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

			// Debug
//			dbgprintf ( L"UAnElement::onReceive:Translate:%s:%s:%s:%g\r\n", 
//							(LPCWSTR)pParent->strLoc, pwRoot, pwLoc, (double)dV );

			// Set component
			if			(bA1)
				{
				fTf.X	= dV;//*10;
				fTt.X	= SZ_TIME_MOVE;
				}	// if
			else if	(bA2)
				{
				fTf.Y	= dV;//*10;
				fTt.Y	= SZ_TIME_MOVE;
				}	// else if
			else				
				{
				fTf.Z	= dV;//*10;
				fTt.Z	= SZ_TIME_MOVE;
				}	// else

			// Schedule work on game thread to perform update
			bSch = true;
			}	// if

	// Scale
	else if ((bA1 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A1/OnFire/Value")) == true ||
				(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A2/OnFire/Value")) == true ||
				(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Scale/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

//			if (dV != 1)
//				dbgprintf ( L"Hi\r\n" );

			// Set component
			if			(bA1)
				fS.X	= (dV != 0.0) ? (double)dV : 1.0;
			else if	(bA2)
				fS.Y	= (dV != 0.0) ? (double)dV : 1.0;
			else				
				fS.Z	= (dV != 0.0) ? (double)dV : 1.0;

			// Debug
//			if (fS.X == 0 || fS.Y == 0 || fS.Z == 0)
//				dbgprintf ( L"fS (%g,%g,%g)\r\n", fS.X, fS.Y, fS.Z );

			// Schedule work on game thread to perform update
			bSch = true;
			}	// if

	// Rotation
	else if ((bA1 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A1/OnFire/Value")) == true ||
				(bA2 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A2/OnFire/Value")) == true ||
				(bA3 = !WCASECMP(pwLoc,L"Element/Transform/Rotate/A3/OnFire/Value")) == true )
			{
			adtDouble	dV(v);

			// Debug
//			if (dV != 1)
//				dbgprintf ( L"Hi\r\n" );

			// Set component
			if			(bA1)
				{
				fR.X		= dV;
				bRot[0]	= true;
				}	// if
			else if	(bA2)
				{
				fR.Y		= dV;
				bRot[1]	= true;
				}	// if
			else				
				{
				fR.Z		= dV;
				bRot[2]	= true;
				}	// if

			// Schedule work on game thread to perform update
			bSch = true;
			}	// if

	// Visible
	else if (!WCASECMP(pwLoc,L"Element/Visible/OnFire/Value"))
		{
		// Notify of new state
//		dbgprintf ( L"Visible %d\r\n", v.vbool );
		iVisible = (adtBool(v) == true) ? 1 : 0;
		bSch = true;
		}	// else if

	// Color
	else if (!WCASECMP(pwLoc,L"Element/Color/OnFire/Value"))
		{
		// Notify of new state
		iColor	= adtInt(v);
		bColor	= true;
		bSch		= true;
		}	// else if

	return bSch;
	}	// onReceive

*/

/*
		// Initialize element
		case ELEM_STATE_INIT :
			{

			AnElement *pParent	= NULL;

			// Retrieve parent element if not root
			if (hr == S_OK && iRoot == 0)
				hr = pRenLoc->getParent ( strLoc, &pParent );

			//
			// Rules
			//
			if (hr == S_OK && pOuter != NULL)
				{


				// Attached to component in parent
	//			dbgprintf ( L"pC %p --> pParent %p\r\n", pOuter, (pParent != NULL) ? pParent->pOuter : NULL );
				if (pParent != NULL)
					{
					// Default is to receive 'InitializeComponent' for nSpace elements
//					pOuter->bWantsInitializeComponent = true;

					// Attached to root of parent
//					pOuter->AttachToComponent(pParent->pOuter, FAttachmentTransformRules::KeepWorldTransform);
					}	// if
				else if (pRenLoc->GetRootComponent() == NULL)
					{
					bool		bCamera	= false;
					FVector	fTrans(0,0,0);
					FVector	fScl(25,25,25);
//					FVector	fScl(1,1,1);

					// Compute the scaling factor required to enusre the current view port
					// is mapped to the nSpace unit cube.
//					UGameViewportClient
//					*pCli = pLoc->GetWorld()->GetGameViewport();
//					FIntPoint
//					pt		= pCli->Viewport->GetSizeXY();
//					FVector
//					fScl	( pt.Y, pt.Y, pt.Y );

					// As a root element, attempt to retrieve default translations for the group
					if (iRoot > 0 && pRen != NULL && pLoc->pDctRen != NULL)
						{
						IDictionary		*pDct	= NULL;
						adtValue			vL;
						adtIUnknown		unkV;

						// Obtain descriptor
						CCLTRY(pLoc->pDctRen->load ( adtInt(iRoot), vL ) );
						CCLTRY(_QISAFE((unkV=vL),IID_IDictionary,&pDct));

						// Default translations.
						// Take into account the rotated coordinate system below
						if (hr == S_OK && pDct->load ( adtString(L"X"), vL ) == S_OK)
							fTrans.Y = fScl.Y*adtDouble(vL);
						if (hr == S_OK && pDct->load ( adtString(L"Y"), vL ) == S_OK)
							fTrans.Z = fScl.Z*adtDouble(vL);
						if (hr == S_OK && pDct->load ( adtString(L"Z"), vL ) == S_OK)
							fTrans.X = fScl.X*adtDouble(vL);

						// Render location type
						if (hr == S_OK && pDct->load ( adtString(L"Type"), vL ) == S_OK)
							bCamera = !WCASECMP(adtString(vL),L"Camera");

						// Clean up
						_RELEASE(pDct);
						hr = S_OK;
						}	// if

					// Use as top level component
					pRenLoc->SetRootComponent ( pOuter );

					// Global transform
					pOuter->SetRelativeTransform ( FTransform (
						// Rotate axis so the nSpace default of XY plane facing user matches what
						// Unreal (and its input) seems to prefer : +X away, +Y right, +Z up
//						FRotator (0,90,-90),

						// No rotation
						FRotator (0,0,0),

						// Translation
						fTrans,

						// Set appropriate scaling from nSpace to Unreal engine
						// This maps the default nSpace 'unit squares' to a usable scaling in Unreal.
						fScl
						) );

					// Is the render location flagged to be part of the camera hierarchy ?
					if (hr == S_OK && bCamera)
						{
						// This root component needs to be attached to the root component of the camera

						// Current player controller
						APlayerController
						*pCtl = pLoc->GetWorld()->GetFirstPlayerController();
						if (pCtl != NULL)
							{
							// Camera manager
							APlayerCameraManager 
							*pMgr = pCtl->PlayerCameraManager;
							if (pMgr != NULL)
								{
								// Root component of camera actor
								USceneComponent *
								pOuterCam = pMgr->GetRootComponent();
								if (pOuterCam != NULL)
									{
//									FTransform
//									ft = pOuterCam->GetRelativeTransform();

									// Attach this component to root of camera
									pOuter->AttachToComponent(pOuterCam, FAttachmentTransformRules::KeepWorldTransform);
									}	// if (pOuterCam != NULL)
								}	// if (pMgr != NULL)
							}	// if (pCtl != NULL)
						}	// if

					}	// else if

				// Finish component registration
				pOuter->RegisterComponent();
				pOuter->SetVisibility(true,true);

				}	// if
			// Schedule worker thread for listening to remote location
			if (hr == S_OK)// && strLoc.length() > 0)
				{
				iState = ELEM_STATE_LISTEN;
				hr = pLoc->addWork ( this );
				}	// if

			// Error ?
			if (hr != S_OK)
				iState = ELEM_STATE_ERROR;
			break;
			}	// case ELEM_STATE_INIT

		// Running
		case ELEM_STATE_RUN :
			// Stil running ?
			if (!bRun)
				{
				// Remove root component from environment
//				if (pOuter != NULL)
//					{
//					pOuter->UnregisterComponent();
//					pOuter = NULL;
//					}	// if
				}	// if
			break;
		}	// switch
*/
