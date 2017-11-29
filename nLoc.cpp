// Fill out your copyright notice in the Description page of Project Settings.

#include "nLoc.h"
//#include "nSpcLbl.h"
#include "nElement.h"

// Element types
#include "nLight.h"
#include "nShape.h"
#include "nLabel.h"
#include "nProjectile.h"
#include "nPhysics.h"

AnLoc :: AnLoc()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	//	PARAMETERS
	//		-	ObjectInitializer is used to create objects.
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////

	// Setup
	pSpc			= NULL;
	strLocRen	= L"";
	pDctRen		= NULL;

	// String references
	strRefActor = L"Actor";

 	// Set this actor to call Tick() every frame.  
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	}	// AnLoc

AnLoc :: ~AnLoc()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Destructor for the object.
	//
	////////////////////////////////////////////////////////////////////////
	_RELEASE(pDctRen);
	}	// AnLoc

HRESULT AnLoc :: addListen ( const WCHAR *pwLoc, nSpaceClientCB *pCB )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add a 'listen' request to work queue.
	//
	//	PARAMETERS
	//		-	pwLoc is the listen location
	//		-	pCB is the callback object
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	return (pSpc != NULL) ? pSpc->addListen ( pwLoc, pCB, strLocRen ) : S_FALSE;
	}	// addListen

HRESULT AnLoc :: addStore ( const WCHAR *pwLoc, const ADTVALUE &v )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Add element to store queue.
	//
	//	PARAMETERS
	//		-	pwLocRen is the root render location
	//		-	pwLoc is the store location
	//		-	v is the store value
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	return (pSpc != NULL) ? pSpc->addStore ( pwLoc, v, strLocRen ) : S_FALSE;
	}	// addStore

void AnLoc::BeginPlay()
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play beings for this actor.
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr			= S_OK;
//	AnGroup	*pElem	= NULL;

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnLoc::BeginPlay"));

	// Base beahviour
	Super::BeginPlay();

	// Create render dictionary
	CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pDctRen ) );

	// Peform listen on target location
	CCLTRY ( pSpc->addListen ( strLocRen, this ) );

/*
	// Spawn a top level group actor to act as root for visuals.
	CCLTRYE ( (pElem = Cast<AnGroup>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
					this,AnGroup::StaticClass(),FTransform()))) != NULL, E_UNEXPECTED );
	CCLOK   ( pElem->init ( this, adtInt(iIdx) ); )
	CCLOK ( UGameplayStatics::FinishSpawningActor(pElem,FTransform()); )

	// Actor is running, allow internal construction
	CCLOK ( pElem->construct(); )
*/
	}	// BeginPlay

void AnLoc::EndPlay(const EEndPlayReason::Type rsn )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Called when play ends for this actor.
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AnLoc::EndPlay"));

	// Ensure shutdown
//	setRender ( NULL, L"" );

	// Clean up
	_RELEASE(pDctRen);

	// Base behaviour
	Super::EndPlay(rsn);
	}	// EndPlay

