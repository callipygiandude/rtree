#include "RList.c"
#include "Def.h"
#include "Data.c"
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

int compDimension;

Boolean RisOverLap(Region rgnRectOne, Region rgnRectTwo)
{   //returs TRUE if the two rectangles are over lapping
	int iDim = 0;
	for(iDim = 0; iDim < DIMENSION; iDim++)
		if(rgnRectOne->iTopRight[iDim] < rgnRectTwo->iBottomLeft[iDim] || rgnRectTwo->iTopRight[iDim] < rgnRectOne->iBottomLeft[iDim])
			return FALSE;
	return TRUE;
}

Boolean RisContains(Region rgnRect, DataPoint iData)
{   //return TRUE if the data point is contained in a rectangle
	int iCnt = 0;
	Boolean bIsContains = TRUE;
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   if((rgnRect->iBottomLeft[iCnt] > iData[iCnt]) || (rgnRect->iTopRight[iCnt] < iData[iCnt]))
        {   bIsContains = FALSE;
			break;
		}
	}
	return bIsContains;
}

Region RinitRgnRect(Dimension iBottomLeft, Dimension iTopRight)
{   //initializes the rectangle with the given bottom left and top right corners
    //if the values for the corners are specified NULL, initializes a rectangle with origin as co-ordinates for both corners.
	Region rgnRect = (Region)malloc(sizeof(*rgnRect));
    if(rgnRect == NULL)
		return NULL;
		
    if(iBottomLeft != NULL)
		rgnRect->iBottomLeft = iBottomLeft;
	else
    //rgnRect->iBottomLeft = (Dimension) calloc( DIMENSION, sizeof( dimension ) );
		rgnRect->iBottomLeft = (Dimension)malloc(sizeof(double)*DIMENSION);

	if(rgnRect->iBottomLeft == NULL)
    {   free(rgnRect);
		return NULL;
    }

	if(iTopRight != NULL)
		rgnRect->iTopRight = iTopRight;
	else
		rgnRect->iTopRight = (Dimension)malloc(sizeof(double)*DIMENSION);

	if(rgnRect->iTopRight == NULL)
    {   
    	if(rgnRect->iBottomLeft == NULL)
			free(rgnRect);
		return NULL;
	}

	return rgnRect;
}

void RsetRect(RLstNd lstNd, RTreeNode tnInfo)
{   // copies the data in the tree node tnInfo to lstNd
    int iCnt = 0;
    switch(tnInfo->ndType)
    {   case INTNODE:
		//incase of internal node copy the bottom left and top right corners
		for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   lstNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
			lstNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
     	}
		break;
        case EXTNODE:
		// in case of external node copy the data element
		for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   lstNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = tnInfo->tdInfo->dataClstElem->iData[iCnt];
			lstNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = tnInfo->tdInfo->dataClstElem->iData[iCnt];
		}
		break;
	}

	return;
}

RTreeNode RinitIntNd(Dimension iBottomLeft, Dimension iTopRight)
{   //intializes internal node of a Tree with rectangle whose bottom left and topright corners are given

	Region rgnRect = RinitRgnRect(iBottomLeft, iTopRight);
	// initializes a rectangle with the given coordonates for the bottom left and top right corners

	RTreeData tdInfo = (RTreeData)malloc(sizeof(*tdInfo));

	if(tdInfo == NULL)
		return NULL;

	tdInfo->rgnRect = rgnRect;
    RTreeNode tnInfo = (RTreeNode)malloc(sizeof(*tnInfo));

	if(tnInfo == NULL)
		return NULL;

	tnInfo->ndType = INTNODE;
	tnInfo->tdInfo = tdInfo;

	return tnInfo;
}

RTreeNode RinitExtNd(Data dataClstElem)
{  // initializes the external node of a tree with Data
   if(dataClstElem == NULL)
		return NULL;

	RTreeNode tnInfo = (RTreeNode)malloc(sizeof(*tnInfo));

	if(tnInfo == NULL)
		return NULL;

	RTreeData tdInfo = (RTreeData)malloc(sizeof(*tdInfo));
    if(tdInfo == NULL)
    {   free(tnInfo);
		return NULL;
	}

	tdInfo->dataClstElem = dataClstElem;	// Data
    tnInfo->ndType = EXTNODE;	// external node
	tnInfo->tdInfo = tdInfo;

	return tnInfo;
}

