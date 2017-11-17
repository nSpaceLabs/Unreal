////////////////////////////////////////////////////////////////////
//
//									nSpace.cpp
//
//				Implementation of the main nSpace actor object
//
////////////////////////////////////////////////////////////////////

#include "nSpace.h"
#include "nLoc.h"
//#include "nElement.h"

// Globals
// Temporary hack until it is determined why Unreal engine in production
// mode creates two actors.
static AnSpace *pThisActor = NULL;

AnSpace::AnSpace()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pCli			= NULL;
	pDctLoc		= NULL;

	// String references
	strRefActor = L"Actor";
//	strRenLoc	= L"/State/Interface/Dict/RenderList/Dict/";
	strRenLoc	= L"/State/Render/3D/State/Default/";

	// Worker thread
	pThrd			= NULL;
	pTick			= NULL;
	pWrkQ			= NULL;
	pWrkIt		= NULL;
	evWork.init();

	// Set this actor to call Tick() every frame.  
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	}	// AnSpace

AnSpace::~AnSpace()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Destructor for the object.
	//
	////////////////////////////////////////////////////////////////////////
	_RELEASE(pTick);
	_RELEASE(pThrd);
	_RELEASE(pWrkIt);
	_RELEASE(pWrkQ);
	_RELEASE(pDctLoc);
	if (pCli != NULL)
		{
		pCli->close();
		delete pCli;
		}	// if

	}	// ~AnSpace

HRESULT AnSpace :: addListen ( const WCHAR *pwLoc, nSpaceClientCB *pCB,
											const WCHAR *pwLocRen )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add a 'listen' request to work queue.
	//
	//	PARAMETERS
	//		-	pwLoc is the listen location
	//		-	pCB is the callback object
	//		-	pwLocRen is the root render location
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLoc(pwLoc);

	// State check
	if (!bWork || pWrkQ == NULL)
		return S_OK;

	// State check
	CCLTRYE ( strLoc.length() > 0, E_INVALIDARG );

	// Prepend the render location for relative locations
	if (hr == S_OK && *pwLoc != '/' && pwLocRen != NULL)
		hr = strLoc.prepend ( pwLocRen );

	// Add to queue and signal thread
	dbgprintf ( L"AnSpace::addStore:%s\r\n", (LPCWSTR)strLoc );
	CCLTRY ( pWrkQ->write ( adtInt(OP_LISTEN) ) );
	CCLTRY ( pWrkQ->write ( strLoc ) );
	CCLTRY ( pWrkQ->write ( adtLong((U64)pCB) ) );
	CCLOK  ( evWork.signal(); )

	return hr;
	}	// addListen

HRESULT AnSpace :: addStore ( const WCHAR *pwLoc, const ADTVALUE &v, 
											const WCHAR *pwLocRen )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add a 'store' request to store queue.
	//
	//	PARAMETERS
	//		-	pwLoc is the store location
	//		-	v is the store value
	//		-	pwLocRen is the root render location
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr = S_OK;
	adtString	strLoc(pwLoc);

	// State check
	if (!bWork || pWrkQ == NULL)
		return S_OK;

	// State check
	CCLTRYE ( strLoc.length() > 0, E_INVALIDARG );

	// Prepend the render location for relative locations
	if (hr == S_OK && *pwLoc != '/' && pwLocRen != NULL)
		hr = strLoc.prepend ( pwLocRen );

	// Add to queue and signal thread
	dbgprintf ( L"AnSpace::addStore:%s\r\n", (LPCWSTR)strLoc );
	CCLTRY ( pWrkQ->write ( adtInt(OP_STORE) ) );
	CCLTRY ( pWrkQ->write ( strLoc ) );
	CCLTRY ( pWrkQ->write ( v ) );
	CCLOK  ( evWork.signal(); )

	return hr;
	}	// addStore

