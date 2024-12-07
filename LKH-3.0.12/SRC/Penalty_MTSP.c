#include "LKH.h"
#include "Segment.h"

// #define REDUNDANT_CHECK /* ONLY DEBUG: checks old and new and assert they are the same. */
#define printffff if (0) printff // when investigating speedup, set to if (1)

#ifdef CAVA_PENALTY
static GainType oldPenaltyMax;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
static GainType Penalty_MTSP_MINMAX_Old(void);
static GainType update_Penalty_MTSP_MINMAX(void);
static Node *setup_Node_MTSP_MINMAX(Node *);
static int setup_Penalty_MTSP_MINMAX(void);
static GainType calculate_DistanceSum(Node *initN, int Forward);
static void route_min_node_update(Node *t);

int SwapCaseCount = 0;
#define printfff if (SwapCaseCount == -1) printff
#endif

int TTTTT = 0;

GainType Penalty_MTSP_MINSUM()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType P = 0, DistanceSum;

    do {
        int Size = -1;
        do {
            Size++;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        } while ((N = NextN)->DepotId == 0);
        if (MTSPMaxSize < Dimension - Salesmen && Size > MTSPMaxSize)
            P += Size - MTSPMaxSize;
        if (MTSPMinSize >= 1 && Size < MTSPMinSize)
            P += MTSPMinSize - Size;
    } while (N != Depot);
    if (DistanceLimit != DBL_MAX) {
        do {
            if (P > CurrentPenalty ||
                (P == CurrentPenalty && CurrentGain <= 0))
                return CurrentPenalty + (CurrentGain > 0);
            DistanceSum = 0;
            do {
                NextN = Forward ? SUCC(N) : PREDD(N);
                DistanceSum += (C(N, NextN) - N->Pi - NextN->Pi) /
                    Precision;
                if (NextN->Id > DimensionSaved)
                    NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            } while ((N = NextN)->DepotId == 0);
            if (DistanceSum > DistanceLimit)
                P += DistanceSum - DistanceLimit;
        } while (N != Depot);
    }
    return P;
}

#ifdef CAVA_PENALTY

#ifdef REDUNDANT_CHECK
static GainType Penalty_MTSP_MINMAX_();
GainType Penalty_MTSP_MINMAX()
{
    assert(CurrentPenalty >= 0);
    GainType P2 = Penalty_MTSP_MINMAX_Old();
    GainType P1 = Penalty_MTSP_MINMAX_();

    int accepted1 = P1 < CurrentPenalty || (P1 == CurrentPenalty && CurrentGain > 0);
    int accepted2 = P2 < CurrentPenalty || (P2 == CurrentPenalty && CurrentGain > 0);
    // if (P1 != P2) {
    //     printffff("-- P1 (new): %d\n", P1);
    //     printffff("-- P2 (old): %d\n", P2);
    // }
    assert(P1 == P2);
    assert(accepted1 == accepted2);
    assert(P1 >= 0);
    return P1;
}