RHdrNd RcreateRoot(RHdrNd hdrNdTree)
{   //in case of root split this is called to create a new root
    RHdrNd hdrNdRoot = RinitHdrNd();
    Dimension iBottomLeft = (Dimension)calloc(DIMENSION, sizeof(dimension));
	Dimension iTopRight = (Dimension)calloc(DIMENSION,sizeof(dimension));

	RLstNd lstNdTemp = hdrNdTree->ptrFirstNd;
	int iCnt = 0;
	Boolean bIsFirst = TRUE;

   //set the bottom left and top right corners for the new root
	while(lstNdTemp != NULL)
    {	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   if(bIsFirst)
            {   iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
         	}
			else
            {   if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < iBottomLeft[iCnt])
					iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > iTopRight[iCnt])
					iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
    	    }
		}
		lstNdTemp = lstNdTemp->ptrNextNd;
		bIsFirst = FALSE;
	}

//initialize a node with the bottomleft and top right corners obtained and insert into the list
	hdrNdRoot->ptrFirstNd = RinitLstNd(RinitIntNd(iBottomLeft, iTopRight));
    hdrNdRoot->ptrFirstNd->ptrChildLst = hdrNdTree;
	hdrNdRoot->uiCnt = 1;

	return hdrNdRoot;
}

double Rarea(Region rgnRect)
{   //calcluates the area of rectangle
    if(rgnRect == NULL)
		return 0;
    double dArea = 1;
	int iCnt = 0;
    //multiply values along each dimension
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
		dArea = dArea * (rgnRect->iTopRight[iCnt] - rgnRect->iBottomLeft[iCnt]);
	return dArea;
}

Boolean RexpansionArea(Region rgnRect, RTreeNode tnInfo, Double ptrDMinExp, Region rgnNewRect)
{   // calculates if the area by which the rgnRect should be enlarged so as to include the tnInfo is less than the value pointed by ptrDMinExp and return TRUE and assigns rgnNewRect with the new enlarged rectangle.
    int iCnt = 0;
    Region rgnRectTemp = RinitRgnRect(NULL, NULL);
    for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   switch(tnInfo->ndType)
        {   case INTNODE:
            //assign least of bottom left corner along each dimension to rgnRectTemp
			rgnRectTemp->iTopRight[iCnt] = (tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > rgnRect->iTopRight[iCnt]) ? tnInfo->tdInfo->rgnRect->iTopRight[iCnt] : rgnRect->iTopRight[iCnt];
			rgnRectTemp->iBottomLeft[iCnt] = (tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < rgnRect->iBottomLeft[iCnt]) ? tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] : rgnRect->iBottomLeft[iCnt];
            break;

		    case EXTNODE:
           //assign maximum of top right corner along each dimension to rgnRectTemp
			rgnRectTemp->iTopRight[iCnt] = (tnInfo->tdInfo->dataClstElem->iData[iCnt] > rgnRect->iTopRight[iCnt]) ? tnInfo->tdInfo->dataClstElem->iData[iCnt] : rgnRect->iTopRight[iCnt];
			rgnRectTemp->iBottomLeft[iCnt] = (tnInfo->tdInfo->dataClstElem->iData[iCnt] < rgnRect->iBottomLeft[iCnt]) ? tnInfo->tdInfo->dataClstElem->iData[iCnt] : rgnRect->iBottomLeft[iCnt];
			break;
		}
	}
    //calculate the difference in area for new rectangle and old rectangle
	double dExp = Rarea(rgnRectTemp) - Rarea(rgnRect);
    //in case there no value to compare ( -1 ) or incase the value is less than the value to be comparedcopy the rgnRectTemp to rgnRectNew to Return it.
	if(*ptrDMinExp == -1 || dExp <= *ptrDMinExp)
    {   if(dExp == *ptrDMinExp)
			*ptrDMinExp = 0 - dExp;
		else
			*ptrDMinExp = dExp;
        for(iCnt =0; iCnt< DIMENSION; iCnt++)
        {	rgnNewRect->iBottomLeft[iCnt] = rgnRectTemp->iBottomLeft[iCnt];
			rgnNewRect->iTopRight[iCnt] = rgnRectTemp->iTopRight[iCnt];
		}
		free(rgnRectTemp->iBottomLeft);
		free(rgnRectTemp->iTopRight);
		free(rgnRectTemp);
    //area to be enlarged is less than the previous value
		return TRUE;
	}

	free(rgnRectTemp->iBottomLeft);
	free(rgnRectTemp->iTopRight);
	free(rgnRectTemp);
    //area to be enlarged is not less than the previous value
	return FALSE;
}