void AnSpace::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when the game starts or when spawned.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT hr = S_OK;

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnSpace::BeginPlay"));

	// Default behaviour
	Super::BeginPlay();

	// TEMPORARY
	if (pThisActor != NULL)
		return;
	pThisActor = this;

	// Create worker object
	CCLTRYE ( (pTick = new AnSpacet(this)) != NULL, E_OUTOFMEMORY );
	CCLOK   ( pTick->AddRef(); )
	CCLTRY  ( pTick->construct() );

	// Create worker thread, no need to wait for startup
	CCLOK (bWork = true;)
	CCLTRY(COCREATE(L"Sys.Thread", IID_IThread, &pThrd ));
	CCLTRY(pThrd->threadStart ( pTick, 0 ));

	}	// BeginPlay

void AnSpace::EndPlay(const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play ends for this actor.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnSpace::EndPlay"));

	// TEMPORARY
	if (pThisActor == this)
		{

		// Shutdown worker thread
		if (pThrd != NULL)
			{
			pThrd->threadStop(30000);
			_RELEASE(pThrd);
			}	// if

		pThisActor = NULL;
		}	// if

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

void AnSpace :: onValue (	const WCHAR *pwRoot, 
									const WCHAR *pwLoc,
									const ADTVALUE &vV )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Process a received value.  It is assumed the string values
	//			have already been validated.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	vV contains the value
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr				= S_OK;
	bool			bCamera[4]	= { false, false, false, false };
	adtInt		iIdx;
//	int			idx;

	// Debug
//	adtString	strV;
//	adtValue::toString(vV,strV);
//	CCLOK ( lprintf ( LOG_INFO, L"%s:%s:%s\r\n", pwRoot, pwLoc, (LPCWSTR)strV ); )
//	UE_LOG(LogTemp, Warning, TEXT("AnSpace::onValue:%s:%s:%s"), pwRoot, pwLoc, (LPCWSTR)strV );

	// Check for render location indices, ignore reserved names
	if (	!WCASENCMP(pwLoc,L"Locations/Dict/",15) &&
			pwLoc[15] != '_' && (iIdx = adtString(&pwLoc[15])) >= 1)
		{
		IDictionary		*pDct	= NULL;
		int				len	= 0;
		adtIUnknown		unkV(vV);
		adtString		strL;
		adtValue			vL;

		// Descriptor
		CCLTRY(_QISAFE(unkV,IID_IDictionary,&pDct));

		// Is a render location specified ?
		CCLTRY ( pDct->load ( adtString(L"Location"), vL ) );
		CCLTRYE( (len = (strL = vL).length()) > 0, E_UNEXPECTED );

		// Local usage requires trailing slash
		if (hr == S_OK && strL[len-1] != '/')
			hr = strL.append ( L"/" );
/*
		// Does location handler at index already exist ?
		if (hr == S_OK && pDctLoc->load ( iIdx, vL ) == S_OK)
			{
			IDictionary *pDsc		= NULL;
			nElement		*pElem	= NULL;
			adtIUnknown	unkV(vL);

			// Access descriptor and root element for existing renderer
			CCLTRY  ( _QISAFE(unkV,IID_IDictionary,&pDsc) );
			CCLTRY  ( pDsc->load ( adtString(L"Element"), vL ) );
			CCLTRYE ( (pElem = (nElement *)(IUnknown *)(unkV=vL)) != NULL, E_UNEXPECTED );
			CCLTRYE ( pElem->pRenLoc != NULL, E_UNEXPECTED );
			_RELEASE(pDsc);

			// Location changed ?
//			if (hr == S_OK && WCASECMP(pElem->pRenLoc->strLocRen,strL))
//				{
				// Shutdown the location
				pElem->pRenLoc->setRoot(NULL);

				// Remove from world
				pElem->pRenLoc->Destroy();

				// Remove from render dictionary
				pDctLoc->remove ( iIdx );

				// A new render location will be created below.
//				}	// if

			// Location same
//			else if (hr == S_OK && pElem->pRoot != NULL)
//				{
//				FVector	fTrans;

				// Render location object exists and location is the same, just update the position.

				// Fill in coordinate.  See comments in 'nElement' for assignment
//				if (hr == S_OK && pDct->load ( adtString(L"X"), vL ) == S_OK)
//					fTrans.Y = 25*adtDouble(vL);
//				if (hr == S_OK && pDct->load ( adtString(L"Y"), vL ) == S_OK)
//					fTrans.Z = 25*adtDouble(vL);
//				if (hr == S_OK && pDct->load ( adtString(L"Z"), vL ) == S_OK)
//					fTrans.X = 25*adtDouble(vL);

				// Position has to be updated in main loop so 'recieve' new position
//				dbgprintf ( L"AnSpace::onValue:Move %g,%g,%g\r\n", fTrans.X, fTrans.Y, fTrans.Z );

//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A1/OnFire/Value", adtDouble(fTrans.X) );
//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A2/OnFire/Value", adtDouble(fTrans.Y) );
//				pElem->pRoot->onReceive ( pElem, L"", L"Element/Transform/Translate/A3/OnFire/Value", adtDouble(fTrans.Z) );

				// Schedule the element for work
//				addMain ( pElem );
//				}	// else if

			}	// else if
*/
		// Need a new location handler at index ?
		if (hr == S_OK && pDctLoc->load ( iIdx, vL ) != S_OK)
			{
			AnLoc			*pLoc = NULL;
			adtValue		vL;
			adtIUnknown	unkV;

			// Store as descriptor for index
			CCLTRY ( pDctLoc->store ( iIdx, adtIUnknown(pDct) ) );

			// Spawn an nSpace render location actor
			CCLTRYE ( (pLoc = Cast<AnLoc>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
							this,AnLoc::StaticClass(),FTransform()))) != NULL, E_UNEXPECTED );
			CCLOK ( pLoc->init(this,strL,iIdx); )
			CCLOK ( UGameplayStatics::FinishSpawningActor(pLoc,FTransform()); )
			}	// if

		// Clean up
		_RELEASE(pDct);
		}	// if

	// Level selection
	else if (!WCASECMP(pwLoc,L"Level/Element/Default/OnFire/Value"))
		{
		char	*pcLevel	= NULL;
		adtString	strLevel(vV);

		// Convert to ASCII for API
		if (strLevel.length() > 0 && strLevel.toAscii(&pcLevel) == S_OK)
			{
			// Is the level name changing ?
			if (UGameplayStatics::GetCurrentLevelName(GetWorld()).
					Compare(pcLevel) != 0)
				{
				UE_LOG(LogTemp, Warning, TEXT("Tick::Level change:%s"), (LPCWSTR)strLevel);

				// Request level change
				UGameplayStatics::OpenLevel(GetWorld(),pcLevel);
				}	// if

			// Clean up
			_FREETASKMEM(pcLevel);
			}	// if

		}	// else if

	// Camera changes
	else if (	((bCamera[0] = (WCASECMP(pwLoc,L"Camera/Rotate/A1/OnFire/Value") == 0)) == true) ||
					((bCamera[1] = (WCASECMP(pwLoc,L"Camera/Rotate/A2/OnFire/Value") == 0)) == true) ||
					((bCamera[2] = (WCASECMP(pwLoc,L"Camera/Rotate/A3/OnFire/Value") == 0)) == true) ||
					((bCamera[3] = (WCASECMP(pwLoc,L"Camera/Rotate/A4/OnFire/Value") == 0)) == true) )
		{
		// Cache new value
		if			(bCamera[0])	fCamera[0] = adtFloat(vV);
		else if	(bCamera[1])	fCamera[1] = adtFloat(vV);
		else if	(bCamera[2])	fCamera[2] = adtFloat(vV);
		else if	(bCamera[3])	fCamera[3] = adtFloat(vV);

		// Camera update ?  NOTE: Currently using the knowledge that all
		// four coordinates are updated at the same time, change this...
		if (bCamera[3])
			{
			static bool			bFirst = true;
			static FRotator	rCtlr0;
			static FRotator	rCamera0;

			// Controller for the game
			APlayerController
			*pCtlr = UGameplayStatics::GetPlayerController(GetWorld(),0);

			// Update orientation
			if (pCtlr != NULL)
				{
				FRotator 
				rCamera = FRotator(FQuat(fCamera[0],fCamera[1],fCamera[2],fCamera[3]));

				// Use start orientation as a baseline
				if (bFirst)
					{
					// Store initial position of player
					rCtlr0	= pCtlr->GetControlRotation();

					// Store initial offset for camera
					rCamera0	= rCamera;

					// Initial capture done
					bFirst	= false;
					}	// if

				// Offset of baseline
				rCamera -= rCamera0;

				// Compute new offset player controller
				FRotator
				rCtlr		= rCtlr0;
				rCtlr		+= rCamera;

				// New orientation
				pCtlr->SetControlRotation(rCtlr);
				}	// if
			}	// if
		}	// else if

	}	// onValue