/*
HRESULT AnLoc :: getParent (	const WCHAR *pwLoc, 
										nElement **ppElem )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Retrieve the parent element.
	//
	//	PARAMETERS
	//		-	pwLoc is the location within the render dictionary.
	//		-	ppElem will receive the parent element
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr		= S_OK;
	IDictionary	*pDct	= NULL;
	WCHAR			*pw	= NULL;
	adtString	strLoc(pwLoc);
	adtValue		vL;
	adtIUnknown	unkV;
	size_t		len;

	// Elements are stored by path in the render dictionary.
	// Move up the path until a valid location/element is found.
//	dbgprintf ( L"AnLoc::getParent:%s\r\n", pwLoc );

	// Setup
	(*ppElem)	= NULL;

	// Remove trailing slash
	if ((len = strLoc.length()) > 0 && strLoc[len-1] == '/')
		strLoc.at(len-1) = '\0';

	// Remove last name in path to start search
//	CCLTRYE ( (pw = wcsrchr ( &strLoc.at(), '/' )) != NULL, E_UNEXPECTED );
//	CCLOK   ( *pw = '\0'; )

	// Retrieve the dictionary at location
	CCLTRY ( nspcLoadPath ( pDctRen, strLoc, vL ) );
	CCLTRY ( _QISAFE((unkV=vL),IID_IDictionary,&pDct) );

	// Skip one level up since element cannot be its own parent
	CCLTRY ( pDct->load ( strnRefPar, vL ) );
	_RELEASE(pDct);
	CCLTRY ( _QISAFE((unkV=((IUnknown *)(adtLong(vL).vlong))),IID_IDictionary,&pDct) );

	// Move up the hierarchy until parent element is found
	while (hr == S_OK && (*ppElem) == NULL && pDct != NULL)
		{
		IDictionary		*pDctChk = NULL;
		IIt				*pKeys	= NULL;
*/
/*
		// Debug
		dbgprintf ( L"AnLoc::parent:At %p (%p)\r\n", pDct, pDctRen );
		if (hr == S_OK)
			{
			if (pDct->keys ( &pKeys ) == S_OK)
				{
				while (pKeys->read ( vL ) == S_OK)
					{
					dbgprintf ( L"%p : %s\r\n", pDct, vL.pstr );
					pKeys->next();
					}	// while
				_RELEASE(pKeys);
				}	// if
			}	// if
*/
/*
		// Parent dictionary
		CCLTRY ( pDct->load ( strnRefPar, vL ) );
		_RELEASE(pDct);
		CCLTRY ( _QISAFE((unkV=((IUnknown *)(adtLong(vL).vlong))),IID_IDictionary,&pDct) );

		// Search for possible parents
		CCLTRY ( pDct->keys ( &pKeys ) );
		while (hr == S_OK && (*ppElem) == NULL && pKeys->read ( vL ) == S_OK)
			{
			// Check for _Location entry in sub-dictionaries
			if (	hr == S_OK												&& 
					adtString(vL)[0] != '_'								&&
					pDct->load ( vL, vL ) == S_OK						&&
					adtValue::type(vL) == VTYPE_UNK					&&
					(IUnknown *)(NULL) != (unkV = vL)				&&
					_QI(unkV,IID_IDictionary,&pDctChk) == S_OK	&&
					pDctChk->load ( strnRefLocn, vL ) == S_OK		&&
					adtValue::type(vL) == VTYPE_UNK )
				(*ppElem) = (nElement *)(vL.punk);

			// Clean up
			_RELEASE(pDctChk);
			pKeys->next();
			}	// while

		// Clean up
		_RELEASE(pKeys);
		}	// while

	// If no parent found, use root.
	if (hr != S_OK)
		{
		// Root element is stored directly off of render tree
		hr = S_OK;
		CCLTRY  ( pDctRen->load ( strnRefLocn, vL ) );
		CCLOK   ( (*ppElem) = (nElement *)(vL.punk); )
		}	// if

	// Parent found ?
	CCLTRYE ( (*ppElem) != NULL, ERROR_NOT_FOUND );

	// Clean up
	_RELEASE(pDct);

	// Debug
	if (hr != S_OK)
		dbgprintf ( L"AnLoc::parent:%s\r\n", pwLoc );

	return hr;
	}	// getParent
*/

void AnLoc :: init (	AnSpace *_pSpc, const WCHAR *_pwLoc, int _iIdx )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Initializer for the object.
	//
	//	PARAMETERS
	//		-	_pSpc is the master render object
	//		-	_pwLoc is the render location for this object
	//		-	_iIdx is the index of the root element in master render list
	//
	////////////////////////////////////////////////////////////////////////
	pSpc			= _pSpc;
	strLocRen	= _pwLoc;
	iIdx			= _iIdx;
	}	// init