Region getMBR(DataHdr inputData)
{
	double* min = (double*) malloc(DIMENSION*sizeof(double));
	double* max = (double*) malloc(DIMENSION*sizeof(double));
	for(int iCnt = 0; iCnt < DIMENSION; iCnt++)
	{
		min[iCnt] = DBL_MAX;
		max[iCnt] = DBL_MIN;
	}
	Region temp = RinitRgnRect(min, max);
	
    for(int i = 0; i < inputData->uiCnt; i++)
	{   
		Data curr = inputData->dataClstElem+i;
    	for(int iCnt = 0; iCnt < DIMENSION; iCnt++)
   		{
           	temp->iTopRight[iCnt] = (curr->iData[iCnt] > temp->iTopRight[iCnt]) ? curr->iData[iCnt] : temp->iTopRight[iCnt];
			temp->iBottomLeft[iCnt] = (curr->iData[iCnt] < temp->iBottomLeft[iCnt]) ? curr->iData[iCnt] : temp->iBottomLeft[iCnt];
		}
		
	}
	
	return temp;
}

int comparator(const void* a, const void* b) {
	Data x = (Data) a;
	Data y = (Data) b;
	
	return x->iData[compDimension] < y->iData[compDimension] ? -1 : 1;
}

void sortPoints(Data points, int numPoints, int dimensionToSortOn) 
{
	compDimension = dimensionToSortOn;
	qsort((void*) points, numPoints, sizeof(struct data), comparator);
}

void bestSplit(DataHdr inputData, long m, DataHdr L, DataHdr H)
{
	printf("pre\n");
	printDataLst(inputData);
    int n = inputData->uiCnt;         
    int p = (int)ceil((double)n / m) - 1; 
    double c = DBL_MAX;               

    DataHdr tempL = initDataHdr(n);   
    DataHdr tempH = initDataHdr(n);

    for (int d = 0; d < DIMENSION; d++) 
    {
        sortPoints(inputData->dataClstElem, n, d);
		printf("post %d\n", d);
		printDataLst(inputData);
		

        for (int i = 1; i <= p; i++)
        {
            //freeDataList(tempL);
            tempL = initDataHdr(n);
			int j;
            for (j = 0; j < i * m && j < n; j++)
            {
                insertDataLstElem(tempL, inputData->dataClstElem[j].iData);
            }
			// printDataLst(inputData);
            Region b1 = getMBR(tempL);
			printf("tempL\n");
			printDataLst(tempL);
            //freeDataList(tempH);
            tempH = initDataHdr(n);
            for (; j < n; j++)
            {
                insertDataLstElem(tempH, inputData->dataClstElem[j].iData);
            }
            Region b2 = getMBR(tempH);
			printf("tempH\n");
			printDataLst(tempH);
            double t = Rarea(b1) + Rarea(b2);

            if (t < c)
            {
                c = t;
                //freeDataList(L);
                //freeDataList(H);
                // L = initDataHdr(tempL->uiCnt);
				for (int j = 0; j < tempL->uiCnt; j++)
				{
					insertDataLstElem(L, tempL->dataClstElem[j].iData);
				}


				// H = initDataHdr(tempH->uiCnt);
				for (int j = 0; j < tempH->uiCnt; j++)
				{
					insertDataLstElem(H, tempH->dataClstElem[j].iData);
				}
            }

            free(b1);
            free(b2);
        }
    }

    // freeDataList(tempL);
    // freeDataList(tempH);
}