void AnSpace :: Tick( float DeltaTime )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called every frame
	//
	//	OVERLOAD FROM
	//		AActor
	//
	//	PARAMETERS
	//		-	DeltaTime is amount of elapased time.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr = S_OK;

	// Base behaviour
	Super::Tick( DeltaTime );

	// Distribute received values
	dequeue();
	}	// Tick

//
// AnSpacet
//

AnSpacet :: AnSpacet ( AnSpace *_pThis )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	//	PARAMETERS
	//		-	_pR is the parent object
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	pThis	= _pThis;
	}	// AnSpacet

HRESULT AnSpacet :: tick ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Perform one 'tick's worth of work.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr	= S_OK;

	// Wait for work
	CCLTRYE ( pThis->evWork.wait ( -1 ), ERROR_TIMEOUT );

	// Execute scheduled work	
	while (	hr == S_OK		&& 
				pThis->bWork	&& 
				pThis->pWrkQ->isEmpty() != S_OK)
		{
		adtValue		vV;
		adtInt		iOp;
		adtString	strLoc;

		// Op code
		CCLTRY ( pThis->pWrkIt->read ( vV ) );
		CCLOK  ( iOp = vV; )
		CCLOK  ( pThis->pWrkIt->next(); )

		// Location
		CCLTRY ( pThis->pWrkIt->read ( vV ) );
		CCLOK  ( pThis->pWrkIt->next(); )

		switch (iOp)
			{
			case OP_STORE :
				{
				adtString strLoc(vV);

				// Value to store
				CCLTRY ( pThis->pWrkIt->read ( vV ) );
				CCLOK  ( pThis->pWrkIt->next(); )

				// Issue command
				CCLOK ( pThis->pCli->store ( strLoc, vV ); )
				}	// OP_STORE
				break;

			case OP_LISTEN :
				{
				adtString strLoc(vV);

				// Callback to use
				CCLTRY ( pThis->pWrkIt->read ( vV ) );
				CCLOK  ( pThis->pWrkIt->next(); )

				// Issue command
				CCLOK ( pThis->pCli->listen ( strLoc, true, (nSpaceClientCB *)(U64)adtLong(vV)); )
				}	// OP_STORE
				break;
			}	// switch

		}	// while

	// Keep running ?
	CCLTRYE ( pThis->bWork == true, S_FALSE );

	return hr;
	}	// tick