void AnLoc :: onValue (	const WCHAR *pwRoot, 
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
	HRESULT		hr			= S_OK;
	size_t		len;
	adtString	strV;

	// Debug
	adtValue::toString(vV,strV);
//	CCLOK ( UE_LOG(LogTemp, Warning, TEXT("%s:%s:%s"), pwRoot, pwLoc, (LPCWSTR)strV ); )

	// This object watches for changes to '_Location's which identify
	// potential states that need to be rendered.
	if (	hr == S_OK									&& 
			(len = wcslen(pwLoc)) >= 9				&& 
			!WCASECMP(strnRefLocn,&pwLoc[len-9]) )
		{
		adtString	strLoc(pwLoc);

		// Remove _Location postfix for element
		CCLOK ( strLoc.at(strLoc.length()-9) = '\0'; )

		// New location ?
		if (	vV.vtype == VTYPE_STR						&&
				(len = wcslen(vV.pstr)) > 13				&&
				!WCASENCMP(vV.pstr,L"State/Visual/",13) )
			{
			AnElement	*pElem	= NULL;
			IDictionary	*pDct		= NULL;
			adtString	strDef(vV);
			adtString	strDefV	= &(strDef[13]);

			// Debug
			UE_LOG(LogTemp, Warning, TEXT("AnLoc:onValue:Location:%s:%s"),
						(LPCWSTR)strDef, (LPCWSTR)strLoc);

			// Create appropriate actor to handle the visual state definition
			if (!WCASECMP(strDefV,L"Shape/")) 
				pElem = Cast<AnShape>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
												this,AnShape::StaticClass(),FTransform()));
			else if (!WCASECMP(strDefV,L"Light/")) 
				pElem = Cast<AnLight>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
												this,AnLight::StaticClass(),FTransform()));
			else if (!WCASECMP(strDefV,L"Label/")) 
				pElem = Cast<AnLabel>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
												this,AnLabel::StaticClass(),FTransform()));
			else if (!WCASECMP(strDefV,L"Rule/Projectile/")) 
				pElem = Cast<AnProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
												this,AnProjectile::StaticClass(),FTransform()));
			else if (!WCASECMP(strDefV,L"Rule/Physics/")) 
				pElem = Cast<AnPhysics>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
												this,AnPhysics::StaticClass(),FTransform()));

			// Valid element ?
			CCLTRYE ( (pElem != NULL), E_NOTIMPL );

			// Establish owner and location for actor
			CCLOK ( pElem->init ( this, strLoc ); )

			// Store at location
			CCLTRY ( strLoc.append(L"Element") );
			CCLTRY ( nspcStoreValue ( pDctRen, strLoc, adtIUnknown(new nElementRef(pElem)) ) );

			// Continue spawning
			CCLOK ( UGameplayStatics::FinishSpawningActor(pElem,FTransform()); )
			}	// if

		// Empty location
		else if (adtValue::empty(vV))
			{
/*
			WCHAR			*pw	= NULL;
			adtIUnknown	unkV;
			adtValue		vL;
			adtString	strName;

			// Simple removal of the hierarchy for the element will destruct
			// the element object which will result is removal from the environment.

			// Isolate path and name
			CCLOK ( strLoc.at(strLoc.length()-1) = '\0'; )
			CCLTRYE( (pw = wcsrchr ( &strLoc.at(), '/' )) != NULL, E_UNEXPECTED );
			CCLOK  ( strName = pw+1; )
			CCLOK  ( *pw = '\0'; )

			// Debug
//			dbgprintf ( L"AnLoc::onValue:Remove %s from %s (%s,%s)\r\n",
//									(LPCWSTR) strName, (LPCWSTR) strLoc, pwRoot, pwLoc );

			// Check for existence of path
			if (nspcLoadPath ( pDctRen, strLoc, vL ) == S_OK)
				{
				IDictionary	*pDctL	= NULL;

				// Dictionary at level
				CCLTRY ( _QISAFE((unkV=vL),IID_IDictionary,&pDctL) );

				// Load the element dictionary
				if (hr == S_OK && pDctL->load ( strName, vL ) == S_OK)
					{
					IDictionary *pDctE	= NULL;

					// Element dictionary
					CCLTRY ( _QISAFE((unkV=vL),IID_IDictionary,&pDctE) );

					// Element object is stored at location
					if (hr == S_OK && pDctE->load ( strnRefLocn, vL ) == S_OK)
						{
//						nElement	*pElem	= (nElement *)(vL.punk);

						// Mark element for destruction and schedule for work on game loop
//						pElem->bRun = false;
//						pSpc->addMain ( pElem );
						}	// if

					// Clean up
					_RELEASE(pDctE);
					}	// if

				// Remove element hierarchy from render dictionary
				CCLOK ( dbgprintf ( L"AnLoc::onValue:Remove %s from %s\r\n",
												(LPCWSTR) strName, (LPCWSTR) strLoc ); )
				CCLOK ( pDctL->remove ( strName ); )

				// Clean up
				_RELEASE(pDctL);
				}	// if
*/
			}	// else if

		}	// if

	}	// onValue