DataHdr* Partition(DataHdr inputData, long m)
{
	DataHdr* P = malloc(RMAXENTRIES * sizeof(DataHdr));
	DataHdr* LD;
	DataHdr* HD; 
	int n = inputData->uiCnt;
	if(n <= m)
	{
		P[0] = inputData;
		P[1] = NULL;
		return P;
	}
	DataHdr L = initDataHdr(n);
	DataHdr H = initDataHdr(n);
	// printDataLst(inputData);
	bestSplit(inputData, m, L, H);
	int i = 0, j = 0;
	LD = Partition(L, m);
	HD = Partition(H, m);
	
	while(LD[i] != NULL)
	{
		P[i] = LD[i];
		i++;
	}
	while(HD[j] != NULL)
	{
		P[i] = HD[j];
		i++; j++;
	}
	return P;
}

RLstNd loadChunk(DataHdr inputData, int h)
{
	RLstNd ret;// = RinitHdrNd();
	if(h == 0)
	{
		RTreeNode info = RinitExtNd(inputData->dataClstElem);
		ret = RinitLstNd(info);
	}
	else
	{
		long m = (long) pow(RMAXENTRIES, h);
		// printDataLst(inputData);
		DataHdr* P = Partition(inputData, m);
		Region MBR = getMBR(inputData);
		RTreeNode info = RinitIntNd(MBR->iBottomLeft, MBR->iTopRight);
		info->tdInfo->rgnRect = MBR;
		ret = RinitLstNd(info);

		ret->ptrChildLst = RinitHdrNd();
		ret->ptrChildLst->ptrParentNd = ret;
		ret->ptrChildLst->uiCnt = 0;
        RLstNd curr = NULL;
        for (int i = 0; i < RMAXENTRIES && P[i] != NULL; i++)
        {
            RLstNd childNode = loadChunk(P[i], h - 1); 
            if (curr == NULL)
            {
                ret->ptrChildLst->ptrFirstNd = childNode;
                curr = childNode;
            }
            else
            {
                curr->ptrNextNd = childNode;
                curr = curr->ptrNextNd;
            }
            ret->ptrChildLst->uiCnt++;
        }
	}
	return ret;
}

RHdrNd RbuildRTree(DataHdr inputData)
{   // build RTree with the list of elements passed
	
	int h = ceil(log2(inputData->uiCnt) / log2(RMAXENTRIES)) -1;
	if(h < 0)
		h = 0;
	RHdrNd tree = RinitHdrNd();
	RinsertLstNd(tree, loadChunk(inputData, h));
	return tree;
}

void RprintTree(RHdrNd hdrNdTree)
{   //travers along the tree and print the tree
	RLstNd lstNdTemp = hdrNdTree->ptrFirstNd;
	int iCnt = 0;
	static int iIndent = 0;
	iIndent++;

	while(lstNdTemp != NULL)
    {   for(iCnt = 0; iCnt < iIndent; iCnt++)
			printf("---");

		if(lstNdTemp->tnInfo->ndType == INTNODE)
        {   //printf("i hav %d children..\n",lstNdTemp->ptrChildLst->uiCnt);
			printf(" BottomLeft: ");
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%lf ", lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
            printf(" TopRight: ");
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%lf ", lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
            printf("\n");
			printf("i hav %d children..\n",lstNdTemp->ptrChildLst->uiCnt);
			RprintTree(lstNdTemp->ptrChildLst);
		}
		else
        {   printf(" DataPoint: ");
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%lf ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
			printf("\n");
		}
		lstNdTemp = lstNdTemp->ptrNextNd;
	}
	iIndent--;
	return;
}

int main()
{
    RMINENTRIES = 2;
    RMAXENTRIES = 4;

	DataHdr inputData = readData("testbig.txt");
	//DataHdr inputData = readData("3D_spatial_network_transformed");
	// Region mbr = getMBR(inputData);
	// printf("hi%lf %lf -> %lf %lf\n", mbr->iBottomLeft[0], mbr->iBottomLeft[1], mbr->iTopRight[0], mbr->iTopRight[1]);
    RHdrNd rtree = (RHdrNd) RbuildRTree(inputData);
    RprintTree(rtree);
	printf("hi\n");
}