HRESULT AnSpacet :: tickAbort ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that 'ticking' should abort.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	pThis->bWork = false;
	pThis->evWork.signal();
	return S_OK;
	}	// tickAbort

HRESULT AnSpacet :: tickBegin ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that it should get ready to 'tick'.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr			= S_OK;
//	nElement	*pElem	= NULL;

	// Initialize COM for thread
	CCLTRYE ( CoInitializeEx ( NULL, COINIT_MULTITHREADED ) == S_OK,
					GetLastError() );

	// Create dictionary for render locations
	CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pThis->pDctLoc ) );

	//
	// Work queues
	//

	// Create queue and event for scheduling work from worker thread
	CCLTRY ( COCREATE ( L"Adt.Queue", IID_IList, &pThis->pWrkQ ) );
	CCLTRY ( pThis->pWrkQ->iterate ( &pThis->pWrkIt ) );

	//
	// Client
	//

	// Create client
	CCLTRYE((pThis->pCli = new nSpaceClient()) != NULL, E_OUTOFMEMORY);

	// Open default namespace 
	CCLTRY(pThis->pCli->open(L"{ Namespace Default }", true, NULL));

	// Listen to the default render locations which contains desired visuals to be rendered
	CCLTRY ( pThis->pCli->listen ( pThis->strRenLoc, true, pThis ) );

	// Debug