/*
HRESULT AnLoc :: setRoot ( nElement *_pRoot )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Set the root element state.  Called by the element created
	//			in 'BeginPlay' from the worker thread during initialization.
	//
	//	PARAMETERS
	//		-	_pRoot is the root element
	//		-	_pLoc is the render location
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT	hr = S_OK;

	// Previous render setup
	if (pDctRen != NULL)
		{
		// Unlisten to previous location
		dbgprintf ( L"AnLoc::setRoot:Unlisten:%s\r\n",
							(LPCWSTR)strLocRen );
		if (strLocRen.length() > 0)
			pSpc->pCli->listen ( strLocRen, false );

		// Clean up
		_RELEASE(pCB);
		_RELEASE(pDctRen);
		}	// if

	// New render setup
	if (_pRoot != NULL)
		{
		// Create render dictionary
		CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pDctRen ) );

		// Calllback object for listens
		CCLTRYE ( (pCB = new nLocCB ( this )) != NULL, E_OUTOFMEMORY );
		_ADDREF(pCB);
		CCLTRY ( pCB->construct() );

		// Store the root component in dictionary so elements always have parent
		CCLTRY ( pDctRen->store ( strnRefLocn, adtIUnknown(_pRoot) ) );

		// Listen to location
		CCLTRY ( pSpc->pCli->listen ( strLocRen, true, pCB ) );
		}	// if

	return hr;
	}	// setRoot
*/
void AnLoc :: Tick( float DeltaTime )
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

	// Debug
//	WCHAR wBfr[100];
//	swprintf ( wBfr, L"AnSpaceRender::BeginPlay:%d %d\r\n", sizeof(ADTVALUE), sizeof(adtString) );
//	MessageBox ( NULL, wBfr, L"Note", MB_OK );

/*
	// Create render locations
	if (hr == S_OK)
		{

		// Render locations
		for (int i = 0;i < 1;++i)
			{
			// Loction
//			adtString	strLoc = (i == 0) ? L"/Apps/Auto/Default/External/" : L"/Apps/Auto/Default/GridVis/";
//			adtString	strLoc = (i == 0) ? L"/Apps/Auto/Default/ConnVis/" : L"/Apps/Auto/Default/TreeVis/";
			adtString	strLoc = (i == 0) ? L"/Apps/Auto/Default/TreeVis/" : L"/Apps/Auto/Default/ConnVis/";
//			adtString	strLoc = (i == 0) ? L"/Apps/Auto/Default/GridVis/" : L"/Apps/Auto/Default/ConnVis/";

			// Spawn render location
			CCLTRYE ( (pLoc = GetWorld()->SpawnActor<AnLoc>(
							FVector(0,0,0),FRotator(0,0,0),FActorSpawnParameters())) != NULL,
							E_OUTOFMEMORY );

			// Set render information
			pLoc->setRender ( this, strLoc );

			// Add to list
			pLstLoc->write ( adtLong((U64)pLoc) );
			}	// for

		}	// if

	// Test
	CCLTRY(pCli->store(L"/Apps/Auto/Default/Debug/Fire/Value",adtString(L"YouCanByteMeNow2")));
*/