GainType Penalty_MTSP_MINMAX_()
#else
GainType Penalty_MTSP_MINMAX()
#endif
{
    GainType P = 0;
    int NC_idx;
    if (Swaps && cava_PetalsData)
    {
        // printff("%lld\n", SwapCaseCount);
        // SwapCaseCount++;
        //printfff("\nSwaps LOOP!! Swaps: %d\n", Swaps);
        NC_idx = setup_Penalty_MTSP_MINMAX();

        // printff("Routes start at:");
        // for (int i = 0; i < NC_idx; i++)
        // {
        //     printff("%d ", cava_NodeCache[i]->Id);
        //     printff("cava_PetalsData %d ", cava_NodeCache[i]->PetalId - cava_PetalsData);
        //     printff("minNode %d,", cava_NodeCache[i]->PetalId->minNode->Id);
        // }
        // printff("\n");

        GainType DistanceSum = 0;
        GainType MaxOldPenaltyInSwaps = 0;
        Node *N;
        int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;

        // Early exit 1
        for (int i = 1; i < Salesmen + 1; i++)
        {
            if (cava_PetalsData[i].flag)
            {
                MaxOldPenaltyInSwaps = MAX(MaxOldPenaltyInSwaps, cava_PetalsData[i].OldPenalty);
                cava_PetalsData[i].flag = 0;
            }
        }
        if (MaxOldPenaltyInSwaps < oldPenaltyMax)
        {
            printffff("MaxOldPenaltyInSwaps %d < oldPenaltyMax %d. Skipped.\n", MaxOldPenaltyInSwaps, oldPenaltyMax);
            return CurrentPenalty;
        }

        // Early exit 2
        if (oldPenaltyMax < CurrentPenalty)
        {
            int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
            Node *NextN;
            printff("\n\n====\nNC_idx: %d\n", NC_idx);
            printff("\n\nNodeCache: ");
            for (int i = 0; i < NC_idx; i++)
            {
                printff("%d ", cava_NodeCache[i]->Id);
            }
            printff("\n");
            while (NC_idx)
            {
                RouteData *petal = cava_NodeCache[--NC_idx]->PetalId;
                N = petal->minNode;
                if (N->Id > DimensionSaved)
                    N -= DimensionSaved;
                DistanceSum = N->prevCostSum;
                printff("SUCC(N), PREDD(N)=(%d, %d)\n", SUCC(N)->Id, PREDD(N)->Id);
                printff("start summing from %d, prevCostSum=%d. ", N->Id, DistanceSum);
                do
                {
                    NextN = Forward ? SUCC(N) : PREDD(N);
                    DistanceSum += MTSP_Penalty(N, NextN);
                    printff("added (%d, %d) cost %d, new sum=%d. ", N->Id, NextN->Id, MTSP_Penalty(N, NextN), DistanceSum);
                    N = NextN;
                } while (N->DepotId == 0);
                DistanceSum /= Precision;
                printff("\nDistanceSum newww: %d\n", DistanceSum);
                // if (DistanceSum > CurrentPenalty)
                //     return CurrentPenalty + (CurrentGain > 0);
            }
            // return CurrentPenalty;
        }

        
        if (oldPenaltyMax < CurrentPenalty)
        {
            // If the max route is not changed
            // the penalty will not change
            // because oldPenaltyMax was improved (smaller)
            printffff("oldPenaltyMax < CurrentPenalty. Skipped.\n");
            for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
            {
                for (int twice = 0; twice < 2; ++twice)
                {
                    if (twice > 0)
                        N = si->t2->PFlag ? si->t2 : si->t3;
                    else
                        N = si->t1->PFlag ? si->t1 : si->t4;
                    if (N->PFlag)
                    {
                        // Calculate the penalty for the route
                        printff("mid point %d\n", N->Id);
                        DistanceSum = calculate_DistanceSum(N, 1) + calculate_DistanceSum(N, 0);

                        printff("\nDistanceSum old %lld\n", DistanceSum);

                        // if (DistanceSum > CurrentPenalty)
                        //     return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
            printff("====\n");
            if (TTTTT++ > 2)
                exit(0);
            return CurrentPenalty;
        }

        // Main improved loop
        for (SwapRecord *si = SwapStack + Swaps - 1; si >= SwapStack; --si)
        {
            for (int twice = 0; twice < 2; ++twice)
            {
                if (twice > 0)
                    N = si->t2->PFlag ? si->t2 : si->t3;
                else
                    N = si->t1->PFlag ? si->t1 : si->t4;
                if (N->PFlag)
                {
                    // Calculate the penalty for the route
                    DistanceSum = calculate_DistanceSum(N, 1) + calculate_DistanceSum(N, 0);

                    //printfff("DistanceSum: %d (or %d) -> %d\n", savedN->PetalId->OldPenalty, N->PetalId->OldPenalty, DistanceSum);
                    P = MAX(P, DistanceSum);

                    if (P > oldPenaltyMax ||
                         (P == oldPenaltyMax && CurrentGain <= 0))
                    {
                        for (SwapRecord *s = si - 1; s >= SwapStack; --s)
                            s->t1->PFlag = s->t2->PFlag = s->t3->PFlag = s->t4->PFlag = 0;
                        printffff("P: %d > oldPenaltyMax: %d. Skipped.\n", P, oldPenaltyMax);
                        return CurrentPenalty + (CurrentGain > 0);
                    }
                }
            }
        }

        if (!CurrentPenalty)
            return P;

        printffff("Improved!\n");
        printffff("-- P: %d\n", P);
        printffff("-- oldPenaltyMax: %d\n", oldPenaltyMax);
        printffff("-- CurrentPenalty: %d\n", CurrentPenalty);
        return update_Penalty_MTSP_MINMAX(); //Improved!
    }
    else
    {
        printffff("Using the old penalty function.\n");
        if (!cava_NodeCache)
            cava_NodeCache = (Node **)calloc(Salesmen, sizeof(Node *));
        if (!cava_PetalsData)
            cava_PetalsData = (RouteData *)calloc(Salesmen + 1, sizeof(RouteData));
        return Penalty_MTSP_MINMAX_Old();
    }
}

static GainType calculate_DistanceSum(Node *initN, int Forward)
{
    // TODO: Cache GainType *SUC_N_SUM = calloc(Dimension, sizeof(GainType));
    //         and GainType *PRED_N_SUM = calloc(Dimension, sizeof(GainType));
    // This function directly returns Forward ? SUC_N_SUM[initN->Id] : PRED_N_SUM[initN->Id];
    // O(1) instead of O(n) for each call
    GainType DistanceSum = 0;
    Node *N = initN, *NextN;
    N->PFlag = 0;

    //Forward
    while (N->DepotId == 0)
    {
        NextN = Forward ? SUCC(N) : PREDD(N);
        DistanceSum += MTSP_Penalty(N, NextN);
        printff("added (%d, %d) cost %d, new sum=%d. ", N->Id, NextN->Id, MTSP_Penalty(N, NextN), DistanceSum);
        N = NextN;
        N->PFlag = 0;
    }
    DistanceSum /= Precision;
    return DistanceSum;
}

/* Returns 1 if only one route is involved in the current move */
static int setup_Penalty_MTSP_MINMAX()
{
    /*
        Setting up the initial penalty values for the routes involved
    */
    Node *N;
    int touched_routes = 0;
    oldPenaltyMax = 0;

    // Reset petals flags and minNode for the routes involved in the swaps
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        s->t1->PetalId->flag = s->t2->PetalId->flag = s->t3->PetalId->flag = s->t4->PetalId->flag = 0;
        s->t1->PetalId->minNode = s->t2->PetalId->minNode = s->t3->PetalId->minNode = s->t4->PetalId->minNode = NULL;
    }
    // printff("\nSwaps: %d\n", Swaps);
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        // If a move has involved the edge of an empty route an additional empty one needs to be counted
        Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
        // the edges (t1, t2) and (t3, t4) are removed,
        // and the new edges (t1, t4) and (t2, t3) are added to form a new tour.

        // setup_Node_MTSP_MINMAX(t1);
        // setup_Node_MTSP_MINMAX(t2);
        // setup_Node_MTSP_MINMAX(t3);
        // setup_Node_MTSP_MINMAX(t4);

        // if ((N = setup_Node_MTSP_MINMAX(s->t1)) != NULL)
        //     cava_NodeCache[touched_routes++] = N;
        // if ((N = setup_Node_MTSP_MINMAX(s->t4)) != NULL)
        //     cava_NodeCache[touched_routes++] = N;
        // if ((N = setup_Node_MTSP_MINMAX(s->t2)) != NULL)
        //     cava_NodeCache[touched_routes++] = N;
        // if ((N = setup_Node_MTSP_MINMAX(s->t3)) != NULL)
        //     cava_NodeCache[touched_routes++] = N;

        Node* NN;
        for (int twice = 0; twice < 2; ++twice)
        {
            if (twice > 0)
                N = !s->t2->DepotId ? s->t2 : s->t3;
            else
                N = !s->t1->DepotId ? s->t1 : s->t4;
            if (!N->DepotId)
            {
                printff("setting up node %d\n", N->Id);
                if ((NN = setup_Node_MTSP_MINMAX(N)) != NULL)
                {
                    printff("NN: %d\n", NN->Id);
                    cava_NodeCache[touched_routes++] = NN;
                }
            }
        }

        route_min_node_update(t1);
        route_min_node_update(t2);
        route_min_node_update(t3);
        route_min_node_update(t4);
    }
    // cava_NodeCache[touched_routes] = NULL;
    // Reset petals flags for next petal counting
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        Node *t1 = s->t1, *t2 = s->t2, *t3 = s->t3, *t4 = s->t4;
        int d1 = t1->DepotId, d2 = t2->DepotId, d3 = t3->DepotId, d4 = t4->DepotId;

        cava_PetalsData[d1].flag = cava_PetalsData[d2].flag =
            cava_PetalsData[d3].flag = cava_PetalsData[d4].flag = 0;

        t1->PetalId->flag = t2->PetalId->flag = t3->PetalId->flag = t4->PetalId->flag = 0;
    }
    // mark non-depot nodes involved in the swaps with PFlag = 1,
    // Depot nodes are marked with PFlag = 0 
    for (SwapRecord *s = SwapStack + Swaps - 1; s >= SwapStack; --s)
    {
        s->t1->PFlag = !s->t1->DepotId;
        s->t2->PFlag = !s->t2->DepotId;
        s->t3->PFlag = !s->t3->DepotId;
        s->t4->PFlag = !s->t4->DepotId;

        // mark the involved routes with flag = 1 for the early exit MaxOldPenaltyInSwaps < oldPenaltyMax
        s->t1->PetalId->flag = s->t2->PetalId->flag = s->t3->PetalId->flag = s->t4->PetalId->flag = 1;
    }
    printff("\ntouched_routes: %d", touched_routes);
    return touched_routes;
}