//	UE_LOG(LogTemp, Warning, TEXT("AnSpace::tickBegin"));
	
//	if (hr != S_OK)
//		MessageBox ( NULL, L"AnSpaceLoc::tickBegin", L"Error!", MB_OK );

	return hr;
	}	// tickBegin

HRESULT AnSpacet :: tickEnd ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	OVERLOAD
	//	FROM		ITickable
	//
	//	PURPOSE
	//		-	Notifies the object that 'ticking' is to stop.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr		= S_OK;
	IIt		*pIt	= NULL;

	// Stop listening to render location
	if (pThis->pCli != NULL)
		pThis->pCli->listen ( pThis->strRenLoc, false );

	// Clean up
	_RELEASE(pThis->pWrkIt);
	_RELEASE(pThis->pWrkQ);

	// Clean up
	_RELEASE(pThis->pDctLoc);
	if (pThis->pCli != NULL)
		{
		// Close link
		pThis->pCli->close();

		// Clean up
		delete pThis->pCli;
		pThis->pCli	= NULL;
		}	// if

	// Clean up
	CoUninitialize();

	return hr;
	}	// tickEnd

///////////
// nValues
///////////

nValues :: nValues()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pQv	= NULL;
	pQvIt	= NULL;

	// Create value queue
	if (COCREATE ( L"Adt.Queue", IID_IList, &pQv ) == S_OK)
		pQv->iterate(&pQvIt);

	}	// nValues

nValues :: ~nValues()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	////////////////////////////////////////////////////////////////////////
	_RELEASE(pQvIt);
	_RELEASE(pQv);
	}	// ~nValues

HRESULT nValues :: dequeue ( void )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Dequeue the queue values and send it through 'onValue'
	//
	//	RETURN VALUE
	//		S_OK if there was a value, S_FALSE if not.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr = S_OK;
	adtValue	vR,vL,vV;

	// Any values ?
	while (hr == S_OK && pQv->isEmpty() == S_FALSE)
		{
		// Next tuple
		CCLTRY ( pQvIt->read ( vR ) );
		CCLOK  ( pQvIt->next(); )
		CCLTRY ( pQvIt->read ( vL ) );
		CCLOK  ( pQvIt->next(); )
		CCLTRY ( pQvIt->read ( vV ) );
		CCLOK  ( pQvIt->next(); )

		// Proces
		onValue ( adtString(vR), adtString(vL), vV );
		}	// while

	return hr;
	}	// dequeue

HRESULT nValues :: enqueue ( const WCHAR *pwRoot, const WCHAR *pwLoc,
										const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add a values to the queue
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
	HRESULT	hr = S_OK;

	// Add to queue
	CCLTRY ( pQv->write ( adtString(pwRoot) ) );
	CCLTRY ( pQv->write ( adtString(pwLoc) ) );
	CCLTRY ( pQv->write ( v ) );

	return S_OK;
	}	// enqueue

HRESULT nValues :: onReceive (	const WCHAR *pwRoot, 
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
	UE_LOG(LogTemp, Warning, TEXT("nValues::onReceive"));

	// Queue it for later distribution in game thread
	return enqueue ( pwRoot, pwLoc, v );
	}	// onReceive

void nValues :: onValue (	const WCHAR *pwRoot, 
									const WCHAR *pwLoc,
									const ADTVALUE &vV )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Process a received value.
	//
	//	PARAMETERS
	//		-	pwRoot is the path to the listened location
	//		-	pwLoc is the location relative to the root for the value
	//		-	vV contains the value
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	}	// onValue