/*
HRESULT AnLoct :: tick ( void )
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
	CCLTRYE ( pParent->evWork.wait ( -1 ), ERROR_TIMEOUT );

	// Execute scheduled work	
	while (	hr == S_OK		&& 
				pParent->bWork && 
				pParent->pWrkQ->isEmpty() != S_OK)
		{
		adtValue		vV;

		// Object for ticking
		CCLTRY ( pParent->pWrkIt->read ( vV ) );

		// Tick work
		CCLOK ( ((nElement *)(vV.punk))->workTick(); )

		// Move to next vlaue
		pParent->pWrkIt->next();
		}	// while

	// Execute scheduled stores
	while (	hr == S_OK		&& 
				pParent->bWork && 
				pParent->pStQ->isEmpty() != S_OK)
		{
		adtValue		vLoc,vV;

		// Load location and value
		if (	pParent->pStIt->read ( vLoc ) == S_OK &&
				adtValue::type(vLoc) == VTYPE_STR )
			{
			// Load value
			pParent->pStIt->next();
			if (pParent->pStIt->read ( vV ) == S_OK )
				pParent->pSpc->pCli->store ( vLoc.pstr, vV );
			}	// if

		// Move to next vlaue
		pParent->pStIt->next();
		}	// while

	// Keep running ?
	CCLTRYE ( pParent->bWork == true, S_FALSE );

	return hr;
	}	// tick

HRESULT AnLoct :: tickAbort ( void )
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
	pParent->bWork = false;
	pParent->evWork.signal();
	return S_OK;
	}	// tickAbort

HRESULT AnLoct :: tickBegin ( void )
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
	HRESULT		hr			= S_OK;

	// Initialize COM for thread
	CCLTRYE ( CoInitializeEx ( NULL, COINIT_MULTITHREADED ) == S_OK,
					GetLastError() );

	// Global mutext to sync between render locations
	if (pParent->pSpc->csRen.enter())
		{
		// Does client need started ?
		if (hr == S_OK && pParent->pSpc->pCli == NULL)
			{
			// Create client
			CCLTRYE((pParent->pSpc->pCli = new nSpaceClient()) != NULL, E_OUTOFMEMORY);

			// Open private namespace with own command line
			CCLTRY(pParent->pSpc->pCli->open(
						L"{ Execute Batch Location C:/dev/nspace/resource/record/install.nspc }", false, NULL));
//						L"{ Executef Batch Location C:/dev/nspace/resource/record/install.nspc }", false, NULL));
			}	// if

		// Does cache need created ?
		if (hr == S_OK && pParent->pSpc->pImgC == NULL)
			{
			// Construct a shared image cache object
			CCLTRYE ( (pParent->pSpc->pImgC = new nSpcImgCache ( pParent )) != NULL,
							E_OUTOFMEMORY );
			_ADDREF(pParent->pSpc->pImgC);
			CCLTRY ( pParent->pSpc->pImgC->construct() );
			}	// if

		// Clean up
		pParent->pSpc->csRen.leave();
		}	// if


	// Debug test
	if (hr == S_OK)
		pParent->pSpc->pCli->store(L"/apps/auto/default/Debug/Fire/Value",adtString(L"YouCanByteMeNow1"));

	// Debug
//	if (hr != S_OK)
//		MessageBox ( NULL, L"AnLoc::tickBegin", L"Error!", MB_OK );

	return hr;
	}	// tickBegin

HRESULT AnLoct :: tickEnd ( void )
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
	HRESULT			hr = S_OK;

	// Clean up
	CoUninitialize();

	return hr;
	}	// tickEnd
*/
/*
HRESULT AnLoc :: desc (	const WCHAR *pwPath, adtString &strRel,
												UnElement **ppElem, UnElement **ppPar )
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Retrieve the element and parent for the given path.
	//
	//	PARAMETERS
	//		-	pwPath is the path of the current location
	//		-	strRel will receive the relative path to descriptor
	//		-	ppElem will receive the element
	//		-	ppPar will receive the parent
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////
	HRESULT		hr		= S_OK;
	IDictionary	*pDct	= NULL;
	WCHAR			*pwL	= NULL;
	adtString	strPath(pwPath);
	adtValue		vL;
	adtIUnknown	unkV;
	size_t		len;

	// Setup
	(*ppElem)	= NULL;
	(*ppPar)		= NULL;
	strRel		= L"";

	// Queries at top level will fail so always make sure length of
	// path is greater than /_Location
	CCLTRYE ( (len = strPath.length()) > 10, E_INVALIDARG );

	//
	// Path preparation
	//

	// Remove trailing slash
	if (strPath[(len = strPath.length())-1] == '/')
		strPath.at(len-1) = '\0';

	// SPECIAL CASE: It is possible for caller to specify the location
	// of an element directly.  Remove '_Location' postfix so loop
	// below will simply find it right away.
	if (!WCASECMP(strnRefLocn,&strPath[len-9]))
		strPath.at(len-9) = '\0';

	// Now make sure there is a trailing slash for logic below
	if (strPath[(len = strPath.length())-1] != '/')
		hr = strPath.append ( L"/" );

	//
	// Search
	//

	// Load the initial level
	CCLTRY ( nspcLoadPath ( pDctRen, strPath, vL ) );
	CCLTRY ( _QISAFE((unkV=vL),IID_IDictionary,&pDct) );

	// Search up tree until element and parent are found
	CCLOK ( pwL = wcsrchr ( &strPath.at(), '/' ); )
	while (hr == S_OK && pDct != NULL && (*ppPar) == NULL)
		{
		// Does current level of a valid location element ?
		if (	hr == S_OK &&
				pDct->load ( strnRefLocn, vL ) == S_OK &&
				(vL.vtype == VTYPE_I8) )
			{
			// Element
			if ((*ppElem) == NULL)
				{
				// Use element
				(*ppElem) = (UnElement *) ((U64)adtLong(vL));

				// The postfix of the path is the relative location to descriptor
				CCLTRY ( adtValue::copy ( adtString(pwL+1), strRel ) );
				}	// if

			// Parent
			else if (*ppPar == NULL)
				(*ppPar) = (UnElement *) ((U64)adtLong(vL));
			}	// if

		// Move to parent
		CCLTRY ( pDct->load ( strnRefPar, vL ) );
		_RELEASE(pDct);
		CCLTRYE ( (IUnknown *)(NULL) != (unkV=(IUnknown *)(adtLong(vL).vlong)), E_UNEXPECTED );
		CCLTRY ( _QI(unkV,IID_IDictionary,&pDct) );

		// Keep track of path
		if (hr == S_OK && pwL != NULL)
			{
			WCHAR	*pwP;

			// Find previous last slash
			for (pwP = pwL-1;pwP != &strPath.at() && *pwP != '/';--pwP);

			// Previous slash is now last slash
			pwL = pwP;
			}	// if

		}	// while

	// Clean up
	_RELEASE(pDct);

	// Element and parent should always be found
	CCLTRYE ( (*ppElem) != NULL && (*ppPar != NULL), ERROR_NOT_FOUND );

	return hr;
	}	// desc
*/