void route_min_node_update(Node *t)
{
    printf("\n");
    printff("route_min_node_update %d. ", t->Id);
    printff("related to petal %d. ", t->PetalId - cava_PetalsData);

    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = t, *NextN;
    do
    {
        NextN = Forward ? SUC(N) : PRED(N);
        printff("%d ", N->Id);
        //printff("(%d, %d)", N->Id, NextN->Id);
        N = NextN;
    } while (N->DepotId == 0);


    if (!t->PetalId->minNode)
    {
        printff("No minNode. Set to %d. Petal rank %d.", t->Id, t->PetalRank);
    } else if (t->PetalId->minNode->PetalRank > t->PetalRank)
    {
        printff("PetalRank %d < %d. Set to %d", t->PetalRank, t->PetalId->minNode->PetalRank, t->Id);
    }

    if (!t->PetalId->minNode || t->PetalId->minNode->PetalRank > t->PetalRank)
        t->PetalId->minNode = t;
}

static Node *setup_Node_MTSP_MINMAX(Node *N)
{
    /*
    The role of setup_Node_CVRP() is to ensure that each route's penalty is counted only once
    during the setup phase of the penalty calculation. This helps in accurately computing
    the previous penalty sum (oldPenaltyMax) for the routes involved in the current move,
    which is essential for determining if the new solution is an improvement.
    */
    if (!N->PetalId->flag) // check if the Node's Route has been Processed
    {
        oldPenaltyMax = MAX(oldPenaltyMax, N->PetalId->OldPenalty); // update the oldPenaltyMax
        //printfff("Route involved: %d : %d\n", N->PetalId - cava_PetalsData, N->PetalId->OldPenalty);
        N->PetalId->flag = 1; // Mark Route as Processed
        int DepotId = N->PetalId - cava_PetalsData;
        return DepotId == MTSPDepot ? Depot : NodeSet + Dim - 1 + DepotId;
    }
    return NULL;
}