/*
	IDictionary	*pDesc	= NULL;
//	IDictionary	*pPar		= NULL;
	WCHAR			*pLoc		= vL.pstr;
	AActor		*pA		= NULL;
	adtString	strV(vV);
	static		int j = 0;


	// New visuals are established when a supported '_Location' is
	// received.
	if (hr == S_OK && (len = wcslen(pLoc)) >= 9 && !WCASECMP(L"_Location",&pLoc[len-9]))
		{
		// Visuals
		if (!WCASECMP(strV,L"State/Visual/Label/"))
			{
			// Create descriptor for visual
			CCLTRY ( COCREATE ( L"Adt.Dictionary", IID_IDictionary, &pDesc ) );

			// Store information in descriptor
			CCLTRY ( pDesc->store ( adtString(L"_Location"), strV ) );

			// Create actor for visual
			CCLTRYE ( (pA = GetWorld()->SpawnActor<AnSpcLbl>(
							FVector(j++*100,0,+100),FRotator(0,0,0),FActorSpawnParameters())) != NULL, E_OUTOFMEMORY );

			// Store actor in dictionary
			CCLTRY ( pDesc->store ( strRefActor, adtLong((U64)pA) ) );

			// Store under path
			CCLTRY ( nspcStoreValue ( pDctRen, vL.pstr, adtIUnknown(pDesc) ) );
			}	// if

		}	// if

	// Non-location value.
	else if (hr == S_OK)
		{
		adtString	strRel;
		adtValue		vA;
		bool			bA1 = false;
		bool			bA2 = false;
		bool			bA3 = false;

		// Retrieve descriptor for path
		CCLTRY ( desc ( vL.pstr, strRel, &pDesc ) );

		// Extract actor if available
		if (hr == S_OK && pDesc->load ( strRefActor, vA ) == S_OK)
			pA = (AActor *)((U64)adtLong(vA));

		// Text for label
		if (!WCASECMP(strRel,L"Element/Label/OnFire/Value"))
			{
			AnSpcLbl *pLbl = Cast<AnSpcLbl> ( pA );

			// Set text
			if (pLbl != NULL)
				pLbl->pcLbl->SetText ( (LPCWSTR)strV );
			}	// if

		// Color
		else if (!WCASECMP(strRel,L"Element/Color/OnFire/Value"))
			{
			AnSpcLbl *pLbl = Cast<AnSpcLbl> ( pA );
			adtInt	iClr(vV);

			// Set render color
			if (pLbl != NULL)
				pLbl->pcLbl->SetTextRenderColor ( FColor ( iClr ) );
			}	// else if

		// Translation
		else	if (	(bA1 = !WCASECMP(strRel,L"Element/Transform/Translate/A1/OnFire/Value")) == true ||
						(bA2 = !WCASECMP(strRel,L"Element/Transform/Translate/A2/OnFire/Value")) == true ||
						(bA3 = !WCASECMP(strRel,L"Element/Transform/Translate/A3/OnFire/Value")) == true )
			{
			FVector		fV;
			adtDouble	dV(vV);

			// Get the current location
			fV = pA->GetRootComponent()->GetComponentLocation();

if (bA1 && dV != 0.0)
	dbgprintf ( L"Hi\r\n" );

			// Set component
			if			(bA1)	fV.X = dV;
			else if	(bA2)	fV.Y = dV;
			else				fV.Z = dV;

			// Set new location
			pA->GetRootComponent()->SetWorldLocation(fV);
			}	// if

		// Scaling
		else	if (	(bA1 = !WCASECMP(strRel,L"Element/Transform/Scale/A1/OnFire/Value")) == true ||
						(bA2 = !WCASECMP(strRel,L"Element/Transform/Scale/A2/OnFire/Value")) == true ||
						(bA3 = !WCASECMP(strRel,L"Element/Transform/Scale/A3/OnFire/Value")) == true )
			{
			FVector		fV;
			adtDouble	dV(vV);

			// Get the current location
			fV = pA->GetRootComponent()->GetComponentScale();

			// Set component
			if			(bA1)	fV.X = dV;
			else if	(bA2)	fV.Y = dV;
			else				fV.Z = dV;

			// Set new location
			pA->GetRootComponent()->SetWorldScale3D(fV);
			}	// if


		}	// else if

	// Clean up
	_RELEASE(pDesc);
*/