/* Update route data when a new improving tour is found */
static GainType update_Penalty_MTSP_MINMAX()
{
    // updates the penalty metadata for each route in the solution
    // It iterates through all routes starting from the depot
    // and updates the OldPenalty in the RouteData structure for each route.
    // not the entire solution / max(all routes)
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    RouteData *CurrId;
    GainType Cost;
    GainType MaxCost = 0;
    int i = 1;
    // printfff("Update Penalty_MTSP_MINMAX (expensive func)\n");
    printff("\n");
    do
    {
        Cost = 0;
        N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        int PetalRank = 0;
        
        printff("PetalRank Route %d: ", N->DepotId);
        do {
            N->PetalId = CurrId;
            N->PetalRank = PetalRank++;
            printff("#%d: %d -> ", N->PetalRank, N->Id);
            N->prevCostSum = Cost;
            NextN = Forward ? SUCC(N) : PREDD(N);
            Cost += MTSP_Penalty(N, NextN);
            // printfff("%d -> %d: %d\n", N->Id, NextN->Id, (C(N, NextN) - N->Pi - NextN->Pi)/Precision);
        } while ((N = NextN)->DepotId == 0);
        printff("\n");
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        MaxCost = MAX(MaxCost, Cost);
        // printfff("-- New: route %d : %d\n", i++, Cost);
    } while (N != Depot);
    return MaxCost;
}

static GainType Penalty_MTSP_MINMAX_Old()
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    Node *N = Depot, *NextN;
    GainType Cost, MaxCost = MINUS_INFINITY;
    RouteData *CurrId;
    printff("\n");
    do {
        Cost = 0;
        N->PetalId = cava_PetalsData; // depots point to 0 cell
        CurrId = cava_PetalsData + N->DepotId;
        CurrId->OldPenalty = 0;
        int PetalRank = 0;
        printff("PetalRank Route %d: ", N->DepotId);
        do {
            N->PetalRank = PetalRank++;
            printff("#%d: %d -> ", N->PetalRank, N->Id);
            N->prevCostSum = Cost;
            N->PetalId = CurrId;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            Cost += MTSP_Penalty(N, NextN);
        } while ((N = NextN)->DepotId == 0);
        printff("\n");
        Cost /= Precision;
        CurrId->OldPenalty = Cost;
        if (Cost > MaxCost) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                MaxCost = CurrentPenalty + (CurrentGain > 0);
                // break; // maybe wrong?
            }
            else
            {
                MaxCost = Cost;
            }
        }
    } while (N != Depot);
    return MaxCost;
}
#else
GainType Penalty_MTSP_MINMAX()
{
    // Forward is true if the next node is not the first node of the next route
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N = Depot, *NextN;
    GainType Cost, MaxCost = MINUS_INFINITY;

    do {
        Cost = 0;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
            // Cost is the sum of the distances between the nodes in the route
            // minus the Pi values of the nodes
            Cost += MTSP_Penalty(N, NextN);
        } while ((N = NextN)->DepotId == 0);
        Cost /= Precision;
        if (Cost > MaxCost) {
            if (Cost > CurrentPenalty ||
                (Cost == CurrentPenalty && CurrentGain <= 0)) {
                return CurrentPenalty + (CurrentGain > 0);
            }
            MaxCost = Cost;
        }
    } while (N != Depot);
    // printfff("_Old() all routes : %d\n", P);
    return MaxCost;
}
#endif

GainType Penalty_MTSP_MINMAX_SIZE()
{
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    static Node *StartRoute = 0;
    Node *N, *NextN, *CurrentRoute;
    int Size, MaxSize = INT_MIN;

    if (!StartRoute)
        StartRoute = Depot;
    if (StartRoute->Id > DimensionSaved)
        StartRoute -= DimensionSaved;
    N = StartRoute;
    do {
        Size = 0;
        CurrentRoute = N;
        do {
            NextN = Forward ? SUCC(N) : PREDD(N);
            Size++;
            if (NextN->Id > DimensionSaved)
                NextN = Forward ? SUCC(NextN) : PREDD(NextN);
        } while ((N = NextN)->DepotId == 0);
        if (Size > MaxSize) {
            if (Size > CurrentPenalty ||
                (Size == CurrentPenalty && CurrentGain <= 0)) {
                StartRoute = CurrentRoute;
                return CurrentPenalty + (CurrentGain > 0);
            }
            MaxSize = Size;
        }
    } while (N != StartRoute);
    return MaxSize;
}
